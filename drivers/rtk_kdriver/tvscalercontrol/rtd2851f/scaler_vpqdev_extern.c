#ifndef BUILD_QUICK_SHOW
/*Kernel Header file*/
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/fs.h>		/* everything... */
#include <linux/cdev.h>
#include <linux/platform_device.h>

#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/pageremap.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <linux/hrtimer.h>
#include <linux/vmalloc.h>
#ifdef CONFIG_COMPAT
#include <linux/compat.h>
#define to_user_ptr(x)          compat_ptr((unsigned long) x)
#else
#define to_user_ptr(x)          ((void* __user)(x)) // convert 32 bit value to user pointer
#endif
#endif
/*RBUS Header file*/
#include <scaler/scalerCommon.h>

#include <scaler/VIP_Define_Structure.h>
/*TVScaler Header file*/
#include "tvscalercontrol/io/ioregdrv.h"
#include <tvscalercontrol/scaler/scalercolorlib.h>
#include <tvscalercontrol/scaler/scalercolorlib_tv001.h>
#include <tvscalercontrol/scaler/scalercolorlib_tv002.h>
#include <tvscalercontrol/vip/scalerColor.h>
#include <tvscalercontrol/vip/scalerColor_tv006.h>
//#include <tvscalercontrol/vip/scalerColor_tv010.h>
#include <tvscalercontrol/vip/viptable.h>
#include <scaler_vpqmemcdev.h>
#include "scaler_vpqdev_extern.h"
#include "scaler_vpqdev.h"

#include "scaler_vscdev.h"
#ifndef BUILD_QUICK_SHOW
#include "ioctrl/vpq/vpq_extern_cmd_id.h"
#endif

#include <tvscalercontrol/vip/pq_rpc.h>
//#include <mach/RPCDriver.h>
#include <rbus/sys_reg_reg.h>
#include <rbus/pll27x_reg_reg.h>
#include <tvscalercontrol/vip/vip_reg_def.h>
#include <tvscalercontrol/vip/gibi_od.h>
#include <tvscalercontrol/vip/ultrazoom.h>
#include <tvscalercontrol/vip/valc.h>
#include <tvscalercontrol/scalerdrv/scalermemory.h>
#include <tvscalercontrol/scalerdrv/scalerip.h>
#include <tvscalercontrol/scalerdrv/scalerdisplay.h>
#include <tvscalercontrol/i3ddma/i3ddma_drv.h>
//#include <rbus/rbus_DesignSpec_MISC_GPIOReg.h>
#include <rtd_log/rtd_module_log.h>
#include <tvscalercontrol/scaler/scalerstruct.h>
#ifndef BUILD_QUICK_SHOW
#include <tvscalercontrol/vip/scalerColor_tv043.h>
#endif
#include <tvscalercontrol/scalerdrv/zoom_smoothtoggle.h>
#include <rtk_kdriver/quick_show/quick_show.h> //pq_quick_show
#include <dprx/dprxfun.h>
#include <tvscalercontrol/hdmirx/hdmifun.h>
#define TAG_NAME "VPQEX"
#include <tvscalercontrol/vdc/yc_separation_vpq_table.h>
#include <rbus/osdovl_reg.h>
#ifdef	CONFIG_DOLBY_IDK_1_6_1_1
#include <dolby_gdbs_delay.h>
#endif
#ifndef BUILD_QUICK_SHOW

static dev_t vpqex_devno;/*vpq device number*/
static struct cdev vpqex_cdev;
#endif
static unsigned char PQ_Dev_Extern_Status = PQ_DEV_EXTERN_NOTHING;
extern struct semaphore ICM_Semaphore;
extern struct semaphore I_RGB2YUV_Semaphore;
extern struct semaphore LC_Semaphore;
extern DRV_film_mode film_mode;
unsigned char g_tv002_demo_fun_flag = 0;
unsigned int g_pq_bypass_lv_ctrl_by_AP = 0xFFFFFFFF;
extern unsigned char g_picmode_Cinema;
unsigned char ioctl_extern_cmd_stop[256] = {0};
extern signed char OD_DMA_TBL_Delta_Saved_fromBin[3][VIP_OD_TBL_SIZE];
//TV030
unsigned char GammaBlendingFlag=0;
unsigned char ColorSpaceMode=0;
//unsigned int tICM_bin[VIP_ICM_TBL_X*VIP_ICM_TBL_Y*VIP_ICM_TBL_Z];
ICM_H_7axis_table icm_7_axis_data;

void Check_smooth_toggle_update_flag(unsigned char display);
void zoom_update_scaler_info_from_vo_smooth_toggle(unsigned char display, unsigned char scaler_before);
extern unsigned char scalerVIP_access_tv002_style_demo_flag(unsigned char access_mode, unsigned char *pFlag);
unsigned char cur_src = 0;
VIP_APDEM_PTG_CTRL Hal_APDEM_PTG_CTRL = {0};

#if defined(CONFIG_RTK_AI_DRV)
TV001_COLOR_TEMP_DATA_S vpqex_color_temp = {0};
#endif
#ifndef BUILD_QUICK_SHOW
extern VIP_D3DLUTbyCS_6axis_TBL *gD3DLUTbyCS_6axis_TBL;
extern VIP_D3DLUTbyCS_Decode_TBL *gD3DLUTbyCS_Decode_TBL;
extern VIP_D3DLUTbyCS_6axis *gD3DLUTbyCS_6axis_tmp;
extern GAMUT_3D_LUT_17x17x17_T *gD3DLUTbyCS_Decode_TBL_tmp;

extern int D_3DLUT[VIP_D_3D_LUT_SIZE][VIP_8VERTEX_RGB_MAX];
extern int D_3DLUT_Reseult[VIP_D_3D_LUT_SIZE][VIP_8VERTEX_RGB_MAX];
#ifdef yc_tbl
extern unsigned int AV_VD_Table[8][4];
extern unsigned int ATV_VD_Table[8][4];
extern unsigned char VPQ_YCBCR_DELAY_TABLE[PRJ_NUM_MAX][YCSEP_STATUS_MAX][INPUT_SRC_MAX][FORMAT_MAX][YCDLEAY_MAX];
#endif

#if 1 // tconless_kdriver_common_flow_
static unsigned short VPQEX_VPQ_CalcCRC16(unsigned char *buf, unsigned int nByte)
{
	unsigned int i, j;
	unsigned short crc = 0x0000;
	unsigned short crc_temp_1, crc_temp_2, crc_temp_3;
	unsigned char data_buffer;
	//unsigned int length;
	//unsigned char *buf;
	unsigned int checksum;
	unsigned short crcData;
	i = 0;
	checksum = 0;
	while(nByte--){
		data_buffer = buf[i++];
		checksum += data_buffer;
		crc_temp_1 = 0;
		crc_temp_2 = 0;
		crc_temp_3 = 0;
#if 1
		for (j=0;j<8;j++) {
			crc_temp_1 = ((crc>>15)^data_buffer)&0x0001; // result 1 CRC MSB[15]^DATA BIT 0
			crc_temp_2 = ((crc>>1)^crc_temp_1)&0x0001;	// result 2 --> CRC[1]^result 1
			crc_temp_3 = ((crc>>14)^crc_temp_1)&0x0001; // result 3 --> CRC[14]^result 1
			crc = crc<<1;
			crc &= 0x7FFA;		// clear CRC[0]/[2]/[15]
			crc |= crc_temp_1;	// load result 1 to CRC[0]
			crc |= (crc_temp_2<<2);	// load result 2 to CRC[2]
			crc |= (crc_temp_3<<15);	// load result 3 to CRC[15]
			data_buffer >>=1;
		}
#endif		
	}

	crcData = crc;

	return crcData;
}
#endif	

#if 1	// tconless_demura
extern VIP_DeMura_TBL DeMura_TBL;
/*
static unsigned short VPQEX_VPQ_CalcCRC16(unsigned char *buf, unsigned int nByte)
{
	unsigned int i, j;
	unsigned short crc = 0x0000;
	unsigned short crc_temp_1, crc_temp_2, crc_temp_3;
	unsigned char data_buffer;
	//unsigned int length;
	//unsigned char *buf;
	unsigned int checksum;
	unsigned short crcData;
	i = 0;
	checksum = 0;
	while(nByte--){
		data_buffer = buf[i++];
		checksum += data_buffer;
		crc_temp_1 = 0;
		crc_temp_2 = 0;
		crc_temp_3 = 0;
#if 1
		for (j=0;j<8;j++) {
			crc_temp_1 = ((crc>>15)^data_buffer)&0x0001; // result 1 CRC MSB[15]^DATA BIT 0
			crc_temp_2 = ((crc>>1)^crc_temp_1)&0x0001;	// result 2 --> CRC[1]^result 1
			crc_temp_3 = ((crc>>14)^crc_temp_1)&0x0001; // result 3 --> CRC[14]^result 1
			crc = crc<<1;
			crc &= 0x7FFA;		// clear CRC[0]/[2]/[15]
			crc |= crc_temp_1;	// load result 1 to CRC[0]
			crc |= (crc_temp_2<<2);	// load result 2 to CRC[2]
			crc |= (crc_temp_3<<15);	// load result 3 to CRC[15]
			data_buffer >>=1;
		}
#endif		
	}

	crcData = crc;

	return crcData;
}
*/
int VPQEX_internal_Demura_TBL_Max_check(short *demura_decode_tbl, unsigned char mode, unsigned char isSeparateRGB, unsigned char layerNum)
{
	unsigned int i,j,k,m;
	unsigned short tbl_height, tbl_width, tbl_ch;
	short tmp = 0;
	
	if( mode == DeMura_TBL_481x271 )
	{
		tbl_height = 271;
		tbl_width = 481;
	}
	else // DeMura_TBL_241x136
	{
		tbl_height = 136;
		tbl_width = 241;
	}
	tbl_ch = isSeparateRGB ? 3 : 1;

	for( i=0; i<tbl_height; i++ ) // line
	{
		for( j=0; j<tbl_width; j++ )
		{
			for( k=0; k<tbl_ch; k++ )
			{
				for( m=0; m<layerNum; m++ )
				{
					tmp = *(demura_decode_tbl + m * tbl_height * tbl_width * tbl_ch + (i * tbl_width + j) * tbl_ch + k);
					if (tmp > 127) 
					{						
						rtd_pr_vpq_info("PDM, val check, val=%d, layer=%d, line=%d, col=%d, ch=%d,\n", tmp, m, i, j, k); 
						*(demura_decode_tbl + m * tbl_height * tbl_width * tbl_ch + (i * tbl_width + j) * tbl_ch + k) = 127;				
					}
					else if (tmp < (-128))
					{
						rtd_pr_vpq_info("PDM, val check, val=%d, layer=%d, line=%d, col=%d, ch=%d,\n", tmp, m, i, j, k); 					
						*(demura_decode_tbl + m * tbl_height * tbl_width * tbl_ch + (i * tbl_width + j) * tbl_ch + k) = -128;
					}
				}
			}
		}
	}
	return 0;
}

static int VPQEX_internal_Demura_TBL_Diff_check_get_condition(unsigned char mode, unsigned char isSeparateRGB,
	unsigned short *tbl_width, unsigned short *tbl_height, unsigned short *tbl_ch)
{

	if( mode == DeMura_TBL_481x271 )
	{
		*tbl_height = 271;
		*tbl_width = 481;
	}
	else // DeMura_TBL_241x136
	{
		*tbl_height = 136;
		*tbl_width = 241;
	}
	*tbl_ch = isSeparateRGB ? 3 : 1;

	return 0;
}

int VPQEX_internal_Demura_TBL_Diff_check(short *demura_decode_tbl, unsigned char mode, unsigned char isSeparateRGB, unsigned char layerNum)
{
	unsigned int i,j,k,m;
	unsigned short tbl_height, tbl_width, tbl_ch;
	short tmp0 = 0, tmp1 = 0, diff = 0;
	
	VPQEX_internal_Demura_TBL_Diff_check_get_condition(mode, isSeparateRGB, &tbl_width, &tbl_height, &tbl_ch);

	for( i=0; i<tbl_height; i++ ) // line
	{
		for( j=0; j<tbl_width; j++ )
		{
			for( k=0; k<tbl_ch; k++ )
			{
				for( m=0; m<layerNum; m++ )
				{
					if(j != 0) // first col 
					{						
						tmp1 = *(demura_decode_tbl + m * tbl_height * tbl_width * tbl_ch + (i * tbl_width + j) * tbl_ch + k);
						tmp0 = *(demura_decode_tbl + m * tbl_height * tbl_width * tbl_ch + (i * tbl_width + (j - 1)) * tbl_ch + k);
						diff = tmp1 - tmp0;
						if (diff > 127) 
						{						
							rtd_pr_vpq_info("PDM, val check, val=%d,%d, layer=%d, line=%d, col=%d, ch=%d,\n", tmp0, tmp1, m, i, j, k); 
							*(demura_decode_tbl + m * tbl_height * tbl_width * tbl_ch + (i * tbl_width + j) * tbl_ch + k) = tmp0 + 127;				
						}
						else if (diff < (-128))
						{
							rtd_pr_vpq_info("PDM, val check, val=%d,%d, layer=%d, line=%d, col=%d, ch=%d,\n", tmp0, tmp1, m, i, j, k); 					
							*(demura_decode_tbl + m * tbl_height * tbl_width * tbl_ch + (i * tbl_width + j) * tbl_ch + k) = tmp0 - 128;				
						}
						
					}
				}
			}
		}
	}
	return 0;
}

static int VPQEX_internal_Demura_CTRL_setting_TH_R(unsigned short *layer_th)
{
	short tmp0, tmp1, tmp2, tmp3, tmp4, tmp5;

	tmp0 = ((layer_th[1] - layer_th[0]) == 0)?(1):(layer_th[1] - layer_th[0]);
	tmp1 = ((layer_th[2] - layer_th[1]) == 0)?(1):(layer_th[2] - layer_th[1]);
	tmp2 = ((layer_th[3] - layer_th[2]) == 0)?(1):(layer_th[3] - layer_th[2]);
	tmp3 = ((layer_th[4] - layer_th[3]) == 0)?(1):(layer_th[4] - layer_th[3]);
	tmp4 = ((layer_th[5] - layer_th[4]) == 0)?(1):(layer_th[5] - layer_th[4]);
	tmp5 = ((layer_th[6] - layer_th[5]) == 0)?(1):(layer_th[6] - layer_th[5]);
	
	DeMura_TBL.DeMura_CTRL_TBL.demura_r_lower_cutoff = layer_th[0];	
	DeMura_TBL.DeMura_CTRL_TBL.demura_r_l_level = layer_th[1];		
	DeMura_TBL.DeMura_CTRL_TBL.demura_r_m_level = layer_th[2];		
	//DeMura_TBL.DeMura_CTRL_TBL.demura_r_m2_level = layer_th[3];		
	//DeMura_TBL.DeMura_CTRL_TBL.demura_r_m3_level = layer_th[4];		
	DeMura_TBL.DeMura_CTRL_TBL.demura_r_h_level = layer_th[5];	
	DeMura_TBL.DeMura_CTRL_TBL.demura_r_h_keep_level = layer_th[5];	// high keep = high lv
	DeMura_TBL.DeMura_CTRL_TBL.demura_r_upper_cutoff = layer_th[6];	

	DeMura_TBL.DeMura_CTRL_TBL.r_div_factor_l = 65536 / (4 * tmp0);
	DeMura_TBL.DeMura_CTRL_TBL.r_div_factor_l_m = 65536 / (4 * tmp1);	
	//DeMura_TBL.DeMura_CTRL_TBL.r_div_factor_m1_m2 = 65536 / (4 * tmp2);	
	//DeMura_TBL.DeMura_CTRL_TBL.r_div_factor_m2_m3 = 65536 / (4 * tmp3); 	
	DeMura_TBL.DeMura_CTRL_TBL.r_div_factor_m_h = 65536 / (4 * tmp4);	
	DeMura_TBL.DeMura_CTRL_TBL.r_div_factor_h = 65536 / (4 * tmp5);
	return 0;
}

static int VPQEX_internal_Demura_CTRL_setting_TH_G(unsigned short *layer_th)
{
	short tmp0, tmp1, tmp2, tmp3, tmp4, tmp5;

	tmp0 = ((layer_th[1] - layer_th[0]) == 0)?(1):(layer_th[1] - layer_th[0]);
	tmp1 = ((layer_th[2] - layer_th[1]) == 0)?(1):(layer_th[2] - layer_th[1]);
	tmp2 = ((layer_th[3] - layer_th[2]) == 0)?(1):(layer_th[3] - layer_th[2]);
	tmp3 = ((layer_th[4] - layer_th[3]) == 0)?(1):(layer_th[4] - layer_th[3]);
	tmp4 = ((layer_th[5] - layer_th[4]) == 0)?(1):(layer_th[5] - layer_th[4]);
	tmp5 = ((layer_th[6] - layer_th[5]) == 0)?(1):(layer_th[6] - layer_th[5]);
	
	DeMura_TBL.DeMura_CTRL_TBL.demura_g_lower_cutoff = layer_th[0];	
	DeMura_TBL.DeMura_CTRL_TBL.demura_g_l_level = layer_th[1];		
	DeMura_TBL.DeMura_CTRL_TBL.demura_g_m_level = layer_th[2];		
	//DeMura_TBL.DeMura_CTRL_TBL.demura_g_m2_level = layer_th[3];		
	//DeMura_TBL.DeMura_CTRL_TBL.demura_g_m3_level = layer_th[4];		
	DeMura_TBL.DeMura_CTRL_TBL.demura_g_h_level = layer_th[5];
	DeMura_TBL.DeMura_CTRL_TBL.demura_g_h_keep_level = layer_th[5];	// high keep = high lv
	DeMura_TBL.DeMura_CTRL_TBL.demura_g_upper_cutoff = layer_th[6];	

	DeMura_TBL.DeMura_CTRL_TBL.g_div_factor_l = 65536 / (4 * tmp0);
	DeMura_TBL.DeMura_CTRL_TBL.g_div_factor_l_m = 65536 / (4 * tmp1);	
	//DeMura_TBL.DeMura_CTRL_TBL.g_div_factor_m1_m2 = 65536 / (4 * tmp2);	
	//DeMura_TBL.DeMura_CTRL_TBL.g_div_factor_m2_m3 = 65536 / (4 * tmp3); 	
	DeMura_TBL.DeMura_CTRL_TBL.g_div_factor_m_h = 65536 / (4 * tmp4);	
	DeMura_TBL.DeMura_CTRL_TBL.g_div_factor_h = 65536 / (4 * tmp5);
	return 0;

}

static int VPQEX_internal_Demura_CTRL_setting_TH_B(unsigned short *layer_th)
{
	short tmp0, tmp1, tmp2, tmp3, tmp4, tmp5;

	tmp0 = ((layer_th[1] - layer_th[0]) == 0)?(1):(layer_th[1] - layer_th[0]);
	tmp1 = ((layer_th[2] - layer_th[1]) == 0)?(1):(layer_th[2] - layer_th[1]);
	tmp2 = ((layer_th[3] - layer_th[2]) == 0)?(1):(layer_th[3] - layer_th[2]);
	tmp3 = ((layer_th[4] - layer_th[3]) == 0)?(1):(layer_th[4] - layer_th[3]);
	tmp4 = ((layer_th[5] - layer_th[4]) == 0)?(1):(layer_th[5] - layer_th[4]);
	tmp5 = ((layer_th[6] - layer_th[5]) == 0)?(1):(layer_th[6] - layer_th[5]);
	
	DeMura_TBL.DeMura_CTRL_TBL.demura_b_lower_cutoff = layer_th[0];	
	DeMura_TBL.DeMura_CTRL_TBL.demura_b_l_level = layer_th[1];		
	DeMura_TBL.DeMura_CTRL_TBL.demura_b_m_level = layer_th[2];		
	//DeMura_TBL.DeMura_CTRL_TBL.demura_b_m2_level = layer_th[3];		
	//DeMura_TBL.DeMura_CTRL_TBL.demura_b_m3_level = layer_th[4];		
	DeMura_TBL.DeMura_CTRL_TBL.demura_b_h_level = layer_th[5];
	DeMura_TBL.DeMura_CTRL_TBL.demura_b_h_keep_level = layer_th[5];	// high keep = high lv
	DeMura_TBL.DeMura_CTRL_TBL.demura_b_upper_cutoff = layer_th[6];	

	DeMura_TBL.DeMura_CTRL_TBL.b_div_factor_l = 65536 / (4 * tmp0);
	DeMura_TBL.DeMura_CTRL_TBL.b_div_factor_l_m = 65536 / (4 * tmp1);	
	//DeMura_TBL.DeMura_CTRL_TBL.b_div_factor_m1_m2 = 65536 / (4 * tmp2);	
	//DeMura_TBL.DeMura_CTRL_TBL.b_div_factor_m2_m3 = 65536 / (4 * tmp3); 	
	DeMura_TBL.DeMura_CTRL_TBL.b_div_factor_m_h = 65536 / (4 * tmp4);	
	DeMura_TBL.DeMura_CTRL_TBL.b_div_factor_h = 65536 / (4 * tmp5);
	return 0;

}

int VPQEX_internal_Demura_CTRL_setting(short *demura_decode_tbl, unsigned char mode, unsigned char isSeparateRGB, 
	unsigned char layerNum, unsigned short *layer_th_r, unsigned short *layer_th_g, unsigned short *layer_th_b)
{
	DeMura_TBL.table_mode = DeMura_TBL_481x271;
		
	DeMura_TBL.DeMura_CTRL_TBL.demura_en = 1;					
	//DeMura_TBL.DeMura_CTRL_TBL.demura_table_level = layerNum - 3;			
	DeMura_TBL.DeMura_CTRL_TBL.demura_rgb_table_seperate = isSeparateRGB;	
	//DeMura_TBL.DeMura_CTRL_TBL.demura_block_size;			
	DeMura_TBL.table_mode = DeMura_TBL_481x271;			

	//DeMura_TBL.DeMura_CTRL_TBL.demura_sample_mode;
	//DeMura_TBL.DeMura_CTRL_TBL.demura_gain_shiftbit;	
	//DeMura_TBL.DeMura_CTRL_TBL.demura_8bitmode_en = 1;		
	//DeMura_TBL.DeMura_CTRL_TBL.demura_block_size_v; 	
	//DeMura_TBL.DeMura_CTRL_TBL.demura_block_size_h; 	
	
	// level settings,
	VPQEX_internal_Demura_CTRL_setting_TH_R(layer_th_r);
	VPQEX_internal_Demura_CTRL_setting_TH_G(layer_th_g);
	VPQEX_internal_Demura_CTRL_setting_TH_B(layer_th_b);
	return 0;
}

// CSOTPDM
static const unsigned short VPQEX_CSOTPDM_CRC16_TABLE[256] = {
0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011, 0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072, 0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2, 0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1, 0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192, 0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1, 0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151, 0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132, 0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312, 0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371, 0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1, 0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2, 0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291, 0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2, 0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252, 0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231, 0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202
};
unsigned short VPQEX_CSOTPDM_CalcCRC16(unsigned char *dataBuffer, unsigned int nByte)
{
	unsigned char dat;
	unsigned int offset = 0;
	unsigned short crcData = 0;
	while(nByte--){
		dat = (unsigned char)(crcData>>8);
		crcData <<= 8;
		crcData ^= VPQEX_CSOTPDM_CRC16_TABLE[dat^dataBuffer[offset++]];
	}
	return crcData;
}

#define CSOTPDM_to_Demura_FMT(arg) (((arg>2048)?(arg-4096):(arg))>>2)

// File Hader
#define VIP_CSOTPDM_u32MagicNum 0x0
#define VIP_CSOTPDM_name 0x4
#define VIP_CSOTPDM_version 0x12
#define VIP_CSOTPDM_sectionNum 0x14
#define VIP_CSOTPDM_size 0x18
#define VIP_CSOTPDM_File_total_CRC 0x1C
#define VIP_CSOTPDM_File_Header_CRC 0x1E
// CSOTPDM single tcon
// section header 1
#define VIP_CSOTPDM_type_0 0x20
#define VIP_CSOTPDM_offset_0 0x24
#define VIP_CSOTPDM_size_0 0x28
#define VIP_CSOTPDM_para_section_CRC_0 0x2C

// section header 2
#define VIP_CSOTPDM_type_1 0x30
#define VIP_CSOTPDM_offset_1 0x34
#define VIP_CSOTPDM_size_1 0x38
#define VIP_CSOTPDM_LUT_section_CRC_1 0x3C

// section header 3	(for dual t-con)
#define VIP_CSOTPDM_type_2 0x40
#define VIP_CSOTPDM_offset_2 0x44
#define VIP_CSOTPDM_size_2 0x48
#define VIP_CSOTPDM_LUT_section_CRC_2 0x4C

// DEMURA_SINGLE parameter
#define VIP_CSOTPDM_SINGLE_enDEMURAMode 0x40
#define VIP_CSOTPDM_SINGLE_u8PlaneNum 0x41
#define VIP_CSOTPDM_SINGLE_u8HblockSize 0x42
#define VIP_CSOTPDM_SINGLE_u8VblockSize 0x42
#define VIP_CSOTPDM_SINGLE_u16HLutNum 0x44
#define VIP_CSOTPDM_SINGLE_u16VLutNum 0x46
#define VIP_CSOTPDM_SINGLE_u8IntBitWidth 0x48
#define VIP_CSOTPDM_SINGLE_u8u8DecimalsBitWidth 0x49
#define VIP_CSOTPDM_SINGLE_u8TwoChipEN 0x4A
#define VIP_CSOTPDM_SINGLE_u8CompressEN 0x4B
#define VIP_CSOTPDM_SINGLE_u16CompValGainR 0x4C
#define VIP_CSOTPDM_SINGLE_u16CompValGainG 0x4E
#define VIP_CSOTPDM_SINGLE_u16CompValGainB 0x50

#define VIP_CSOTPDM_SINGLE_u16CompValOffsetR 0x54
#define VIP_CSOTPDM_SINGLE_u16CompValOffsetG 0x56
#define VIP_CSOTPDM_SINGLE_u16CompValOffsetB 0x58

#define VIP_CSOTPDM_SINGLE_u16BlackLimitR 0x5C
#define VIP_CSOTPDM_SINGLE_u16BlackLimitG 0x5E
#define VIP_CSOTPDM_SINGLE_u16BlackLimitB 0x60

#define VIP_CSOTPDM_SINGLE_u16WhiteLimitR 0x64
#define VIP_CSOTPDM_SINGLE_u16WhiteLimitG 0x66
#define VIP_CSOTPDM_SINGLE_u16WhiteLimitB 0x68

#define VIP_CSOTPDM_SINGLE_u16Plane1LevelR 0x6C
#define VIP_CSOTPDM_SINGLE_u16Plane2LevelR 0x6E
#define VIP_CSOTPDM_SINGLE_u16Plane3LevelR 0x70
#define VIP_CSOTPDM_SINGLE_u16Plane4LevelR 0x72
#define VIP_CSOTPDM_SINGLE_u16Plane5LevelR 0x74
#define VIP_CSOTPDM_SINGLE_u16Plane6LevelR 0x76
#define VIP_CSOTPDM_SINGLE_u16Plane7LevelR 0x78
#define VIP_CSOTPDM_SINGLE_u16Plane8LevelR 0x7A
#define VIP_CSOTPDM_SINGLE_u16Plane9LevelR 0x7C
#define VIP_CSOTPDM_SINGLE_u16Plane1LevelG 0x7E
#define VIP_CSOTPDM_SINGLE_u16Plane2LevelG 0x80
#define VIP_CSOTPDM_SINGLE_u16Plane3LevelG 0x82
#define VIP_CSOTPDM_SINGLE_u16Plane4LevelG 0x84
#define VIP_CSOTPDM_SINGLE_u16Plane5LevelG 0x86
#define VIP_CSOTPDM_SINGLE_u16Plane6LevelG 0x88
#define VIP_CSOTPDM_SINGLE_u16Plane7LevelG 0x8A
#define VIP_CSOTPDM_SINGLE_u16Plane8LevelG 0x8C
#define VIP_CSOTPDM_SINGLE_u16Plane9LevelG 0x8E
#define VIP_CSOTPDM_SINGLE_u16Plane1LevelB 0x90
#define VIP_CSOTPDM_SINGLE_u16Plane2LevelB 0x92
#define VIP_CSOTPDM_SINGLE_u16Plane3LevelB 0x94
#define VIP_CSOTPDM_SINGLE_u16Plane4LevelB 0x96
#define VIP_CSOTPDM_SINGLE_u16Plane5LevelB 0x98
#define VIP_CSOTPDM_SINGLE_u16Plane6LevelB 0x9A
#define VIP_CSOTPDM_SINGLE_u16Plane7LevelB 0x9C
#define VIP_CSOTPDM_SINGLE_u16Plane8LevelB 0x9E
#define VIP_CSOTPDM_SINGLE_u16Plane9LevelB 0xA0

#define VIP_CSOTPDM_SINGLE_u16PlaneB1SlopeR 0xB4
#define VIP_CSOTPDM_SINGLE_u16Plane12SlopeR 0xB6
#define VIP_CSOTPDM_SINGLE_u16Plane23SlopeR 0xB8
#define VIP_CSOTPDM_SINGLE_u16Plane34SlopeR 0xBA
#define VIP_CSOTPDM_SINGLE_u16Plane45SlopeR 0xBC
#define VIP_CSOTPDM_SINGLE_u16Plane56SlopeR 0xBE
#define VIP_CSOTPDM_SINGLE_u16Plane67SlopeR 0xC0
#define VIP_CSOTPDM_SINGLE_u16Plane78SlopeR 0xC2
#define VIP_CSOTPDM_SINGLE_u16Plane89SlopeR 0xC4
#define VIP_CSOTPDM_SINGLE_u16Plane9WSlopeR 0xC6
#define VIP_CSOTPDM_SINGLE_u16PlaneB1SlopeG 0xC8
#define VIP_CSOTPDM_SINGLE_u16Plane12SlopeG 0xCA
#define VIP_CSOTPDM_SINGLE_u16Plane23SlopeG 0xCC
#define VIP_CSOTPDM_SINGLE_u16Plane34SlopeG 0xCE
#define VIP_CSOTPDM_SINGLE_u16Plane45SlopeG 0xD0
#define VIP_CSOTPDM_SINGLE_u16Plane56SlopeG 0xD2
#define VIP_CSOTPDM_SINGLE_u16Plane67SlopeG 0xD4
#define VIP_CSOTPDM_SINGLE_u16Plane78SlopeG 0xD6
#define VIP_CSOTPDM_SINGLE_u16Plane89SlopeG 0xD8
#define VIP_CSOTPDM_SINGLE_u16Plane9WSlopeG 0xDA
#define VIP_CSOTPDM_SINGLE_u16PlaneB1SlopeB 0xDC
#define VIP_CSOTPDM_SINGLE_u16Plane12SlopeB 0xDE
#define VIP_CSOTPDM_SINGLE_u16Plane23SlopeB 0xE0
#define VIP_CSOTPDM_SINGLE_u16Plane34SlopeB 0xE2
#define VIP_CSOTPDM_SINGLE_u16Plane45SlopeB 0xE4
#define VIP_CSOTPDM_SINGLE_u16Plane56SlopeB 0xE6
#define VIP_CSOTPDM_SINGLE_u16Plane67SlopeB 0xE8
#define VIP_CSOTPDM_SINGLE_u16Plane78SlopeB 0xEA
#define VIP_CSOTPDM_SINGLE_u16Plane89SlopeB 0xEC
#define VIP_CSOTPDM_SINGLE_u16Plane9WSlopeB 0xEE

#define VIP_CSOTPDM_SINGLE_LUT_STA 0x110
#define VIP_CSOTPDM_SINGLE_LUT_Line_Size 0x310
#define VIP_CSOTPDM_SINGLE_LUT_Line_Addr_Get(MaxLayerNum, LayerNum, LineNum) (VIP_CSOTPDM_SINGLE_LUT_STA+((LineNum*MaxLayerNum+LayerNum)*VIP_CSOTPDM_SINGLE_LUT_Line_Size))
#define VIP_CSOTPDM_SINGLE_TOTAL_SIZE 0x9BAE0

// DEMURA_Dual parameter
#define VIP_CSOTPDM_DUAL_enDEMURAMode 0x50
#define VIP_CSOTPDM_DUAL_u8PlaneNum 0x51
#define VIP_CSOTPDM_DUAL_u8HblockSize 0x52
#define VIP_CSOTPDM_DUAL_u8VblockSize 0x52
#define VIP_CSOTPDM_DUAL_u16HLutNum 0x54
#define VIP_CSOTPDM_DUAL_u16VLutNum 0x56
#define VIP_CSOTPDM_DUAL_u8IntBitWidth 0x58
#define VIP_CSOTPDM_DUAL_u8u8DecimalsBitWidth 0x59
#define VIP_CSOTPDM_DUAL_u8TwoChipEN 0x5A
#define VIP_CSOTPDM_DUAL_u8CompressEN 0x5B
#define VIP_CSOTPDM_DUAL_u16CompValGainR 0x5C
#define VIP_CSOTPDM_DUAL_u16CompValGainG 0x5E
#define VIP_CSOTPDM_DUAL_u16CompValGainB 0x60

#define VIP_CSOTPDM_DUAL_u16CompValOffsetR 0x64
#define VIP_CSOTPDM_DUAL_u16CompValOffsetG 0x66
#define VIP_CSOTPDM_DUAL_u16CompValOffsetB 0x68

#define VIP_CSOTPDM_DUAL_u16BlackLimitR 0x6C
#define VIP_CSOTPDM_DUAL_u16BlackLimitG 0x6E
#define VIP_CSOTPDM_DUAL_u16BlackLimitB 0x70

#define VIP_CSOTPDM_DUAL_u16WhiteLimitR 0x74
#define VIP_CSOTPDM_DUAL_u16WhiteLimitG 0x76
#define VIP_CSOTPDM_DUAL_u16WhiteLimitB 0x78

#define VIP_CSOTPDM_DUAL_u16Plane1LevelR 0x7C
#define VIP_CSOTPDM_DUAL_u16Plane2LevelR 0x7E
#define VIP_CSOTPDM_DUAL_u16Plane3LevelR 0x80
#define VIP_CSOTPDM_DUAL_u16Plane4LevelR 0x82
#define VIP_CSOTPDM_DUAL_u16Plane5LevelR 0x84
#define VIP_CSOTPDM_DUAL_u16Plane6LevelR 0x86
#define VIP_CSOTPDM_DUAL_u16Plane7LevelR 0x88
#define VIP_CSOTPDM_DUAL_u16Plane8LevelR 0x8A
#define VIP_CSOTPDM_DUAL_u16Plane9LevelR 0x8C
#define VIP_CSOTPDM_DUAL_u16Plane1LevelG 0x8E
#define VIP_CSOTPDM_DUAL_u16Plane2LevelG 0x90
#define VIP_CSOTPDM_DUAL_u16Plane3LevelG 0x92
#define VIP_CSOTPDM_DUAL_u16Plane4LevelG 0x94
#define VIP_CSOTPDM_DUAL_u16Plane5LevelG 0x96
#define VIP_CSOTPDM_DUAL_u16Plane6LevelG 0x98
#define VIP_CSOTPDM_DUAL_u16Plane7LevelG 0x9A
#define VIP_CSOTPDM_DUAL_u16Plane8LevelG 0x9C
#define VIP_CSOTPDM_DUAL_u16Plane9LevelG 0x9E
#define VIP_CSOTPDM_DUAL_u16Plane1LevelB 0xA0
#define VIP_CSOTPDM_DUAL_u16Plane2LevelB 0xA2
#define VIP_CSOTPDM_DUAL_u16Plane3LevelB 0xA4
#define VIP_CSOTPDM_DUAL_u16Plane4LevelB 0xA6
#define VIP_CSOTPDM_DUAL_u16Plane5LevelB 0xA8
#define VIP_CSOTPDM_DUAL_u16Plane6LevelB 0xAA
#define VIP_CSOTPDM_DUAL_u16Plane7LevelB 0xAC
#define VIP_CSOTPDM_DUAL_u16Plane8LevelB 0xAE
#define VIP_CSOTPDM_DUAL_u16Plane9LevelB 0xB0

#define VIP_CSOTPDM_DUAL_u16PlaneB1SlopeR 0xC4
#define VIP_CSOTPDM_DUAL_u16Plane12SlopeR 0xC6
#define VIP_CSOTPDM_DUAL_u16Plane23SlopeR 0xC8
#define VIP_CSOTPDM_DUAL_u16Plane34SlopeR 0xCA
#define VIP_CSOTPDM_DUAL_u16Plane45SlopeR 0xCC
#define VIP_CSOTPDM_DUAL_u16Plane56SlopeR 0xCE
#define VIP_CSOTPDM_DUAL_u16Plane67SlopeR 0xD0
#define VIP_CSOTPDM_DUAL_u16Plane78SlopeR 0xD2
#define VIP_CSOTPDM_DUAL_u16Plane89SlopeR 0xD4
#define VIP_CSOTPDM_DUAL_u16Plane9WSlopeR 0xD6
#define VIP_CSOTPDM_DUAL_u16PlaneB1SlopeG 0xD8
#define VIP_CSOTPDM_DUAL_u16Plane12SlopeG 0xDA
#define VIP_CSOTPDM_DUAL_u16Plane23SlopeG 0xDC
#define VIP_CSOTPDM_DUAL_u16Plane34SlopeG 0xDE
#define VIP_CSOTPDM_DUAL_u16Plane45SlopeG 0xE0
#define VIP_CSOTPDM_DUAL_u16Plane56SlopeG 0xE2
#define VIP_CSOTPDM_DUAL_u16Plane67SlopeG 0xE4
#define VIP_CSOTPDM_DUAL_u16Plane78SlopeG 0xE6
#define VIP_CSOTPDM_DUAL_u16Plane89SlopeG 0xE8
#define VIP_CSOTPDM_DUAL_u16Plane9WSlopeG 0xEA
#define VIP_CSOTPDM_DUAL_u16PlaneB1SlopeB 0xEC
#define VIP_CSOTPDM_DUAL_u16Plane12SlopeB 0xEE
#define VIP_CSOTPDM_DUAL_u16Plane23SlopeB 0xF0
#define VIP_CSOTPDM_DUAL_u16Plane34SlopeB 0xF2
#define VIP_CSOTPDM_DUAL_u16Plane45SlopeB 0xF4
#define VIP_CSOTPDM_DUAL_u16Plane56SlopeB 0xF6
#define VIP_CSOTPDM_DUAL_u16Plane67SlopeB 0xF8
#define VIP_CSOTPDM_DUAL_u16Plane78SlopeB 0xFA
#define VIP_CSOTPDM_DUAL_u16Plane89SlopeB 0xFC
#define VIP_CSOTPDM_DUAL_u16Plane9WSlopeB 0xFE

#define VIP_CSOTPDM_DUAL_LUT_LEFT_STA 0x120
#define VIP_CSOTPDM_DUAL_LUT_RIGHT_STA 0x4F770
#define VIP_CSOTPDM_DUAL_LUT_Line_Size 0x190
#define VIP_CSOTPDM_DUAL_LUT_Line_LEFT_Addr_Get(MaxLayerNum, LayerNum, LineNum) (VIP_CSOTPDM_DUAL_LUT_LEFT_STA+((LineNum*MaxLayerNum+LayerNum)*VIP_CSOTPDM_DUAL_LUT_Line_Size))
#define VIP_CSOTPDM_DUAL_LUT_Line_RIGHT_Addr_Get(MaxLayerNum, LayerNum, LineNum) (VIP_CSOTPDM_DUAL_LUT_RIGHT_STA+((LineNum*MaxLayerNum+LayerNum)*VIP_CSOTPDM_DUAL_LUT_Line_Size))
#define VIP_CSOTPDM_DUAL_TOTAL_SIZE 0x9EDC0

typedef enum _VPQ_CSOTPDM_MODE {
	VPQ_CSOTPDM_SINGLE_T_con_3_Plane_Y_MODE_481x271 = 0,
	VPQ_CSOTPDM_DUAL_T_con_3_Plane_Y_MODE_481x271,

	VPQ_CSOTPDM_MODE_MAX,

	VPQ_CSOTPDM_MODE_Fail_UNKNOW_TYPE = -1,
	
} VPQ_CSOTPDM_MODE;

static int VPQEX_CSOTPDM_to_internal_Demura_single_tcon_section_check_Y_Mode_3_Plane(unsigned char *pBuff_tmp)
{
	unsigned char enDEMURAMode, PlaneNum, /*HblockSize, VblockSize,*/ IntBitWidth, DecimalsBitWidth, TwoChipEN, CompressEN;
	unsigned short HLutNum, VLutNum;
	int ret;
	
	enDEMURAMode = *(pBuff_tmp+VIP_CSOTPDM_SINGLE_enDEMURAMode);
	PlaneNum = *(pBuff_tmp+VIP_CSOTPDM_SINGLE_u8PlaneNum);
	//HblockSize = *(pBuff_tmp+VIP_CSOTPDM_DUAL_u8HblockSize);
	//VblockSize = *(pBuff_tmp+VIP_CSOTPDM_DUAL_u8VblockSize);
	HLutNum = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16HLutNum));
	VLutNum = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16VLutNum));
	IntBitWidth = *(pBuff_tmp+VIP_CSOTPDM_SINGLE_u8IntBitWidth);
	DecimalsBitWidth = *(pBuff_tmp+VIP_CSOTPDM_SINGLE_u8u8DecimalsBitWidth);
	TwoChipEN = *(pBuff_tmp+VIP_CSOTPDM_SINGLE_u8TwoChipEN);
	CompressEN = *(pBuff_tmp+VIP_CSOTPDM_SINGLE_u8CompressEN);
#if 0	
	if ((enDEMURAMode == 0x00) && /*(PlaneNum <= 0x05) && (PlaneNum >= 0x03)*/ (PlaneNum == 0x03) && /*(HblockSize == 0x08) && 
		(VblockSize == 0x08) &&*/ (HLutNum == 0x01E1) && (VLutNum == 0x010F) && (IntBitWidth == 0x0A) && 
		(DecimalsBitWidth == 0x02) && (TwoChipEN == 0x00) && (CompressEN == 0x00)) 
#endif
	if ((enDEMURAMode == 0x00) && (PlaneNum == 0x03) && (HLutNum == 0x01E1) && (VLutNum == 0x010F) && (IntBitWidth == 0x0A) && 
		(DecimalsBitWidth == 0x02) && (TwoChipEN == 0x00) && (CompressEN == 0x00)) 
	{
		rtd_pr_vpq_info("CSOTPDM, SINGLE t-con, mode=%x,PlaneNum=%x,\n", 
			enDEMURAMode, PlaneNum);
		ret = 0;
	} 
	else 
	{
		rtd_pr_vpq_emerg("CSOTPDM,para not support,mode=%x,PlaneNum=%x,bitW=%x,%x,TwoChipEN=%x,CompressEN=%x,HVLutNum=%x,%x,\n",
			enDEMURAMode, PlaneNum, IntBitWidth, DecimalsBitWidth, TwoChipEN, CompressEN, HLutNum, VLutNum/*, HblockSize, VblockSize*/);
		
		ret = -1;
	}
	return ret;
}

static short VPQEX_CSOTPDM_to_internal_Demura_single_Y_Mode_decode_Line_Get(
	unsigned char *pLineData, short *pLineBuff, 
	unsigned short gain_r, unsigned short gain_g, unsigned short gain_b, short offset_r, short offset_g, short offset_b)
{
	static unsigned char tmp_128b[16];
	unsigned char *ptmp;
	unsigned char *ptmp_N_128b;
	unsigned short i, N, remain;
	unsigned short index;
	short val, val_10b;
	short val_tmp_H, val_tmp_L;
	unsigned short numGet;
	
	for (i=0;i<481;i++) {
		ptmp = pLineData;
		numGet = i;

		N = ((unsigned int)numGet * 205)>>11; // N = numGet / 10;

		remain = numGet - N * 10;
		ptmp_N_128b = ptmp + (N * 16);

		memcpy(&tmp_128b[0], ptmp_N_128b, sizeof(char)*16);

		index = (remain>>1)*3;	// remain/2*3

		if ((remain&0x1) == 0) {	// even
			val_tmp_H = tmp_128b[index+1];
			val_tmp_L = tmp_128b[index];
		
			val = ((val_tmp_H&0x0F)<<8) + (val_tmp_L);
		} else {	// odd
			val_tmp_H = tmp_128b[index+2];
			val_tmp_L = tmp_128b[index+1];
			
			val = (val_tmp_H<<4) + ((val_tmp_L&0xF0)>>4);
		}
		// table is 12 bit signed, to 10 bit
		val_10b = CSOTPDM_to_Demura_FMT(val);
		val_10b = (val_10b * gain_r)>>4;
		val_10b = val_10b + CSOTPDM_to_Demura_FMT(offset_r);

		*(pLineBuff+i) = val_10b;
	}
	
	return 0;
}

static int VPQEX_CSOTPDM_to_internal_Demura_single_tcon_convert(unsigned char *pBuff_tmp)
{
	int ret;
	unsigned int i, /*j,*/ k;
	unsigned char *pLineN_addr;
	unsigned char enDEMURAMode, PlaneNum;
	unsigned short tbl_height, tbl_width;
	unsigned short gain_r, gain_g, gain_b;
	short offset_r, offset_g, offset_b;
	unsigned short layer_th_r[7], layer_th_g[7], layer_th_b[7];
	unsigned int addr;
	unsigned char ch_num;

	unsigned short *demura_decode_tbl = NULL;
	
	enDEMURAMode = *(pBuff_tmp+VIP_CSOTPDM_SINGLE_enDEMURAMode);
	PlaneNum = *(pBuff_tmp+VIP_CSOTPDM_SINGLE_u8PlaneNum);
	
	gain_r = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16CompValGainR));
	gain_g = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16CompValGainG));
	gain_b = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16CompValGainB));
	offset_r = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16CompValOffsetR));
	offset_g = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16CompValOffsetG));
	offset_b = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16CompValOffsetB));

	
	if (PlaneNum == 3) 
	{
		layer_th_r[0] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16BlackLimitR));
		layer_th_g[0] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16BlackLimitG));
		layer_th_b[0] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16BlackLimitB));
		
		layer_th_r[1] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane1LevelR));
		layer_th_g[1] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane1LevelG));
		layer_th_b[1] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane1LevelB));
		
		layer_th_r[2] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane2LevelR));
		layer_th_g[2] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane2LevelG));
		layer_th_b[2] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane2LevelB));
		
		layer_th_r[3] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane2LevelR));
		layer_th_g[3] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane2LevelG));
		layer_th_b[3] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane2LevelB));
		
		layer_th_r[4] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane2LevelR));
		layer_th_g[4] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane2LevelG));
		layer_th_b[4] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane2LevelB));
		
		layer_th_r[5] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane3LevelR));
		layer_th_g[5] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane3LevelG));
		layer_th_b[5] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane3LevelB));
		
		layer_th_r[6] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16WhiteLimitR));
		layer_th_g[6] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16WhiteLimitG));
		layer_th_b[6] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16WhiteLimitB));

	} 
	else if (PlaneNum == 4)
	{
		layer_th_r[0] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16BlackLimitR));
		layer_th_g[0] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16BlackLimitG));
		layer_th_b[0] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16BlackLimitB));
		
		layer_th_r[1] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane1LevelR));
		layer_th_g[1] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane1LevelG));
		layer_th_b[1] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane1LevelB));
		
		layer_th_r[2] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane2LevelR));
		layer_th_g[2] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane2LevelG));
		layer_th_b[2] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane2LevelB));
		
		layer_th_r[3] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane3LevelR));
		layer_th_g[3] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane3LevelG));
		layer_th_b[3] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane3LevelB));
		
		layer_th_r[4] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane3LevelR));
		layer_th_g[4] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane3LevelG));
		layer_th_b[4] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane3LevelB));
		
		layer_th_r[5] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane4LevelR));
		layer_th_g[5] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane4LevelG));
		layer_th_b[5] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane4LevelB));
		
		layer_th_r[6] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16WhiteLimitR));
		layer_th_g[6] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16WhiteLimitG));
		layer_th_b[6] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16WhiteLimitB));

	}
	else
	{
		layer_th_r[0] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16BlackLimitR));
		layer_th_g[0] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16BlackLimitG));
		layer_th_b[0] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16BlackLimitB));

		layer_th_r[1] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane1LevelR));
		layer_th_g[1] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane1LevelG));
		layer_th_b[1] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane1LevelB));
		
		layer_th_r[2] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane2LevelR));
		layer_th_g[2] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane2LevelG));
		layer_th_b[2] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane2LevelB));
		
		layer_th_r[3] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane3LevelR));
		layer_th_g[3] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane3LevelG));
		layer_th_b[3] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane3LevelB));
		
		layer_th_r[4] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane4LevelR));
		layer_th_g[4] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane4LevelG));
		layer_th_b[4] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane4LevelB));
		
		layer_th_r[5] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane5LevelR));
		layer_th_g[5] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane5LevelG));
		layer_th_b[5] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16Plane5LevelB));
		
		layer_th_r[6] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16WhiteLimitR));
		layer_th_g[6] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16WhiteLimitG));
		layer_th_b[6] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_SINGLE_u16WhiteLimitB));
	}
	
	tbl_height = 271;
	tbl_width = 481;
	ch_num = (enDEMURAMode == 1)?(3):(1);
	
	demura_decode_tbl = (unsigned short *)dvr_malloc_specific(PlaneNum * tbl_height * tbl_width * ch_num * sizeof(short), GFP_DCU2_FIRST);

	if (demura_decode_tbl == NULL) 
	{
		rtd_pr_vpq_emerg("CSOTPDM, alloc demura_decode_tbl fail\n");
		ret = -1;
	}
	else
	{
		// Y mode
		for (k=0;k<PlaneNum;k++) {
			for (i=0;i<tbl_height;i++) {
				pLineN_addr = pBuff_tmp + VIP_CSOTPDM_SINGLE_LUT_Line_Addr_Get(PlaneNum, k, i);
				addr = (k*tbl_height*tbl_width*ch_num) + (i*tbl_width*ch_num);
				VPQEX_CSOTPDM_to_internal_Demura_single_Y_Mode_decode_Line_Get(pLineN_addr, &demura_decode_tbl[addr],
					gain_r, gain_g, gain_b, offset_r, offset_g, offset_b);
			}
		}


		VPQEX_internal_Demura_TBL_Max_check(&demura_decode_tbl[0], DeMura_TBL_481x271, 0, PlaneNum);
		VPQEX_internal_Demura_TBL_Diff_check(&demura_decode_tbl[0], DeMura_TBL_481x271, 0, PlaneNum);

		VPQEX_internal_Demura_CTRL_setting(&demura_decode_tbl[0], DeMura_TBL_481x271, 0, PlaneNum, &layer_th_r[0], &layer_th_g[0], &layer_th_b[0]);

		//fwif_color_DeMura_encode_8bitmode(&demura_decode_tbl[0], DeMura_TBL_481x271, 0, PlaneNum, &DeMura_TBL);	
		fwif_color_DeMura_encode_mode(&demura_decode_tbl[0], DeMura_TBL_481x271, 0, PlaneNum, &DeMura_TBL);

		rtd_pr_vpq_info("CSOTPDM, single tcon convert ,gain=%d,%d,%d,offset=%d,%d,%d,th_r=%d,%d,%d,%d,%d,%d,%d,th_g=%d,%d,%d,%d,%d,%d,%d,th_b=%d,%d,%d,%d,%d,%d,%d\n", 
			gain_r, gain_g, gain_b, offset_r, offset_g, offset_b, 
			layer_th_r[0],layer_th_r[1],layer_th_r[2],layer_th_r[3],layer_th_r[4],layer_th_r[5],layer_th_r[6],
			layer_th_g[0],layer_th_g[1],layer_th_g[2],layer_th_g[3],layer_th_g[4],layer_th_g[5],layer_th_g[6],
			layer_th_b[0],layer_th_b[1],layer_th_b[2],layer_th_b[3],layer_th_b[4],layer_th_b[5],layer_th_b[6]);
			
		rtd_pr_vpq_info("CSOTPDM, single tcon convert done, panel num = %d, isRGB mode = %d\n", PlaneNum, enDEMURAMode);
		dvr_free(demura_decode_tbl);
		ret = 0;
	}
	
	return ret;
}

static int VPQEX_CSOTPDM_to_internal_Demura_dual_tcon_section_check_Y_Mode_3_Plane(unsigned char *pBuff_tmp)
{
	unsigned char enDEMURAMode, PlaneNum, /*HblockSize, VblockSize,*/ IntBitWidth, DecimalsBitWidth, TwoChipEN, CompressEN;
	unsigned short HLutNum, VLutNum;
	int ret;
	
	enDEMURAMode = *(pBuff_tmp+VIP_CSOTPDM_DUAL_enDEMURAMode);
	PlaneNum = *(pBuff_tmp+VIP_CSOTPDM_DUAL_u8PlaneNum);
	//HblockSize = *(pBuff_tmp+VIP_CSOTPDM_DUAL_u8HblockSize);
	//VblockSize = *(pBuff_tmp+VIP_CSOTPDM_DUAL_u8VblockSize);
	HLutNum = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16HLutNum));
	VLutNum = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16VLutNum));
	IntBitWidth = *(pBuff_tmp+VIP_CSOTPDM_DUAL_u8IntBitWidth);
	DecimalsBitWidth = *(pBuff_tmp+VIP_CSOTPDM_DUAL_u8u8DecimalsBitWidth);
	TwoChipEN = *(pBuff_tmp+VIP_CSOTPDM_DUAL_u8TwoChipEN);
	CompressEN = *(pBuff_tmp+VIP_CSOTPDM_DUAL_u8CompressEN);
#if 0	
	if ((enDEMURAMode == 0x00) && (PlaneNum <= 0x05) && (PlaneNum >= 0x03) && /*(HblockSize == 0x08) && 
		(VblockSize == 0x08) &&*/ (HLutNum == 0x01E1) && (VLutNum == 0x010F) && (IntBitWidth == 0x0A) && 
		(DecimalsBitWidth == 0x02) && (TwoChipEN == 0x01) && (CompressEN == 0x00))
#endif		
	if ((enDEMURAMode == 0x00) && (PlaneNum == 0x03) && (HLutNum == 0x01E1) && (VLutNum == 0x010F) && (IntBitWidth == 0x0A) && 
		(DecimalsBitWidth == 0x02) && (TwoChipEN == 0x01) && (CompressEN == 0x00)) 
	{
		rtd_pr_vpq_info("CSOTPDM, dual t-con, mode=%x,PlaneNum=%x,\n", 
			enDEMURAMode, PlaneNum);
		ret = 0;
	} 
	else 
	{
		rtd_pr_vpq_emerg("CSOTPDM,para not support,mode=%x,PlaneNum=%x,bitW=%x,%x,TwoChipEN=%x,CompressEN=%x,HVLutNum=%x,%x,\n",
			enDEMURAMode, PlaneNum, IntBitWidth, DecimalsBitWidth, TwoChipEN, CompressEN, HLutNum, VLutNum/*, HblockSize, VblockSize*/);
		
		ret = -1;
	}
	return ret;
}

static short VPQEX_CSOTPDM_to_internal_Demura_dual_Y_Mode_decode_Line_Get(
	unsigned char *pLineData_Left, unsigned char *pLineData_Right, short *pLineBuff, 
	unsigned short gain_r, unsigned short gain_g, unsigned short gain_b, short offset_r, short offset_g, short offset_b)
{
	static unsigned char tmp_128b[16];
	unsigned char *ptmp;
	unsigned char *ptmp_N_128b;
	unsigned short i, N, remain;
	unsigned short index;
	short val, val_10b;
	short val_tmp_H, val_tmp_L;
	unsigned short numGet;
	
	for (i=0;i<481;i++) {
		if (i <= 238) {
			ptmp = pLineData_Left;
			numGet = i;
		} else {
			ptmp = pLineData_Right;
			numGet = i - 239;
		}

		N = ((unsigned int)numGet * 205)>>11; // N = numGet / 10;

		remain = numGet - N * 10;
		ptmp_N_128b = ptmp + (N * 16);

		memcpy(&tmp_128b[0], ptmp_N_128b, sizeof(char)*16);

		index = (remain>>1)*3;	// remain/2*3

		if ((remain&0x1) == 0) {	// even
			val_tmp_H = tmp_128b[index+1];
			val_tmp_L = tmp_128b[index];
		
			val = ((val_tmp_H&0x0F)<<8) + (val_tmp_L);
		} else {	// odd
			val_tmp_H = tmp_128b[index+2];
			val_tmp_L = tmp_128b[index+1];
			
			val = (val_tmp_H<<4) + ((val_tmp_L&0xF0)>>4);
		}
		// table is 12 bit signed, to 10 bit
		val_10b = CSOTPDM_to_Demura_FMT(val);
		val_10b = (val_10b * gain_r)>>4;
		val_10b = val_10b + CSOTPDM_to_Demura_FMT(offset_r);

		*(pLineBuff+i) = val_10b;
	}
	
	return 0;
}

static int VPQEX_CSOTPDM_to_internal_Demura_dual_tcon_convert(unsigned char *pBuff_tmp)
{
	int ret;
	unsigned int i, /*j,*/ k;
	unsigned char *pLineN_Left_addr, *pLineN_Right_addr;
	unsigned char enDEMURAMode, PlaneNum;
	unsigned short tbl_height, tbl_width;
	unsigned short gain_r, gain_g, gain_b;
	short offset_r, offset_g, offset_b;
	unsigned short layer_th_r[7], layer_th_g[7], layer_th_b[7];
	unsigned int addr;
	unsigned char ch_num;

	unsigned short *demura_decode_tbl = NULL;
	
	enDEMURAMode = *(pBuff_tmp+VIP_CSOTPDM_DUAL_enDEMURAMode);
	PlaneNum = *(pBuff_tmp+VIP_CSOTPDM_DUAL_u8PlaneNum);
	
	gain_r = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16CompValGainR));
	gain_g = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16CompValGainG));
	gain_b = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16CompValGainB));
	offset_r = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16CompValOffsetR));
	offset_g = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16CompValOffsetG));
	offset_b = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16CompValOffsetB));

	if (PlaneNum == 3) 
	{
		layer_th_r[0] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16BlackLimitR));
		layer_th_g[0] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16BlackLimitG));
		layer_th_b[0] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16BlackLimitB));
		
		layer_th_r[1] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane1LevelR));
		layer_th_g[1] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane1LevelG));
		layer_th_b[1] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane1LevelB));
		
		layer_th_r[2] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane2LevelR));
		layer_th_g[2] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane2LevelG));
		layer_th_b[2] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane2LevelB));
		
		layer_th_r[3] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane2LevelR));
		layer_th_g[3] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane2LevelG));
		layer_th_b[3] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane2LevelB));
		
		layer_th_r[4] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane2LevelR));
		layer_th_g[4] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane2LevelG));
		layer_th_b[4] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane2LevelB));
		
		layer_th_r[5] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane3LevelR));
		layer_th_g[5] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane3LevelG));
		layer_th_b[5] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane3LevelB));
		
		layer_th_r[6] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16WhiteLimitR));
		layer_th_g[6] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16WhiteLimitG));
		layer_th_b[6] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16WhiteLimitB));

	} 
	else if (PlaneNum == 4)
	{
		layer_th_r[0] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16BlackLimitR));
		layer_th_g[0] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16BlackLimitG));
		layer_th_b[0] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16BlackLimitB));
		
		layer_th_r[1] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane1LevelR));
		layer_th_g[1] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane1LevelG));
		layer_th_b[1] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane1LevelB));
		
		layer_th_r[2] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane2LevelR));
		layer_th_g[2] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane2LevelG));
		layer_th_b[2] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane2LevelB));
		
		layer_th_r[3] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane3LevelR));
		layer_th_g[3] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane3LevelG));
		layer_th_b[3] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane3LevelB));
		
		layer_th_r[4] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane3LevelR));
		layer_th_g[4] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane3LevelG));
		layer_th_b[4] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane3LevelB));
		
		layer_th_r[5] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane4LevelR));
		layer_th_g[5] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane4LevelG));
		layer_th_b[5] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane4LevelB));
		
		layer_th_r[6] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16WhiteLimitR));
		layer_th_g[6] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16WhiteLimitG));
		layer_th_b[6] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16WhiteLimitB));

	}
	else
	{
		layer_th_r[0] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16BlackLimitR));
		layer_th_g[0] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16BlackLimitG));
		layer_th_b[0] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16BlackLimitB));

		layer_th_r[1] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane1LevelR));
		layer_th_g[1] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane1LevelG));
		layer_th_b[1] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane1LevelB));
		
		layer_th_r[2] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane2LevelR));
		layer_th_g[2] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane2LevelG));
		layer_th_b[2] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane2LevelB));
		
		layer_th_r[3] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane3LevelR));
		layer_th_g[3] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane3LevelG));
		layer_th_b[3] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane3LevelB));
		
		layer_th_r[4] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane4LevelR));
		layer_th_g[4] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane4LevelG));
		layer_th_b[4] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane4LevelB));
		
		layer_th_r[5] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane5LevelR));
		layer_th_g[5] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane5LevelG));
		layer_th_b[5] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16Plane5LevelB));
		
		layer_th_r[6] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16WhiteLimitR));
		layer_th_g[6] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16WhiteLimitG));
		layer_th_b[6] = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_DUAL_u16WhiteLimitB));
	}
	
	tbl_height = 271;
	tbl_width = 481;
	ch_num = (enDEMURAMode == 1)?(3):(1);
	
	demura_decode_tbl = (unsigned short *)dvr_malloc_specific(PlaneNum * tbl_height * tbl_width * ch_num * sizeof(short), GFP_DCU2_FIRST);

	if (demura_decode_tbl == NULL) 
	{
		rtd_pr_vpq_emerg("CSOTPDM, alloc demura_decode_tbl fail\n");
		ret = -1;
	}
	else
	{
		// Y mode
		for (k=0;k<PlaneNum;k++) {
			for (i=0;i<tbl_height;i++) {
				pLineN_Left_addr = pBuff_tmp + VIP_CSOTPDM_DUAL_LUT_Line_LEFT_Addr_Get(PlaneNum, k, i);
				pLineN_Right_addr = pBuff_tmp + VIP_CSOTPDM_DUAL_LUT_Line_RIGHT_Addr_Get(PlaneNum, k, i);
				addr = (k*tbl_height*tbl_width*ch_num) + (i*tbl_width*ch_num);
				VPQEX_CSOTPDM_to_internal_Demura_dual_Y_Mode_decode_Line_Get(pLineN_Left_addr, pLineN_Right_addr, &demura_decode_tbl[addr],
					gain_r, gain_g, gain_b, offset_r, offset_g, offset_b);
			}
		}


		VPQEX_internal_Demura_TBL_Max_check(&demura_decode_tbl[0], DeMura_TBL_481x271, 0, PlaneNum);
		VPQEX_internal_Demura_TBL_Diff_check(&demura_decode_tbl[0], DeMura_TBL_481x271, 0, PlaneNum);

		VPQEX_internal_Demura_CTRL_setting(&demura_decode_tbl[0], DeMura_TBL_481x271, 0, PlaneNum, &layer_th_r[0], &layer_th_g[0], &layer_th_b[0]);

		//fwif_color_DeMura_encode_8bitmode(&demura_decode_tbl[0], DeMura_TBL_481x271, 0, PlaneNum, &DeMura_TBL);
		fwif_color_DeMura_encode_mode(&demura_decode_tbl[0], DeMura_TBL_481x271, 0, PlaneNum, &DeMura_TBL);
		
		rtd_pr_vpq_info("CSOTPDM, dual tcon convert ,gain=%d,%d,%d,offset=%d,%d,%d,th_r=%d,%d,%d,%d,%d,%d,%d,th_g=%d,%d,%d,%d,%d,%d,%d,th_b=%d,%d,%d,%d,%d,%d,%d\n", 
			gain_r, gain_g, gain_b, offset_r, offset_g, offset_b, 
			layer_th_r[0],layer_th_r[1],layer_th_r[2],layer_th_r[3],layer_th_r[4],layer_th_r[5],layer_th_r[6],
			layer_th_g[0],layer_th_g[1],layer_th_g[2],layer_th_g[3],layer_th_g[4],layer_th_g[5],layer_th_g[6],
			layer_th_b[0],layer_th_b[1],layer_th_b[2],layer_th_b[3],layer_th_b[4],layer_th_b[5],layer_th_b[6]);
		
		rtd_pr_vpq_info("CSOTPDM, dual tcon convert done, panel num = %d, isRGB mode = %d\n", PlaneNum, enDEMURAMode);
		dvr_free(demura_decode_tbl);
		ret = 0;
	}
	
	return ret;
}

static int VPQEX_CSOTPDM_to_internal_Demura_header_check(unsigned char *pBuff_tmp)
{
	int ret;
	unsigned int sec_header_type_0, sec_header_type_1, sec_header_type_2;  

	sec_header_type_0 = *((unsigned int*)(pBuff_tmp+VIP_CSOTPDM_type_0));
	sec_header_type_1 = *((unsigned int*)(pBuff_tmp+VIP_CSOTPDM_type_1));
	sec_header_type_2 = *((unsigned int*)(pBuff_tmp+VIP_CSOTPDM_type_2));
	if ((sec_header_type_0 == 0x01) && (sec_header_type_1 == 0x02)) 
	{
		ret = 0;
	} 
	else if ((sec_header_type_0 == 0x01) && (sec_header_type_1 == 0x03) && (sec_header_type_2 == 0x04)) 
	{
		ret = 1;
	} 
	else 
	{
		rtd_pr_vpq_emerg("CSOTPDM sec type fail, sec type=%x,%x,%x,\n",
			sec_header_type_0, sec_header_type_1, sec_header_type_2);
		ret = -1;
	}

	
	return ret;
}

static int VPQEX_CSOTPDM_to_internal_Demura_SINGLE_Tcon_3_Plane_Ymode_481x271_check
	(unsigned short crc_get, unsigned short crc_cal, unsigned int data_len, int sec_check)
{
	int ret = 0;
	if ((crc_get == crc_cal) && (data_len == VIP_CSOTPDM_SINGLE_TOTAL_SIZE) && (sec_check == 0))
		ret = 0;
	else
		ret = -1;
	return ret;
}

static int VPQEX_CSOTPDM_to_internal_Demura_DUAL_Tcon_3_Plane_Ymode_481x271_check
	(unsigned short crc_get, unsigned short crc_cal, unsigned int data_len, int sec_check)
{
	int ret = 0;
	if ((crc_get == crc_cal) && (data_len == VIP_CSOTPDM_DUAL_TOTAL_SIZE) && (sec_check == 0))
		ret = 0;
	else
		ret = -1;
	return ret;
}
	
static int VPQEX_CSOTPDM_to_internal_Demura_condition_check(HAL_VPQ_DATA_EXTERN_T *HAL_VPQ_DATA_EXTERN, unsigned char *pBuff_tmp)
{
	int ret;
	int header_check, sec_check;
	unsigned short CSOTPDM_CRC_cal, CSOTPDM_CRC;

	header_check = VPQEX_CSOTPDM_to_internal_Demura_header_check(pBuff_tmp);

	if (header_check == 0) // header is single t-con
	{		
		sec_check = VPQEX_CSOTPDM_to_internal_Demura_single_tcon_section_check_Y_Mode_3_Plane(pBuff_tmp);
		CSOTPDM_CRC_cal = VPQEX_CSOTPDM_CalcCRC16(pBuff_tmp+VIP_CSOTPDM_type_0, HAL_VPQ_DATA_EXTERN->length-VIP_CSOTPDM_type_0);
		CSOTPDM_CRC = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_File_total_CRC));
		//if ((CSOTPDM_CRC_cal == CSOTPDM_CRC) && (HAL_VPQ_DATA_EXTERN->length == VIP_CSOTPDM_SINGLE_TOTAL_SIZE) && (sec_check == 0))
		if (VPQEX_CSOTPDM_to_internal_Demura_SINGLE_Tcon_3_Plane_Ymode_481x271_check(CSOTPDM_CRC, CSOTPDM_CRC_cal, HAL_VPQ_DATA_EXTERN->length, sec_check) == 0)
		{
			rtd_pr_vpq_info("CSOTPDM, single section check 3 plane Y mode 481x271\n");
			ret = VPQ_CSOTPDM_SINGLE_T_con_3_Plane_Y_MODE_481x271;
		}
		else
		{
			rtd_pr_vpq_emerg("CSOTPDM, single CRC or len error or sec check fail, crc cal=%d, crc = %d, HAL_VPQ_DATA_EXTERN->length=%d, VIP_CSOTPDM_DUAL_TOTAL_SIZE=%d,\n", 
				CSOTPDM_CRC_cal, CSOTPDM_CRC, HAL_VPQ_DATA_EXTERN->length, VIP_CSOTPDM_SINGLE_TOTAL_SIZE);
			ret = VPQ_CSOTPDM_MODE_Fail_UNKNOW_TYPE;
		}	
	} 
	else if (header_check == 1)	// dual t-con
	{		
		sec_check = VPQEX_CSOTPDM_to_internal_Demura_dual_tcon_section_check_Y_Mode_3_Plane(pBuff_tmp);
		CSOTPDM_CRC_cal = VPQEX_CSOTPDM_CalcCRC16(pBuff_tmp+VIP_CSOTPDM_type_0, HAL_VPQ_DATA_EXTERN->length-VIP_CSOTPDM_type_0);
		CSOTPDM_CRC = *((unsigned short*)(pBuff_tmp+VIP_CSOTPDM_File_total_CRC));
		//if ((CSOTPDM_CRC_cal == CSOTPDM_CRC) && (HAL_VPQ_DATA_EXTERN->length == VIP_CSOTPDM_DUAL_TOTAL_SIZE) && (sec_check == 0)) 		
		if (VPQEX_CSOTPDM_to_internal_Demura_DUAL_Tcon_3_Plane_Ymode_481x271_check(CSOTPDM_CRC, CSOTPDM_CRC_cal, HAL_VPQ_DATA_EXTERN->length, sec_check) == 0)
		{		
			rtd_pr_vpq_info("CSOTPDM, dual section check 3 plane Y mode 481x271\n");
			ret = VPQ_CSOTPDM_DUAL_T_con_3_Plane_Y_MODE_481x271;
		}
		else
		{
			rtd_pr_vpq_emerg("CSOTPDM, dual CRC or len error or sec check fail, crc cal=%d, crc = %d, HAL_VPQ_DATA_EXTERN->length=%d, VIP_CSOTPDM_DUAL_TOTAL_SIZE=%d,\n", 
				CSOTPDM_CRC_cal, CSOTPDM_CRC, HAL_VPQ_DATA_EXTERN->length, VIP_CSOTPDM_DUAL_TOTAL_SIZE);
			ret = VPQ_CSOTPDM_MODE_Fail_UNKNOW_TYPE;
		}
	}
	else
	{		
		rtd_pr_vpq_emerg("CSOTPDM, HEADER check fail\n");
		ret = VPQ_CSOTPDM_MODE_Fail_UNKNOW_TYPE;
	}

	return ret;
}

static int VPQEX_CSOTPDM_to_internal_Demura_mode(unsigned char MODE, unsigned char *pBuff_tmp)
{
	int ret = 0;
	
	if (MODE == VPQ_CSOTPDM_SINGLE_T_con_3_Plane_Y_MODE_481x271) {
		if (VPQEX_CSOTPDM_to_internal_Demura_single_tcon_convert(pBuff_tmp) == 0)
		{					
			fwif_color_DeMura_init();
			rtd_pr_vpq_info("CSOTPDM, single tcon\n");	
			ret = 0;
		}
		else
		{					
			rtd_pr_vpq_info("CSOTPDM, single tcon error\n");	
			ret = -1;
		}
	} else if (MODE == VPQ_CSOTPDM_DUAL_T_con_3_Plane_Y_MODE_481x271) {
		if (VPQEX_CSOTPDM_to_internal_Demura_dual_tcon_convert(pBuff_tmp) == 0) 
		{
			fwif_color_DeMura_init();
			rtd_pr_vpq_info("CSOTPDM, dual tcon\n");	
			ret = 0;
		}
		else
		{
			rtd_pr_vpq_info("CSOTPDM, dual tcon error\n");	
			ret = -1;
		}
	} else {
		rtd_pr_vpq_emerg("CSOTPDM, mode fail\n");
		ret = -1;
	}

	return ret;
}

static int VPQEX_CSOTPDM_to_internal_Demura(HAL_VPQ_DATA_EXTERN_T *HAL_VPQ_DATA_EXTERN)
{
	int ret, MODE;
	unsigned char *pBuff_tmp;
	unsigned long long Data_addr;
	
	Data_addr = HAL_VPQ_DATA_EXTERN->pData;	
	pBuff_tmp = (unsigned char *)dvr_malloc_specific(HAL_VPQ_DATA_EXTERN->length * sizeof(char), GFP_DCU2_FIRST);
	if (pBuff_tmp == NULL) 
	{
		rtd_pr_vpq_emerg("CSOTPDM dynamic alloc memory fail!!!\n");
		ret = -1;
	} 
	else 
	{
		if(copy_from_user(pBuff_tmp, (int __user *)Data_addr, HAL_VPQ_DATA_EXTERN->length))
		{	
			rtd_pr_vpq_emerg("CSOTPDM bin table copy fail\n");
			ret = -1;
		} 
		else 
		{
			MODE = VPQEX_CSOTPDM_to_internal_Demura_condition_check(HAL_VPQ_DATA_EXTERN, pBuff_tmp);
			if (VPQEX_CSOTPDM_to_internal_Demura_mode(MODE, pBuff_tmp) == 0)
				ret = 0;
			else
				ret = -1;
		}
		dvr_free(pBuff_tmp);
	}

	return ret;
}

// HKCPDM
unsigned short VPQEX_HKCPDM_CalcCRC16(unsigned char *buf, unsigned int nByte)
{
	unsigned int i, j;
	unsigned short crc = 0x0000;
	unsigned short crc_temp_1, crc_temp_2, crc_temp_3;
	unsigned char data_buffer;
	//unsigned int length;
	//unsigned char *buf;
	unsigned int checksum;
	unsigned short crcData;
	i = 0;
	checksum = 0;
	while(nByte--){
		data_buffer = buf[i++];
		checksum += data_buffer;
		crc_temp_1 = 0;
		crc_temp_2 = 0;
		crc_temp_3 = 0;
#if 1
		for (j=0;j<8;j++) {
			crc_temp_1 = ((crc>>15)^data_buffer)&0x0001; // result 1 CRC MSB[15]^DATA BIT 0
			crc_temp_2 = ((crc>>1)^crc_temp_1)&0x0001;	// result 2 --> CRC[1]^result 1
			crc_temp_3 = ((crc>>14)^crc_temp_1)&0x0001; // result 3 --> CRC[14]^result 1
			crc = crc<<1;
			crc &= 0x7FFA;		// clear CRC[0]/[2]/[15]
			crc |= crc_temp_1;	// load result 1 to CRC[0]
			crc |= (crc_temp_2<<2);	// load result 2 to CRC[2]
			crc |= (crc_temp_3<<15);	// load result 3 to CRC[15]
			data_buffer >>=1;
		}
#endif		
	}

	crcData = crc;

	return crcData;
}

#define HKCPDM_to_Demura_FMT(arg) (((arg>2048)?(arg-4096):(arg))>>2)

#define VIP_HKCPDM_HEADER_0 0x00
#define VIP_HKCPDM_HEADER_1 0x01
#define VIP_HKCPDM_HEADER_2 0x02
#define VIP_HKCPDM_HEADER_3 0x03
#define VIP_HKCPDM_PARAMETER_CRC_H 0x04
#define VIP_HKCPDM_PARAMETER_CRC_L 0x05

#define VIP_HKCPDM_DEMURA_PLANE_NUM 0x07
#define VIP_HKCPDM_DEMURA_TBL_H_H 0x08
#define VIP_HKCPDM_DEMURA_TBL_H_L 0x09
#define VIP_HKCPDM_DEMURA_TBL_V_H 0x0A
#define VIP_HKCPDM_DEMURA_TBL_V_L 0x0B
#define VIP_HKCPDM_DEMURA_BLK_H 0x0C
#define VIP_HKCPDM_DEMURA_BLK_V 0x0D

#define VIP_HKCPDM_LOWER_BOUND_H 0x18
#define VIP_HKCPDM_LOWER_BOUND_L 0x19
#define VIP_HKCPDM_UPPER_BOUND_H 0x1A
#define VIP_HKCPDM_UPPER_BOUND_L 0x1B

#define VIP_HKCPDM_PLANE00_LV_H 0x28
#define VIP_HKCPDM_PLANE00_LV_L 0x29
#define VIP_HKCPDM_PLANE01_LV_H 0x2A
#define VIP_HKCPDM_PLANE01_LV_L 0x2B
#define VIP_HKCPDM_PLANE02_LV_H 0x2C
#define VIP_HKCPDM_PLANE02_LV_L 0x2D
#define VIP_HKCPDM_PLANE03_LV_H 0x2E
#define VIP_HKCPDM_PLANE03_LV_L 0x2F
#define VIP_HKCPDM_PLANE04_LV_H 0x30
#define VIP_HKCPDM_PLANE04_LV_L 0x31

#define VIP_HKCPDM_TABLE_CRC_H 0x40
#define VIP_HKCPDM_TABLE_CRC_L 0x41

#define VIP_HKCPDM_DEMURA_TABLE_STA 0x42

#define VIP_HKCPDM_DEMURA_SIZE 0x8F3A2	// 586658

static int VPQEX_HKCPDM_to_internal_Demura_TH_convert(unsigned char *pBuff_tmp, unsigned short *layer_th)
{
	unsigned char tmp0, tmp1;
	unsigned char PlaneNum;
	
	PlaneNum = *(pBuff_tmp+VIP_HKCPDM_DEMURA_PLANE_NUM);

	if (pBuff_tmp == NULL)
		return -1;
	
	if (PlaneNum == 3) 
	{
		tmp1 = *(pBuff_tmp+VIP_HKCPDM_LOWER_BOUND_H);
		tmp0 = *(pBuff_tmp+VIP_HKCPDM_LOWER_BOUND_L);
		layer_th[0] = (tmp1<<8) + tmp0;

		tmp1 = *(pBuff_tmp+VIP_HKCPDM_PLANE00_LV_H);
		tmp0 = *(pBuff_tmp+VIP_HKCPDM_PLANE00_LV_L);
		layer_th[1] = (tmp1<<8) + tmp0;

		tmp1 = *(pBuff_tmp+VIP_HKCPDM_PLANE01_LV_H);
		tmp0 = *(pBuff_tmp+VIP_HKCPDM_PLANE01_LV_L);
		layer_th[2] = (tmp1<<8) + tmp0;

		tmp1 = *(pBuff_tmp+VIP_HKCPDM_PLANE01_LV_H);
		tmp0 = *(pBuff_tmp+VIP_HKCPDM_PLANE01_LV_L);
		layer_th[3] = (tmp1<<8) + tmp0;

		tmp1 = *(pBuff_tmp+VIP_HKCPDM_PLANE01_LV_H);
		tmp0 = *(pBuff_tmp+VIP_HKCPDM_PLANE01_LV_L);
		layer_th[4] = (tmp1<<8) + tmp0;

		tmp1 = *(pBuff_tmp+VIP_HKCPDM_PLANE02_LV_H);
		tmp0 = *(pBuff_tmp+VIP_HKCPDM_PLANE02_LV_L);
		layer_th[5] = (tmp1<<8) + tmp0;

		tmp1 = *(pBuff_tmp+VIP_HKCPDM_UPPER_BOUND_H);
		tmp0 = *(pBuff_tmp+VIP_HKCPDM_UPPER_BOUND_L);
		layer_th[6] = (tmp1<<8) + tmp0;
	} 
	else if (PlaneNum == 4)
	{
		tmp1 = *(pBuff_tmp+VIP_HKCPDM_LOWER_BOUND_H);
		tmp0 = *(pBuff_tmp+VIP_HKCPDM_LOWER_BOUND_L);
		layer_th[0] = (tmp1<<8) + tmp0;

		tmp1 = *(pBuff_tmp+VIP_HKCPDM_PLANE00_LV_H);
		tmp0 = *(pBuff_tmp+VIP_HKCPDM_PLANE00_LV_L);
		layer_th[1] = (tmp1<<8) + tmp0;

		tmp1 = *(pBuff_tmp+VIP_HKCPDM_PLANE01_LV_H);
		tmp0 = *(pBuff_tmp+VIP_HKCPDM_PLANE01_LV_L);
		layer_th[2] = (tmp1<<8) + tmp0;

		tmp1 = *(pBuff_tmp+VIP_HKCPDM_PLANE02_LV_H);
		tmp0 = *(pBuff_tmp+VIP_HKCPDM_PLANE02_LV_L);
		layer_th[3] = (tmp1<<8) + tmp0;

		tmp1 = *(pBuff_tmp+VIP_HKCPDM_PLANE02_LV_H);
		tmp0 = *(pBuff_tmp+VIP_HKCPDM_PLANE02_LV_L);
		layer_th[4] = (tmp1<<8) + tmp0;

		tmp1 = *(pBuff_tmp+VIP_HKCPDM_PLANE03_LV_H);
		tmp0 = *(pBuff_tmp+VIP_HKCPDM_PLANE03_LV_L);
		layer_th[5] = (tmp1<<8) + tmp0;

		tmp1 = *(pBuff_tmp+VIP_HKCPDM_UPPER_BOUND_H);
		tmp0 = *(pBuff_tmp+VIP_HKCPDM_UPPER_BOUND_L);
		layer_th[6] = (tmp1<<8) + tmp0;
	}
	else
	{
		tmp1 = *(pBuff_tmp+VIP_HKCPDM_LOWER_BOUND_H);
		tmp0 = *(pBuff_tmp+VIP_HKCPDM_LOWER_BOUND_L);
		layer_th[0] = (tmp1<<8) + tmp0;

		tmp1 = *(pBuff_tmp+VIP_HKCPDM_PLANE00_LV_H);
		tmp0 = *(pBuff_tmp+VIP_HKCPDM_PLANE00_LV_L);
		layer_th[1] = (tmp1<<8) + tmp0;

		tmp1 = *(pBuff_tmp+VIP_HKCPDM_PLANE01_LV_H);
		tmp0 = *(pBuff_tmp+VIP_HKCPDM_PLANE01_LV_L);
		layer_th[2] = (tmp1<<8) + tmp0;

		tmp1 = *(pBuff_tmp+VIP_HKCPDM_PLANE02_LV_H);
		tmp0 = *(pBuff_tmp+VIP_HKCPDM_PLANE02_LV_L);
		layer_th[3] = (tmp1<<8) + tmp0;

		tmp1 = *(pBuff_tmp+VIP_HKCPDM_PLANE03_LV_H);
		tmp0 = *(pBuff_tmp+VIP_HKCPDM_PLANE03_LV_L);
		layer_th[4] = (tmp1<<8) + tmp0;

		tmp1 = *(pBuff_tmp+VIP_HKCPDM_PLANE04_LV_H);
		tmp0 = *(pBuff_tmp+VIP_HKCPDM_PLANE04_LV_L);
		layer_th[5] = (tmp1<<8) + tmp0;

		tmp1 = *(pBuff_tmp+VIP_HKCPDM_UPPER_BOUND_H);
		tmp0 = *(pBuff_tmp+VIP_HKCPDM_UPPER_BOUND_L);
		layer_th[6] = (tmp1<<8) + tmp0;
	}
	return 0;
}

static int VPQEX_HKCPDM_to_internal_Demura_convert(HAL_VPQ_DATA_EXTERN_T *HAL_VPQ_DATA_EXTERN, unsigned char *pBuff_tmp)
{
	int ret;
	unsigned int i, j, k;
	unsigned char *pLineN_addr;
	unsigned short layer_th[7];
	unsigned int addr;
	unsigned char tmp0, tmp1;
	short stmp16;	
	unsigned short *demura_decode_tbl = NULL;
	unsigned short tbl_height, tbl_width;
	unsigned int bin_addr_offset = 0;
	unsigned char sel;
	unsigned char PlaneNum;

	VPQEX_HKCPDM_to_internal_Demura_TH_convert(pBuff_tmp, &layer_th[0]);
	
	PlaneNum = *(pBuff_tmp+VIP_HKCPDM_DEMURA_PLANE_NUM);

	tbl_height = 271;
	tbl_width = 481;
	
	demura_decode_tbl = (unsigned short *)dvr_malloc_specific(PlaneNum * tbl_height * tbl_width * sizeof(short), GFP_DCU2_FIRST);

	if (demura_decode_tbl == NULL) 
	{
		rtd_pr_vpq_emerg("HKCPDM, alloc demura_decode_tbl fail\n");
		ret = -1;
	}
	else
	{
		// Y mode
		sel = 0;
		bin_addr_offset = 0;
		pLineN_addr = pBuff_tmp + VIP_HKCPDM_DEMURA_TABLE_STA;
		for (i=0;i<tbl_height;i++) {
			for (j=0;j<tbl_width;j++) {
				for (k=0;k<PlaneNum;k++) {
					addr = (k*tbl_height*tbl_width) + (i*tbl_width) + j;
					if (sel == 0) {
						tmp1 = *(pLineN_addr + bin_addr_offset);
						tmp0 = *(pLineN_addr + bin_addr_offset + 1);
						stmp16 = (tmp1<<4) + ((tmp0&0xF0)>>4);
						demura_decode_tbl[addr] = HKCPDM_to_Demura_FMT(stmp16);
						sel = 1;
					} else {
						tmp1 = *(pLineN_addr + bin_addr_offset + 1);
						tmp0 = *(pLineN_addr + bin_addr_offset + 2);
						stmp16 = ((tmp1&0x0F)<<8) + tmp0;
						demura_decode_tbl[addr] = HKCPDM_to_Demura_FMT(stmp16);
						sel = 0;
						bin_addr_offset+=3;
					}
				}
			}
		}		

		VPQEX_internal_Demura_TBL_Max_check(&demura_decode_tbl[0], DeMura_TBL_481x271, 0, PlaneNum);
		VPQEX_internal_Demura_TBL_Diff_check(&demura_decode_tbl[0], DeMura_TBL_481x271, 0, PlaneNum);

		VPQEX_internal_Demura_CTRL_setting(&demura_decode_tbl[0], DeMura_TBL_481x271, 0, PlaneNum, &layer_th[0], &layer_th[0], &layer_th[0]);

		//fwif_color_DeMura_encode_8bitmode(&demura_decode_tbl[0], DeMura_TBL_481x271, 0, PlaneNum, &DeMura_TBL);
		fwif_color_DeMura_encode_mode(&demura_decode_tbl[0], DeMura_TBL_481x271, 0, PlaneNum, &DeMura_TBL);
		
		rtd_pr_vpq_info("HKCPDM, convert ,th=%d,%d,%d,%d,%d,%d,%d,\n", 
			layer_th[0],layer_th[1],layer_th[2],layer_th[3],layer_th[4],layer_th[5],layer_th[6]);
			
		rtd_pr_vpq_info("HKCPDM, convert done, panel num = %d, bin_addr_offset=%x,\n", PlaneNum, bin_addr_offset);
		dvr_free(demura_decode_tbl);
		ret = 0;
	}
	
	return ret;
}

static int VPQEX_HKCPDM_to_internal_Demura_condition_check(HAL_VPQ_DATA_EXTERN_T *HAL_VPQ_DATA_EXTERN, unsigned char *pBuff_tmp)
{
	int ret;
	unsigned char tmp0, tmp1;
	unsigned char header_0, header_1, header_2, header_3, plane_num;  
	unsigned int header32;
	unsigned short width, length;
	unsigned short crc_cal, crc;
	header_0 = *((pBuff_tmp+VIP_HKCPDM_HEADER_0));
	header_1 = *((pBuff_tmp+VIP_HKCPDM_HEADER_1));
	header_2 = *((pBuff_tmp+VIP_HKCPDM_HEADER_2));
	header_3 = *((pBuff_tmp+VIP_HKCPDM_HEADER_3));
	header32 = (header_0 << 24) + (header_1 << 16) + (header_2 << 8) + (header_3);

	plane_num = *((pBuff_tmp+VIP_HKCPDM_DEMURA_PLANE_NUM));
		
	tmp1 = *((pBuff_tmp+VIP_HKCPDM_DEMURA_TBL_H_H));
	tmp0 = *((pBuff_tmp+VIP_HKCPDM_DEMURA_TBL_H_L));
	width = (tmp1 << 8) + tmp0;

	tmp1 = *((pBuff_tmp+VIP_HKCPDM_DEMURA_TBL_V_H));
	tmp0 = *((pBuff_tmp+VIP_HKCPDM_DEMURA_TBL_V_L));
	length = (tmp1 << 8) + tmp0;

	tmp1 = *((pBuff_tmp+VIP_HKCPDM_TABLE_CRC_H));
	tmp0 = *((pBuff_tmp+VIP_HKCPDM_TABLE_CRC_L));
	crc = (tmp1 << 8) + tmp0;
	crc_cal = VPQEX_HKCPDM_CalcCRC16(pBuff_tmp+VIP_HKCPDM_DEMURA_TABLE_STA, HAL_VPQ_DATA_EXTERN->length-VIP_HKCPDM_DEMURA_TABLE_STA);
	
	if ((header32 == 0xAA555AA5) && (plane_num == 0x03) && (HAL_VPQ_DATA_EXTERN->length == VIP_HKCPDM_DEMURA_SIZE) &&
		(crc == crc_cal) && (width == 0x1E1) && (length == 0x10F)) 
	{
		rtd_pr_vpq_info("HKCPDM header ok,\n");
		ret = 0;
	} 
	else 
	{
		rtd_pr_vpq_emerg("HKCPDM header fail, header32=%x,header8=%x,%x,%x,%x,crc=%x, crc_cal=%x, width=%x, length=%x,plane_num=%x,table_size=%x,\n",
			header32, header_0, header_1, header_2, header_3, crc, crc_cal, width, length, plane_num, HAL_VPQ_DATA_EXTERN->length);
		ret = -1;
	}
	
	return ret;
}


static int VPQEX_HKCPDM_to_internal_Demura(HAL_VPQ_DATA_EXTERN_T *HAL_VPQ_DATA_EXTERN)
{
	int ret;
	unsigned char *pBuff_tmp;
	unsigned long long Data_addr;
	
	Data_addr = HAL_VPQ_DATA_EXTERN->pData;	
	pBuff_tmp = (unsigned char *)dvr_malloc_specific(HAL_VPQ_DATA_EXTERN->length * sizeof(char), GFP_DCU2_FIRST);
	if (pBuff_tmp == NULL) 
	{
		rtd_pr_vpq_emerg("HKCPDM dynamic alloc memory fail!!!\n");
		ret = -1;
	} 
	else 
	{
		if(copy_from_user(pBuff_tmp, (int __user *)Data_addr, HAL_VPQ_DATA_EXTERN->length))
		{	
			rtd_pr_vpq_emerg("HKCPDM bin table copy fail\n");
			ret = -1;
		} 
		else 
		{
			if (VPQEX_HKCPDM_to_internal_Demura_condition_check(HAL_VPQ_DATA_EXTERN, pBuff_tmp) == 0) {
				if (VPQEX_HKCPDM_to_internal_Demura_convert(HAL_VPQ_DATA_EXTERN, pBuff_tmp) == 0)
				{	
					fwif_color_DeMura_init();
					rtd_pr_vpq_info("HKCPDM, \n");	
					ret = 0;
				}
				else
				{					
					rtd_pr_vpq_info("HKCPDM, error\n");	
					ret = -1;
				}
			} else {
				rtd_pr_vpq_emerg("HKCPDM, header_check fail\n");
				ret = -1;
			}		
		}
		dvr_free(pBuff_tmp);
	}

	return ret;
}

#define CHOTPDM_to_Demura_FMT(arg) ((arg-2048)>>2)
#define VIP_CHOTPDM_STA 0x00000
#define VIP_CHOTPDM_HEADER_STA VIP_CHOTPDM_STA
#define VIP_CHOTPDM_Demura_Table_CRC_H 0x00099
#define VIP_CHOTPDM_Demura_Table_CRC_L 0x0009A
#define VIP_CHOTPDM_Low_Bound_H 0x00060		// 
#define VIP_CHOTPDM_Low_Bound_L 0x00061
#define VIP_CHOTPDM_Plane0_LV_H 0x0003E
#define VIP_CHOTPDM_Plane0_LV_L 0x00035
#define VIP_CHOTPDM_Plane1_LV_H 0x0003E
#define VIP_CHOTPDM_Plane1_LV_L 0x00036
#define VIP_CHOTPDM_Plane2_LV_H 0x0003F
#define VIP_CHOTPDM_Plane2_LV_L 0x00037
#define VIP_CHOTPDM_Plane3_LV_H 0x0003F
#define VIP_CHOTPDM_Plane3_LV_L 0x00038
#define VIP_CHOTPDM_Plane4_LV_H 0x00040
#define VIP_CHOTPDM_Plane4_LV_L 0x00039
#define VIP_CHOTPDM_Plane5_LV_H 0x00040
#define VIP_CHOTPDM_Plane5_LV_L 0x0003A
#define VIP_CHOTPDM_Plane6_LV_H 0x00041
#define VIP_CHOTPDM_Plane6_LV_L 0x0003B
#define VIP_CHOTPDM_Plane7_LV_H 0x00041
#define VIP_CHOTPDM_Plane7_LV_L 0x0003C
#define VIP_CHOTPDM_Plane8_LV_H 0x00042
#define VIP_CHOTPDM_Plane8_LV_L 0x0003D
#define VIP_CHOTPDM_Plane9_LV_H 0x000B3
#define VIP_CHOTPDM_Plane9_LV_L 0x000B0
#define VIP_CHOTPDM_Plane10_LV_H 0x000B3
#define VIP_CHOTPDM_Plane10_LV_L 0x000B1
#define VIP_CHOTPDM_Plane11_LV_H 0x000B4
#define VIP_CHOTPDM_Plane11_LV_L 0x000B2
#define VIP_CHOTPDM_High_Bound_H 0x00066		// 
#define VIP_CHOTPDM_High_Bound_L 0x00067
#define VIP_CHOTPDM_0X67 0x001A0
#define VIP_CHOTPDM_0X72 0x001A1
#define VIP_CHOTPDM_0X79 0x001A2
#define VIP_CHOTPDM_0X84 0x001A3
#define VIP_CHOTPDM_Demura_Setting_CRC_H 0x0019B
#define VIP_CHOTPDM_Demura_Setting_CRC_L 0x0019C
#define VIP_CHOTPDM_HEADER_END 0x001FF
#define VIP_CHOTPDM_LUT_STA 0x00200
#define VIP_CHOTPDM_LUT_END_Y_3Layer 0x9039D
#define VIP_CHOTPDM_HEADER_Size (VIP_CHOTPDM_HEADER_END + 1)
#define VIP_CHOTPDM_LUT_Size_Y_3Layer (VIP_CHOTPDM_LUT_END_Y_3Layer - VIP_CHOTPDM_HEADER_END)
#define VIP_CHOTPDM_Total_Size_Y_3Layer (VIP_CHOTPDM_LUT_END_Y_3Layer + 1)

static int VPQEX_CHOTDM_to_internal_Demura_TH_Get(unsigned char *pBuff_tmp, unsigned short *layer_th, unsigned char *layer_num)
{
	unsigned int i;
	unsigned short plane_num = 0;
	unsigned short plane_th[12];
	unsigned short lowBND;
	unsigned short highBND;
	
	if (pBuff_tmp == NULL || layer_num == NULL)
	{
		rtd_pr_vpq_info("VPQEX_CHOTDM_to_internal_Demura_TH_Get, null pointer\n");
		return -1;
	}

	lowBND = ((*(pBuff_tmp+VIP_CHOTPDM_Low_Bound_H))<<4) + ((*(pBuff_tmp+VIP_CHOTPDM_Low_Bound_L))&0x0F);
	plane_th[0] = (((*(pBuff_tmp+VIP_CHOTPDM_Plane0_LV_H))&0x0F)<<8) + (*(pBuff_tmp+VIP_CHOTPDM_Plane0_LV_L));
	plane_th[1] = (((*(pBuff_tmp+VIP_CHOTPDM_Plane1_LV_H))&0xF0)<<4) + (*(pBuff_tmp+VIP_CHOTPDM_Plane1_LV_L));
	plane_th[2] = (((*(pBuff_tmp+VIP_CHOTPDM_Plane2_LV_H))&0x0F)<<8) + (*(pBuff_tmp+VIP_CHOTPDM_Plane2_LV_L));
	plane_th[3] = (((*(pBuff_tmp+VIP_CHOTPDM_Plane3_LV_H))&0xF0)<<4) + (*(pBuff_tmp+VIP_CHOTPDM_Plane3_LV_L));
	plane_th[4] = (((*(pBuff_tmp+VIP_CHOTPDM_Plane4_LV_H))&0x0F)<<8) + (*(pBuff_tmp+VIP_CHOTPDM_Plane4_LV_L));
	plane_th[5] = (((*(pBuff_tmp+VIP_CHOTPDM_Plane5_LV_H))&0xF0)<<4) + (*(pBuff_tmp+VIP_CHOTPDM_Plane5_LV_L));
	plane_th[6] = (((*(pBuff_tmp+VIP_CHOTPDM_Plane6_LV_H))&0x0F)<<8) + (*(pBuff_tmp+VIP_CHOTPDM_Plane6_LV_L));
	plane_th[7] = (((*(pBuff_tmp+VIP_CHOTPDM_Plane7_LV_H))&0xF0)<<4) + (*(pBuff_tmp+VIP_CHOTPDM_Plane7_LV_L));
	plane_th[8] = (((*(pBuff_tmp+VIP_CHOTPDM_Plane8_LV_H))&0x0F)<<8) + (*(pBuff_tmp+VIP_CHOTPDM_Plane8_LV_L));
	plane_th[9] = (((*(pBuff_tmp+VIP_CHOTPDM_Plane9_LV_H))&0xF0)<<4) + (*(pBuff_tmp+VIP_CHOTPDM_Plane9_LV_L));
	plane_th[10] = (((*(pBuff_tmp+VIP_CHOTPDM_Plane10_LV_H))&0x0F)<<8) + (*(pBuff_tmp+VIP_CHOTPDM_Plane10_LV_L));
	plane_th[11] = (((*(pBuff_tmp+VIP_CHOTPDM_Plane11_LV_H))&0xF0)<<4) + (*(pBuff_tmp+VIP_CHOTPDM_Plane11_LV_L));
	highBND = ((*(pBuff_tmp+VIP_CHOTPDM_High_Bound_H))<<4) + ((*(pBuff_tmp+VIP_CHOTPDM_High_Bound_L))&0x0F);

	plane_num = 1;
	for (i=0;i<4;i++)	// demura only max 5
	{
		if (plane_th[i] != plane_th[i+1])
			plane_num++;
	}	

	*layer_num = plane_num;

	if (plane_num == 3) 
	{
		layer_th[0] = lowBND>>2;

		layer_th[1] = plane_th[0]>>2;

		layer_th[2] = plane_th[1]>>2;

		layer_th[3] = plane_th[1]>>2;

		layer_th[4] = plane_th[1]>>2;

		layer_th[5] = plane_th[2]>>2;

		layer_th[6] = highBND>>2;
	} 
	else if (plane_num == 4)
	{
		layer_th[0] = lowBND>>2;

		layer_th[1] = plane_th[0]>>2;

		layer_th[2] = plane_th[1]>>2;

		layer_th[3] = plane_th[2]>>2;

		layer_th[4] = plane_th[2]>>2;

		layer_th[5] = plane_th[3]>>2;

		layer_th[6] = highBND>>2;
	}
	else
	{
		layer_th[0] = lowBND>>2;

		layer_th[1] = plane_th[0]>>2;

		layer_th[2] = plane_th[1]>>2;

		layer_th[3] = plane_th[2]>>2;

		layer_th[4] = plane_th[3]>>2;

		layer_th[5] = plane_th[4]>>2;

		layer_th[6] = highBND>>2;
	}
	rtd_pr_vpq_info("VPQEX_CHOTDM_to_internal_Demura_TH_Get, th=%d,%d,%d,%d,%d,plane_num=%d \n", 
		plane_th[0], plane_th[1], plane_th[2], plane_th[3], plane_th[4], plane_num);
	return 0;
}

static int VPQEX_CHOTPDM_to_internal_Demura_convert(HAL_VPQ_DATA_EXTERN_T *HAL_VPQ_DATA_EXTERN, unsigned char *pBuff_tmp)
{	
	int ret = 0;
	unsigned char plane_num;
	unsigned short layer_th[7];
	unsigned short *demura_decode_tbl = NULL;
	unsigned short tbl_height, tbl_width;
	unsigned short h_line_byte = 726;
	unsigned short i, j, k;
	unsigned int bin_addr_offset = 0;
	unsigned char sel;
	unsigned char *pLineN_addr;
	unsigned int addr;
	unsigned char tmp_h, tmp_l;
	short stmp16;	
	
	tbl_height = 271;
	tbl_width = 481;
	
	VPQEX_CHOTDM_to_internal_Demura_TH_Get(pBuff_tmp, &layer_th[0], &plane_num);

	demura_decode_tbl = (unsigned short *)dvr_malloc_specific(plane_num * tbl_height * tbl_width * sizeof(short), GFP_DCU2_FIRST);

	if (demura_decode_tbl == NULL) 
	{
		rtd_pr_vpq_emerg("CHOTPDM, alloc demura_decode_tbl fail\n");
		ret = -1;
	}
	else
	{
		// Y mode
		for (i=0;i<tbl_height;i++) {		
			for (k=0;k<plane_num;k++) {				
				pLineN_addr = (pBuff_tmp + VIP_CHOTPDM_LUT_STA) + ((i * plane_num + k) * h_line_byte);
				sel = 0;
				bin_addr_offset = 0;
				for (j=0;j<tbl_width;j++) {
					addr = (k*tbl_height*tbl_width) + (i*tbl_width) + j;
					if (sel == 0) {
						tmp_l = *(pLineN_addr + bin_addr_offset);
						tmp_h = *(pLineN_addr + bin_addr_offset + 1);
						stmp16 = ((tmp_h&0x0F)<<8) + tmp_l;
						demura_decode_tbl[addr] = CHOTPDM_to_Demura_FMT(stmp16);
						sel = 1;
					} else {
						tmp_l = *(pLineN_addr + bin_addr_offset + 1);
						tmp_h = *(pLineN_addr + bin_addr_offset + 2);
						stmp16 = (tmp_h<<4) + ((tmp_l&0xF0)>>4);
						demura_decode_tbl[addr] = CHOTPDM_to_Demura_FMT(stmp16);
						sel = 0;
						bin_addr_offset+=3;
					}
				}
			}
		}

		VPQEX_internal_Demura_TBL_Max_check(&demura_decode_tbl[0], DeMura_TBL_481x271, 0, plane_num);
		VPQEX_internal_Demura_TBL_Diff_check(&demura_decode_tbl[0], DeMura_TBL_481x271, 0, plane_num);

		VPQEX_internal_Demura_CTRL_setting(&demura_decode_tbl[0], DeMura_TBL_481x271, 0, plane_num, &layer_th[0], &layer_th[0], &layer_th[0]);

		//fwif_color_DeMura_encode_8bitmode(&demura_decode_tbl[0], DeMura_TBL_481x271, 0, PlaneNum, &DeMura_TBL);
		fwif_color_DeMura_encode_mode(&demura_decode_tbl[0], DeMura_TBL_481x271, 0, plane_num, &DeMura_TBL);
		
		rtd_pr_vpq_info("CHOTPDM, convert ,plane_num=%d,th=%d,%d,%d,%d,%d,%d,%d,\n", 
			plane_num, layer_th[0],layer_th[1],layer_th[2],layer_th[3],layer_th[4],layer_th[5],layer_th[6]);
				
		ret = 0;
		dvr_free(demura_decode_tbl);
	}

	return ret;
}

static unsigned short VPQEX_CHOTPDM_to_internal_Demura_Check_Sum_Cal(unsigned char *pBuff_LUT_tmp, unsigned int sizeByte)
{
	unsigned int i;
	unsigned short check_sum;

	check_sum = 0;
	for (i=0;i<(sizeByte>>1);i++)
		check_sum = (check_sum + ((*(pBuff_LUT_tmp+2*i))<<8) + (*(pBuff_LUT_tmp+2*i+1))) & 0xFFFF;

	return check_sum;
}

static int VPQEX_CHOTPDM_to_internal_Demura_condition_check(HAL_VPQ_DATA_EXTERN_T *HAL_VPQ_DATA_EXTERN, unsigned char *pBuff_tmp)
{
	int ret;
	unsigned char tmp0, tmp1;
	unsigned char header_0, header_1, header_2, header_3, plane_num;
	unsigned short layer_th[7];
	unsigned int header32;
	unsigned short check_sum_cal, check_sum;
	
	header_0 = *(pBuff_tmp+VIP_CHOTPDM_0X67);
	header_1 = *(pBuff_tmp+VIP_CHOTPDM_0X72);
	header_2 = *(pBuff_tmp+VIP_CHOTPDM_0X79);
	header_3 = *(pBuff_tmp+VIP_CHOTPDM_0X84);
	header32 = (header_0 << 24) + (header_1 << 16) + (header_2 << 8) + (header_3);

	VPQEX_CHOTDM_to_internal_Demura_TH_Get(pBuff_tmp, &layer_th[0], &plane_num);
		
	tmp1 = *((pBuff_tmp+VIP_CHOTPDM_Demura_Table_CRC_H));
	tmp0 = *((pBuff_tmp+VIP_CHOTPDM_Demura_Table_CRC_L));
	check_sum = (tmp1 << 8) + tmp0;
	check_sum_cal = VPQEX_CHOTPDM_to_internal_Demura_Check_Sum_Cal(pBuff_tmp+VIP_CHOTPDM_LUT_STA, VIP_CHOTPDM_LUT_Size_Y_3Layer);// 590238 byte is for y mode 3 layer
	
	if ((header32 == 0x67727984) && (plane_num == 0x03) && (HAL_VPQ_DATA_EXTERN->length == VIP_CHOTPDM_Total_Size_Y_3Layer) &&
		(check_sum == check_sum_cal)) 
	{
		rtd_pr_vpq_info("CHOTPDM header ok,\n");
		ret = 0;
	} 
	else 
	{
		rtd_pr_vpq_emerg("CHOTPDM header fail, layer_th=%x,%x,%x,%x,%x,%x,%x,\n",
			layer_th[0], layer_th[1], layer_th[2], layer_th[3], layer_th[4], layer_th[5], layer_th[6]);
		rtd_pr_vpq_emerg("CHOTPDM header fail, header32=%x,header8=%x,%x,%x,%x,check_sum=%x, check_sum=%x,plane_num=%x,table_size=%x,\n",
			header32, header_0, header_1, header_2, header_3, check_sum, check_sum_cal, plane_num, HAL_VPQ_DATA_EXTERN->length);

		ret = -1;
	}
	
	return ret;
}

static int VPQEX_CHOTPDM_to_internal_Demura(HAL_VPQ_DATA_EXTERN_T *HAL_VPQ_DATA_EXTERN)
{
	int ret;
	unsigned char *pBuff_tmp;
	unsigned long long Data_addr;
	
	Data_addr = HAL_VPQ_DATA_EXTERN->pData;	
	pBuff_tmp = (unsigned char *)dvr_malloc_specific(HAL_VPQ_DATA_EXTERN->length * sizeof(char), GFP_DCU2_FIRST);
	if (pBuff_tmp == NULL) 
	{
		rtd_pr_vpq_emerg("HKCPDM dynamic alloc memory fail!!!\n");
		ret = -1;
	} 
	else 
	{
		if(copy_from_user(pBuff_tmp, (int __user *)Data_addr, HAL_VPQ_DATA_EXTERN->length))
		{	
			rtd_pr_vpq_emerg("HKCPDM bin table copy fail\n");
			ret = -1;
		} 
		else 
		{
			if (VPQEX_CHOTPDM_to_internal_Demura_condition_check(HAL_VPQ_DATA_EXTERN, pBuff_tmp) == 0) {
				if (VPQEX_CHOTPDM_to_internal_Demura_convert(HAL_VPQ_DATA_EXTERN, pBuff_tmp) == 0)
				{	
					fwif_color_DeMura_init();
					rtd_pr_vpq_info("CHOTPDM, \n");	
					ret = 0;
				}
				else
				{					
					rtd_pr_vpq_info("CHOTPDM, error\n");	
					ret = -1;
				}
			} else {
				rtd_pr_vpq_emerg("CHOTPDM, header_check fail\n");
				ret = -1;
			}		
		}
		dvr_free(pBuff_tmp);
	}

	return ret;
}

extern char vpq_INNX_Demura_toDemura_Conv(unsigned char *pINNXDemura, unsigned int TBL_Size);
static int VPQEX_INNXPDM_to_internal_Demura(HAL_VPQ_DATA_EXTERN_T *HAL_VPQ_DATA_EXTERN)
{
	// for innx, use ori code flow
	int ret;
	unsigned char *pBuff_tmp;
	unsigned long long Data_addr;
	
	Data_addr = HAL_VPQ_DATA_EXTERN->pData;	
	pBuff_tmp = (unsigned char *)dvr_malloc_specific(HAL_VPQ_DATA_EXTERN->length * sizeof(char), GFP_DCU2_FIRST);
	if (pBuff_tmp == NULL) 
	{
		rtd_pr_vpq_emerg("INNXPDM dynamic alloc memory fail!!!\n");
		ret = -1;
	} 
	else 
	{
		if(copy_from_user(pBuff_tmp, (int __user *)Data_addr, HAL_VPQ_DATA_EXTERN->length))
		{	
			rtd_pr_vpq_emerg("INNXPDM bin table copy fail\n");
			ret = -1;
		} 
		else 
		{
			if (vpq_INNX_Demura_toDemura_Conv(pBuff_tmp, HAL_VPQ_DATA_EXTERN->length) == 0) {
				rtd_pr_vpq_info("INNXPDM, done\n");
				ret = 0;
			} else {
				rtd_pr_vpq_emerg("INNXPDM, header_check fail\n");
				ret = -1;
			}		
		}
		dvr_free(pBuff_tmp);
	}

	return ret;


}
#endif

int vpqex_open(struct inode *inode, struct file *filp)
{
	rtd_pr_vpq_info("vpqex_open %d\n", __LINE__);
	return 0;
}

ssize_t  vpqex_read(struct file *filep, char *buffer, size_t count, loff_t *offp)
{
	return 0;
}

ssize_t vpqex_write(struct file *filep, const char *buffer, size_t count, loff_t *offp)
{
	return 0;
}
int vpqex_release(struct inode *inode, struct file *filep)
{
	rtd_pr_vpq_info("vpqex_release %d\n", __LINE__);
	return 0;
}

#ifdef CONFIG_PM
static int vpqex_suspend(struct device *p_dev)
{
	return 0;
}

static int vpqex_resume(struct device *p_dev)
{
	return 0;
}
#endif

void vpqex_boot_init(void)
{

}

unsigned char vpq_extern_ioctl_get_stop_run(unsigned int cmd)
{
	return (ioctl_extern_cmd_stop[_IOC_NR(cmd)&0xff]|ioctl_extern_cmd_stop[0]);
}

unsigned char vpq_extern_ioctl_get_stop_run_by_idx(unsigned char cmd_idx)
{
	return ioctl_extern_cmd_stop[cmd_idx];
}

void vpq_extern_ioctl_set_stop_run_by_idx(unsigned char cmd_idx, unsigned char stop)
{
	ioctl_extern_cmd_stop[cmd_idx] = stop;
}

bool vpqex_skip_middle_ware_picmode_data(unsigned int cmd)
{
	switch(cmd) {
		case VPQ_EXTERN_IOC_SET_PIC_MODE_DATA:
		case VPQ_EXTERN_IOC_SET_PIC_MODE_DATA_defaultPictureModeSet:
		case VPQ_EXTERN_IOC_SET_PIC_MODE_DATA_DolbyHDRPictureModeSet:
		case VPQ_EXTERN_IOC_SET_StructColorDataType:
		case VPQ_EXTERN_IOC_SET_StructColorDataType_defaultSDColorData:
		case VPQ_EXTERN_IOC_SET_StructColorDataType_defaultHDColorData:
		case VPQ_EXTERN_IOC_SET_COLOR_TEMP_DATA:
		case VPQ_EXTERN_IOC_SET_COLOR_TEMP_DATA_defColorTempSet:
		case VPQ_EXTERN_IOC_SET_StructColorDataFacModeType:
		case VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_defaultColorFacMode:
		case VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_AvColorFacMode:
		case VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_YppColorFacMode:
		case VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_VgaColorFacMode:
		case VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_HdmiColorFacMode:
		case VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_HdmiSDColorFacMode:
			return true;
	}
	return false;
}
#endif
UINT32 HistoData_bin[TV006_VPQ_chrm_bin];
UINT32 HistoData_Hue_bin[COLOR_HUE_HISTOGRAM_LEVEL];
UINT32 HistoData_Sat_bin[COLOR_AutoSat_HISTOGRAM_LEVEL];
VIP_MAGIC_GAMMA_Curve_Driver_Data data;
extern VIP_MAGIC_GAMMA_Curve_Driver_Data g_MagicGammaDriverDataSave;
RTK_TableSize_Gamma Gamma;
extern VIP_DeMura_TBL m_pDeMura_TBL;
extern unsigned char m_pVALC_Table[6][17][17];
extern unsigned short m_pLEDOutGamma[257];
extern DRV_pcid_valc_t m_pVALCprotectSetting[2];
extern DRV_od_table_mode_t m_pOD_setting_table[VIP_QUALITY_SOURCE_NUM];
extern VIP_Output_InvOutput_Gamma m_pOutput_InvOutput_Gamma[Output_InvOutput_Gamma_TBL_MAX];
extern unsigned char m_pod_table_test[OD_table_length];
extern DRV_od_table_t m_pOD_table;
#ifndef BUILD_QUICK_SHOW
VIP_INI_Gamma_Curve AP_INI_Gamma_Curve = {0};

void sm_vpq_extern_init(void)
{
    //vpqex_project_id = 0x00010000;
	rtd_pr_vpq_debug("VPQ_EXTERN_IOC_INIT\n");	
	Scaler_color_Set_HDR_AutoRun(TRUE);
    return;
}
EXPORT_SYMBOL(sm_vpq_extern_init);

long vpqex_ioctl_tv030(struct file *file, unsigned int cmd,  unsigned long arg)
{

	int ret = 0;

	switch (cmd) {
	case VPQ_EXTERN_IOC_SET_OSDPIC_FLAG_TV030://flag
	{
		RTK_VIP_Flag pFlag;
		SCALER_DISPLAY_DATA sdp_data;
		if (copy_from_user(&pFlag,  (int __user *)arg,sizeof(RTK_VIP_Flag))) {
			rtd_pr_vpq_info("[kernel]set VPQ_EXTERN_IOC_SET_OSDPIC_FLAG_TV030 fail\n");
			ret = -1;
		} else {

			sdp_data.h_sta = pFlag.flag_2;
			sdp_data.h_end = pFlag.flag_3;
			sdp_data.v_sta = pFlag.flag_4;
			sdp_data.v_end = pFlag.flag_5;
			Scaler_Set_Partten4AutoGamma_mute(pFlag.flag_0,pFlag.flag_1,&sdp_data);

			ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_MAGICGAMMA_TV030:
	{
		//unsigned char i=0;
		RTK_TV030_MagicGamma MagicGamma;
		//rtd_pr_vpq_info("[OSD]MAGICGAMMA\n");

		if (copy_from_user(&MagicGamma,  (int __user *)arg,sizeof(RTK_TV030_MagicGamma))) {
			ret = -1;
		} else {
			//for(i=0;i<12;i++){
			//rtd_pr_vpq_info("G[%d]=%d\n",i,MagicGamma.MagicGammaG[i]);
			//}
			//rtd_pr_vpq_info("[12points]ColorSpaceMode=%d\n",ColorSpaceMode);
			fwif_color_set_20pointsData(&MagicGamma);
			fwif_color_set_gamma_Magic();

			ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_GAMMA_LEVEL_TV030:
	{
		unsigned char level;
		rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_GAMMA_LEVE\n");

		if (copy_from_user(&level,	(void __user *)arg,sizeof(unsigned char))) {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_GAMMA_LEVEL fail\n");
			ret = -1;
		} else {
			Scaler_Set_Gamma_level(level);
			ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_PQ_SETGAMMA_TABLESIZE_TV030:
	{
		static RTK_TableSize_Gamma Gamma;
		if (copy_from_user(&Gamma,	(int __user *)arg,sizeof(RTK_TableSize_Gamma))) {
			ret = -1;
		} else {
			//rtd_pr_vpq_info("[OSD]SETGAMMA\n");
			fwif_color_gamma_encode_TableSize(&Gamma);
			fwif_color_set_gamma_Magic();
			ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_GET_LC_BLK_HVNUM_TV030:
	{
		//rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_LC_BLK_HVNUM_TV030\n");
		LC_BLK_Num_Size LC_pData;

		LC_pData.nBlk_Hnum=Scaler_Get_LC_blk_hnum();
		LC_pData.nBlk_Vnum=Scaler_Get_LC_blk_vnum();

		if (copy_to_user((void __user *)arg, (void *)&LC_pData, sizeof(LC_BLK_Num_Size))) {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_LC_BLK_HVNUM_TV030 fail\n");
			ret = -1;
		} else
			ret = 0;

	}
	break;

	case VPQ_EXTERN_IOC_GET_HDR_PicInfo_TV030:
	{
		static UINT32 HDR_info_bin[131] = {0};
		//rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_HDR_PicInfo_TV030\n");
		if (fwif_color_get_DM_HDR10_enable_TV030() == 0) {
			//rtd_pr_vpq_info("1.HDR_PicInfo_TV030\n");
			ret = -1;
			break;
		}

		if (fwif_color_get_DM2_HDR_histogram_TV030(HDR_info_bin) == 0) {
			//rtd_pr_vpq_info("2.HDR_PicInfo_TV030\n");
			ret = -1;
		} else {
			if (copy_to_user((void __user *)arg, HDR_info_bin, 131 * 4)) {
			//rtd_pr_vpq_info("3.HDR_PicInfo_TV030\n");
				ret = -1;
			} else
				ret = 0;
		}
		break;
	}

	case VPQ_EXTERN_IOC_GET_HDR_HIS_MAXRGB:
	{
		static UINT32 HDR_info_bin[131] = {0};
		//rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_HDR_HIS_MAXRGB\n");
		if (fwif_color_get_DM_HDR10_enable_TV030() == 0) {
			//rtd_pr_vpq_info("1.HDR_PicInfo_TV030\n");
			ret = -1;
			break;
		}

		if (fwif_color_get_DM2_HDR_histogram_MaxRGB(HDR_info_bin) == 0) {
			//rtd_pr_vpq_info("2.HDR_PicInfo_TV030\n");
			ret = -1;
		} else {
			if (copy_to_user((void __user *)arg, HDR_info_bin, 131 * 4)) {
			//rtd_pr_vpq_info("3.HDR_PicInfo_TV030\n");
				ret = -1;
			} else
				ret = 0;
		}
		break;
	}
	case VPQ_EXTERN_IOC_SET_SPLIT_DEMO_TV030:
	{
		bool bOnOff;
		//rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_SPLIT_DEMO_TV030=%d\n",bOnOff);

		if (copy_from_user(&bOnOff, (int __user *)arg, sizeof(bool))) {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_SPLIT_DEMO_TV030 fail\n");
			ret = -1;
		} else {
			if (bOnOff == TRUE)
				Scaler_SetMagicPicture(SLR_MAGIC_STILLDEMO);
			else
				Scaler_SetMagicPicture(SLR_MAGIC_OFF);
		}
	}
	break;

	case VPQ_EXTERN_IOC_GET_LD_Enable_TV030:
	{
		unsigned char bEnable = Scaler_Get_LD_Enable();
		//rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_LD_Enable_TV030=%d\n",bEnable);

		if (copy_to_user((void __user *)arg, (void *)&bEnable, sizeof(unsigned char))) {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_LD_Enable_TV030 fail\n");
			ret = -1;
		} else
			ret = 0;

	}
	break;

	case VPQ_EXTERN_IOC_SET_COLORMATRIXSRGB_TV030:
	{
		RTK_COLOR_GAMUT_T ColorMatrixSRGB;
		//short sRGBMatrix[3][3];
		extern short BT2020_sRGB_APPLY[3][3];
		extern short DCI_P3_RGB_APPLY[3][3];
		extern short CCIR_709_sRGB_APPLY[3][3];
		extern unsigned char g_srgbForceUpdate;

		if (copy_from_user(&ColorMatrixSRGB,  (int __user *)arg,sizeof(RTK_COLOR_GAMUT_T))) {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_COLORMATRIXSRGB_TV030 fail\n");
			ret = -1;
		} else {
	//rtd_pr_vpq_err("yuan,VPQ_IOC_SET_COLORMATRIXSRGB=%d\n",ColorMatrixSRGB.mode);
	//rtd_pr_vpq_err("ColorMatrixSRGB.gamutmatrix_00 = %d\n", ColorMatrixSRGB.gamutmatrix_00);

		switch(ColorMatrixSRGB.mode){

		case 0://BT2020 ==>  CCIR 709
				CCIR_709_sRGB_APPLY[0][0] = ColorMatrixSRGB.gamutmatrix_00;
				CCIR_709_sRGB_APPLY[0][1] = ColorMatrixSRGB.gamutmatrix_01;
				CCIR_709_sRGB_APPLY[0][2] = ColorMatrixSRGB.gamutmatrix_02;
				CCIR_709_sRGB_APPLY[1][0] = ColorMatrixSRGB.gamutmatrix_10;
				CCIR_709_sRGB_APPLY[1][1] = ColorMatrixSRGB.gamutmatrix_11;
				CCIR_709_sRGB_APPLY[1][2] = ColorMatrixSRGB.gamutmatrix_12;
				CCIR_709_sRGB_APPLY[2][0] = ColorMatrixSRGB.gamutmatrix_20;
				CCIR_709_sRGB_APPLY[2][1] = ColorMatrixSRGB.gamutmatrix_21;
				CCIR_709_sRGB_APPLY[2][2] = ColorMatrixSRGB.gamutmatrix_22;

				break;

		case 1://DCI_P3   ==>  BT2020
				BT2020_sRGB_APPLY[0][0] = ColorMatrixSRGB.gamutmatrix_00;
				BT2020_sRGB_APPLY[0][1] = ColorMatrixSRGB.gamutmatrix_01;
				BT2020_sRGB_APPLY[0][2] = ColorMatrixSRGB.gamutmatrix_02;
				BT2020_sRGB_APPLY[1][0] = ColorMatrixSRGB.gamutmatrix_10;
				BT2020_sRGB_APPLY[1][1] = ColorMatrixSRGB.gamutmatrix_11;
				BT2020_sRGB_APPLY[1][2] = ColorMatrixSRGB.gamutmatrix_12;
				BT2020_sRGB_APPLY[2][0] = ColorMatrixSRGB.gamutmatrix_20;
				BT2020_sRGB_APPLY[2][1] = ColorMatrixSRGB.gamutmatrix_21;
				BT2020_sRGB_APPLY[2][2] = ColorMatrixSRGB.gamutmatrix_22;

				break;

		case 2://CCIR 709  ==>	DCI_P3
				DCI_P3_RGB_APPLY[0][0] = ColorMatrixSRGB.gamutmatrix_00;
				DCI_P3_RGB_APPLY[0][1] = ColorMatrixSRGB.gamutmatrix_01;
				DCI_P3_RGB_APPLY[0][2] = ColorMatrixSRGB.gamutmatrix_02;
				DCI_P3_RGB_APPLY[1][0] = ColorMatrixSRGB.gamutmatrix_10;
				DCI_P3_RGB_APPLY[1][1] = ColorMatrixSRGB.gamutmatrix_11;
				DCI_P3_RGB_APPLY[1][2] = ColorMatrixSRGB.gamutmatrix_12;
				DCI_P3_RGB_APPLY[2][0] = ColorMatrixSRGB.gamutmatrix_20;
				DCI_P3_RGB_APPLY[2][1] = ColorMatrixSRGB.gamutmatrix_21;
				DCI_P3_RGB_APPLY[2][2] = ColorMatrixSRGB.gamutmatrix_22;

				break;

		default:

			break;

		}
		 g_srgbForceUpdate = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_LOWDELAY_MODE_TV030:
	{
		bool bOnOff;

		if (copy_from_user(&bOnOff, (int __user *)arg, sizeof(bool))) {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_LOWDELAY_MODE_TV030 fail\n");
			ret = -1;
		} else {
			if (bOnOff == TRUE)
				Scaler_SetMagicPicture(SLR_MAGIC_STILLDEMO);
			else
				Scaler_SetMagicPicture(SLR_MAGIC_OFF);
		}
	}
	break;

	case VPQ_EXTERN_IOC_GET_HistoData_APL_TV030:
	{
		unsigned int DC_APL = 0;

		if (fwif_color_get_HistoData_APL(&DC_APL) == FALSE) {
			rtd_pr_vpq_err("kernel fwif_color_get_HistoData_APL fail\n");
			ret = -1;
		} else {
			if (drvif_color_get_WB_pattern_on())
				DC_APL = 940;
			if (copy_to_user((void __user *)arg, (void *)&DC_APL, sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_IOC_GET_HistoData_APL fail\n");
				ret = -1;
			} else
				ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_GET_HistoData_Min_TV030:
	{
		signed int DC_min = 0;

		if (fwif_color_get_HistoData_Min(&DC_min) == FALSE) {
			rtd_pr_vpq_err("kernel fwif_color_get_HistoData_Min fail\n");
			ret = -1;
		} else {
			if (copy_to_user((void __user *)arg, (void *)&DC_min, sizeof(signed int))) {
				rtd_pr_vpq_err("kernel VPQ_IOC_GET_HistoData_Min fail\n");
				ret = -1;
			} else
				ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_GET_HistoData_Max_TV030:
	{
		signed int DC_max = 0;

		if (fwif_color_get_HistoData_Max(&DC_max) == FALSE) {
			rtd_pr_vpq_err("kernel fwif_color_get_HistoData_Max fail\n");
			ret = -1;
		} else {
			if (copy_to_user((void __user *)arg, (void *)&DC_max, sizeof(signed int))) {
				rtd_pr_vpq_err("kernel VPQ_IOC_GET_HistoData_Max fail\n");
				ret = -1;
			} else
				ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_PARTTENFORAUTOGAMMA_TV030:
	{
		//rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_SET_PARTTENFORAUTOGAMMA_TV030\n");
		INTERNAL_PARTTEN pData;

		if (copy_from_user(&pData,	(void __user *)arg,sizeof(INTERNAL_PARTTEN))) {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_PARTTENFORAUTOGAMMA_TV030 fail\n");
			ret = -1;
		} else {
			if (pData.enable > 4)
				pData.enable = 0;
			Scaler_Set_Partten4AutoGamma(pData.enable,pData.r_Val,pData.g_Val,pData.b_Val);
			ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_GET_PIXELINFO_TV030:
	{
		//rtd_pr_vpq_info("Ioc VPQ_EXTERN_IOC_GET_PIXELINFO_TV030!!!!!!!!\n");
		SCREEN_PIXEL_INFO pixelInfo;
		if (copy_from_user(&pixelInfo, (int __user *)arg, sizeof(SCREEN_PIXEL_INFO))) {
			rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_GET_PIXELINFO_TV030 fail\n");
			ret = -1;
		} else {
			ret = 0;
			Scaler_Get_ScreenPixel_info(&pixelInfo);
			if (copy_to_user((void __user *)arg, (void *)&pixelInfo, sizeof(SCREEN_PIXEL_INFO))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_PIXELINFO_TV030 scaler_get_screen_pixel_info fail\n");
				ret = -1;
			} else
				ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_FILM_TABLE_TV030:
	{
		unsigned int film_mode_enable = 0;
		if (copy_from_user(&film_mode_enable,  (int __user *)arg,sizeof(unsigned int))) {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_FILM_TABLE_TV030 fail\n");
			ret = -1;
		} else {
			film_mode.film_status = film_mode_enable?28:0;
			down(get_DI_semaphore());
			drvif_module_film_mode((DRV_film_mode *) &film_mode);
			up(get_DI_semaphore());
			ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_GET_DYNAMIC_RANGE_TV030:
	{
		unsigned char DolbyMode = 0;
		unsigned char HdrMode = 0;
		unsigned char ret_Val = 0;

		SLR_VOINFO* pVOInfo = NULL;
		_system_setting_info *VIP_system_info_structure_table = NULL;

		VIP_system_info_structure_table = (_system_setting_info *)Scaler_GetShareMemVirAddr(SCALERIOC_VIP_system_info_structure);
		pVOInfo = Scaler_VOInfoPointer(Scaler_Get_CurVoInfo_plane());

		if (pVOInfo == NULL || VIP_system_info_structure_table == NULL) {
			rtd_pr_vpq_emerg( "kernel VPQ_EXTERN_IOC_GET_DYNAMIC_RANGE_TV030, point NULL\n");
			ret = -1;
		} else {
			if(cur_src == 0)
			{
				DolbyMode = VIP_system_info_structure_table->DolbyHDR_flag;//fwif_vip_DolbyHDR_check(SLR_MAIN_DISPLAY, _SRC_HDMI);
				HdrMode = VIP_system_info_structure_table ->HDR_flag;//fwif_vip_HDR10_check(SLR_MAIN_DISPLAY, _SRC_HDMI);
			} else if(cur_src == 1){
				DolbyMode = VIP_system_info_structure_table->DolbyHDR_flag;//fwif_vip_DolbyHDR_check(SLR_MAIN_DISPLAY, _SRC_VO);
				HdrMode = VIP_system_info_structure_table ->HDR_flag;;//fwif_vip_HDR10_check(SLR_MAIN_DISPLAY, _SRC_VO);
			}

			if(Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_STATE) == _MODE_STATE_ACTIVE) {
				if (Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_VDEC) {
					if((DolbyMode == 1)&& (HdrMode == HAL_VPQ_HDR_MODE_SDR)) {
						ret_Val = VPQ_DYNAMIC_RANGE_DOLBY_VISION;
					} else if ((DolbyMode == 0)&& (HdrMode == HAL_VPQ_HDR_MODE_HDR10)) {
						ret_Val = VPQ_DYNAMIC_RANGE_HDR10;
					} else if ((DolbyMode == 0)&& (HdrMode == HAL_VPQ_HDR_MODE_HLG)) {
						if (pVOInfo->transfer_characteristics == 14) {	// will be HLG if Scaler_color_HLG_support_HLG14() = 1
							ret_Val = VPQ_DYNAMIC_RANGE_HLG_14;
						} else {
							ret_Val = VPQ_DYNAMIC_RANGE_HLG_18;
						}
					} else if ((DolbyMode == 0)&& (HdrMode == HAL_VPQ_HDR_MODE_SDR)) {
						if (pVOInfo->transfer_characteristics == 14) {
							ret_Val = VPQ_DYNAMIC_RANGE_HLG_14;
						} else {
							ret_Val = VPQ_DYNAMIC_RANGE_SDR;
						}
					} else {
						ret_Val = VPQ_DYNAMIC_RANGE_SDR;
					}

				} else {
					if((DolbyMode == 1)&& (HdrMode == HAL_VPQ_HDR_MODE_SDR)) {
						ret_Val = VPQ_DYNAMIC_RANGE_DOLBY_VISION;
					} else if ((DolbyMode == 0)&& (HdrMode == HAL_VPQ_HDR_MODE_HDR10)) {
						ret_Val = VPQ_DYNAMIC_RANGE_HDR10;
					} else if ((DolbyMode == 0)&& (HdrMode == HAL_VPQ_HDR_MODE_HLG)) {
						ret_Val = VPQ_DYNAMIC_RANGE_HLG;
					} else {
						ret_Val = VPQ_DYNAMIC_RANGE_SDR;
					}
				}

				if (copy_to_user((void __user *)arg, (void *)&ret_Val, sizeof(unsigned char))){
					rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_DYNAMIC_RANGE_TV030 fail\n");
					ret = -1;
				}else{
					ret = 0;
				}

			} else {
				ret = -1;
			}
		}
	}
	break;


	case VPQ_EXTERN_IOC_SET_DYNAMIC_RANGE_CUR_SRC_TV030:
	{
		if (copy_from_user((void *)&cur_src, (const void __user *)arg, sizeof(unsigned char))){
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_DYNAMIC_RANGE_CUR_SRC_TV030 fail\n");
			ret = -1;
		} else {
			//rtd_pr_vpq_err("hzf=cur_src = %d", cur_src);
			ret = 0;
		}
	}
	break;

		case VPQ_EXTERN_IOC_SET_HDMI_MODE_TYPE_TV030:
		{
			unsigned char hdmi_mode;
			if (copy_from_user((void *)&hdmi_mode, (const void __user *)arg, sizeof(unsigned char))){
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_HDMI_MODE_TYPE_TV030 fail\n");
				ret = -1;
			} else {
				//rtd_pr_vpq_err("set_hdmi_mode_type = %d", hdmi_mode);
				Scaler_Set_hdmi_provider_type(hdmi_mode);
				ret = 0;
			}
		}
		break;

	case VPQ_EXTERN_IOC_SET_HIGHLIGHTWINDOW_FORDEMO_TV030:
	{

		RTK_DEMO_MODE_T DEMO_MODE_T;
		//unsigned int args = 0;
		if (copy_from_user(&DEMO_MODE_T,  (int __user *)arg,sizeof(RTK_DEMO_MODE_T))) {

			ret = -1;
		} else {

			//rtd_pr_vpq_info("[zjl]VPQ_EXTERN_IOC_SET_DEMO DEMO_MODE_T.DEMO_MODE[%u] DEMO_MODE_T.onOff(%d)	\n",DEMO_MODE_T.DEMO_MODE,DEMO_MODE_T.onOff);
			Scaler_SetLight_Window_forDemo((Light_Window_forDemo_E)DEMO_MODE_T.DEMO_MODE,(unsigned char)DEMO_MODE_T.onOff);
			ret = 0;
		}

	}
	break;

	case VPQ_EXTERN_IOC_SET_BRIGHTNESS_LUT_OETF_TV030:
	{
		//rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_BRIGHTNESS_LUT_OETF\n");
		BRIGHTNESS_LUT_OETF BRIGHTNESS_LUT_T;
		extern UINT16 OETF_LUT_R[1025];
		extern unsigned int EOTF_LUT_2084_nits[OETF_size];

		if (copy_from_user(&BRIGHTNESS_LUT_T,  (int __user *)arg,sizeof(BRIGHTNESS_LUT_OETF))) {
			ret = -1;
		} else {

			//rtd_pr_vpq_info("db408,yuan,un16_length=%d\n",BRIGHTNESS_LUT_T.un16_length);
			if(BRIGHTNESS_LUT_T.un16_length==256){
				Scaler_SetBrightness_LUT_OETF(OETF_LUT_R,&BRIGHTNESS_LUT_T);
				fwif_color_rtice_DM2_OETF_Set(OETF_LUT_R,OETF_LUT_R,OETF_LUT_R);
				drvif_fwif_color_Set_HDR10_EOTF_depNITS();
				drvif_DM2_EOTF_Set(EOTF_LUT_2084_nits, EOTF_LUT_2084_nits, EOTF_LUT_2084_nits);
			}
			else if(BRIGHTNESS_LUT_T.un16_length==1024){
			//GammaBlendOETF=1;
			Scaler_SetOETF2Gamma(&BRIGHTNESS_LUT_T);
			fwif_color_set_gamma_Magic();
			}

			ret = 0;
		}
	}
	break;


	case VPQ_EXTERN_IOC_GET_MEMCLD_ENABLE_TV030:
	{
		unsigned char bEnable = 0;
		bEnable =Scaler_GetLocalContrastEnable();
		if (Scaler_GetLocalContrastEnable() == FALSE) {
			rtd_pr_vpq_err("kernel Scaler_GetLocalContrastEnable fail\n");
			ret = -1;
		} else {
			if (copy_to_user((void __user *)arg, (void *)&bEnable, sizeof(unsigned char))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_MEMCLD_ENABLE_TV030 fail\n");
				ret = -1;
			} else
				ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_MEMCLD_ENABLE_TV030:
	{
		unsigned int args = 0;

		if (copy_from_user(&args,  (int __user *)arg,sizeof(unsigned int))) {

			ret = -1;
		} else {
			Scaler_SetLocalContrastEnable((unsigned char)args);
			ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_GET_ICM_7_Axis_TV030:
	{
		Scaler_get_ICM_7_Axis_from_user(&icm_7_axis_data);
		if (copy_to_user((void __user *)arg, (void *)&icm_7_axis_data, sizeof(ICM_H_7axis_table))) {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_ICM_7_Axis_TV030 fail\n");
			ret = -1;
		} else
		{
			ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_ICM_7_Axis_TV030:
	{
		if (copy_from_user(&icm_7_axis_data,  (void __user *)arg,sizeof(ICM_H_7axis_table))) {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_ICM_7_Axis_TV030 fail\n");
			ret = -1;
		} else {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_ICM_7_Axis_TV030 success\n");
			Scaler_set_ICM_7_Axis_from_user(&icm_7_axis_data);
			ret = 0;
		}
	}
	break;
	case VPQ_EXTERN_IOC_SET_DCC_PICTURE_MODE_WEIGHTING_TV030:
	{
		unsigned int weighting = 0;
		rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_DCC_PICTURE_MODE_WEIGHTING_TV030\n");
		if (copy_from_user(&weighting,  (void __user *)arg,sizeof(unsigned int))) {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_DCC_PICTURE_MODE_WEIGHTING_TV030 fail\n");
			ret = -1;
		} else {
			ret = 0;
			Scaler_Set_DCC_Picture_Mode_Weight(weighting);
		}
	}
	break;
	case VPQ_EXTERN_IOC_GET_DCC_PICTURE_MODE_WEIGHTING_TV030:
	{
		unsigned int weighting = 0;
		rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_DCC_PICTURE_MODE_WEIGHTING_TV030\n");
		weighting = (unsigned int)Scaler_Get_DCC_Picture_Mode_Weight();
		if (copy_to_user((void __user *)arg, (void *)&weighting, sizeof(unsigned int))) {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_DCC_PICTURE_MODE_WEIGHTING_TV030 fail\n");
			ret = -1;
		} else
			ret = 0;
	}
	break;
	case VPQ_EXTERN_IOC_SET_DECONTOUR_ENABLE_TV030:
	{
		unsigned char value = 0;
		rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_DECONTOUR_ENABLE_TV030\n");
		if (copy_from_user(&value,  (void __user *)arg,sizeof(unsigned char))) {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_DECONTOUR_ENABLE_TV030 fail\n");
			ret = -1;
		} else {
			ret = 0;
			Scaler_Set_I_De_Contour((unsigned char)value);
		}
	}
	break;
	case VPQ_EXTERN_IOC_GET_DECONTOUR_ENABLE_TV030:
	{
		unsigned char value = 0;
		rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_DECONTOUR_ENABLE_TV030\n");
		value = (unsigned char)Scaler_Get_I_De_Contour();
		if (copy_to_user((void __user *)arg, (void *)&value, sizeof(unsigned char))) {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_DECONTOUR_ENABLE_TV030 fail\n");
			ret = -1;
		} else
			ret = 0;
	}
	break;
	case VPQ_EXTERN_IOC_GET_LC_HISTOGRAM_TV030:
	{
		unsigned int  plc_out[16];
		Scaler_color_get_LC_His_dat(&plc_out[0]);
		if (copy_to_user((void __user *)arg, (void *)&plc_out[0], sizeof(plc_out) )) {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_LC_HISTOGRAM_TV030 fail\n");
			ret = -1;
		} else
			ret = 0;

	}
	break;

	case VPQ_EXTERN_IOC_SET_RGBWTOSRGBMATRIX_TV030:
	{
		//rtd_pr_vpq_info("[Kernel]VPQ_EXTERN_IOC_SET_RGBWTOSRGBMATRIX\n");
		unsigned char i,j;
		extern short BT709Target_sRGB_APPLY[3][3];
		extern short BT2020Target_sRGB_APPLY[3][3];
		extern short DCIP3Target_sRGB_APPLY[3][3];
		extern short ADOBERGBTarget_sRGB_APPLY[3][3];

		RTK_VPQ_sRGB_Matrix sRGB_T;
		extern unsigned char g_srgbForceUpdate;
		_system_setting_info *VIP_system_info_structure_table = NULL;
		VIP_system_info_structure_table = (_system_setting_info *)Scaler_GetShareMemVirAddr(SCALERIOC_VIP_system_info_structure);

		if (copy_from_user(&sRGB_T,  (int __user *)arg,sizeof(RTK_VPQ_sRGB_Matrix))) {
			ret = -1;
		} else {
		#if 0//for debug
			for(i=0;i<3;i++){
				 for(j=0;j<3;j++)
					rtd_pr_vpq_info("BT709Target[%d][%d]=%d\n",i,j,sRGB_T.BT709Target[i][j]);
				}
			for(i=0;i<3;i++){
				 for(j=0;j<3;j++)
					rtd_pr_vpq_info("sRGB_T->BT2020Target[%d][%d]=%d\n",i,j,sRGB_T.BT2020Target[i][j]);
				}
			for(i=0;i<3;i++){
				 for(j=0;j<3;j++)
					rtd_pr_vpq_info("sRGB_T->DCIP3Target[%d][%d]=%d\n",i,j,sRGB_T.DCIP3Target[i][j]);
				}
			for(i=0;i<3;i++){
				 for(j=0;j<3;j++)
					rtd_pr_vpq_info("sRGB_T->ADOBERGBTarget[%d][%d]=%d\n",i,j,sRGB_T.ADOBERGBTarget[i][j]);
				}
		#endif
			//-----------------------------------------------------------------------------
			for(i=0;i<3;i++){
				 for(j=0;j<3;j++)
					BT709Target_sRGB_APPLY[i][j]=sRGB_T.BT709Target[i][j];
				}
			for(i=0;i<3;i++){
				 for(j=0;j<3;j++)
					BT2020Target_sRGB_APPLY[i][j]=sRGB_T.BT2020Target[i][j];
				}
			for(i=0;i<3;i++){
				 for(j=0;j<3;j++)
					DCIP3Target_sRGB_APPLY[i][j]=sRGB_T.DCIP3Target[i][j];
				}
			for(i=0;i<3;i++){
				 for(j=0;j<3;j++)
					ADOBERGBTarget_sRGB_APPLY[i][j]=sRGB_T.ADOBERGBTarget[i][j];
				}
			fwif_color_set_sRGBMatrix();
		 	g_srgbForceUpdate = 0;

			ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_LC_HISBIN_TV030:
	{
		//LC histogram only 7 bin
		//rtd_pr_vpq_info("[Kernel]VPQ_EXTERN_IOC_SET_LC_HISBIN_TV030\n");
		ret = 0;

	}
	break;

	case VPQ_EXTERN_IOC_SET_PICTUREMODE_TV030:
	{
		RTK_VPQ_MODE_TYPE_TV030 args = 0;
		//rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_PICTUREMODE_TV030\n");
		if (copy_from_user(&args,  (int __user *)arg,sizeof(RTK_VPQ_MODE_TYPE_TV030))) {

			ret = -1;
		} else {
		ret = 0;
			Scaler_Set_PictureMode_PQsetting((RTK_VPQ_MODE_TYPE_TV030)args);
		}
	}
	break;
	case VPQ_EXTERN_IOC_SET_BRIGHTNESS_OSDLAYER_TV030:
	{
		//rtd_pr_vpq_info("[Kernel]VPQ_EXTERN_IOC_SET_BRIGHTNESS_OSDLAYER_TV030\n");
		ret = 0;

	}
	break;

	case VPQ_EXTERN_IOC_SET_CONTRAST_OSDLAYER_TV030:
	{
		//rtd_pr_vpq_info("[Kernel]VPQ_EXTERN_IOC_SET_CONTRAST_OSDLAYER_TV030\n");
		ret = 0;

	}
	break;

	case VPQ_EXTERN_IOC_SET_ICMTABLE:
	{
		extern SLR_VIP_TABLE* m_pVipTable;
		SLR_VIP_TABLE_ICM* pICM = NULL;
		pICM = (SLR_VIP_TABLE_ICM *)dvr_malloc_specific(sizeof(SLR_VIP_TABLE_ICM), GFP_DCU2_FIRST);

		if (pICM == NULL)
			return -1;

		if (copy_from_user(pICM, (void __user *)arg, sizeof(SLR_VIP_TABLE_ICM))) {
			rtd_pr_vpq_info("kernel copy VPQ_EXTERN_IOC_SET_ICMTABLE fail\n");
			rtd_pr_vpq_info("%s %d\n", __FUNCTION__, __LINE__);
			ret = -1;
		} else {
			rtd_pr_vpq_info("kernel copy VPQ_EXTERN_IOC_SET_ICMTABLE success\n");
			rtd_pr_vpq_info("%s %d\n", __FUNCTION__, __LINE__);
			if (m_pVipTable) {
				memcpy(m_pVipTable->tICM_ini, pICM->tICM_ini, sizeof(unsigned short)*VIP_ICM_TBL_X*VIP_ICM_TBL_Y*VIP_ICM_TBL_Z);
			}
			ret = 0;
		}
		dvr_free(pICM);
	}
	break;

	default:
		rtd_pr_vpq_debug("kernel IO command %d not found!\n", cmd);
		rtd_pr_vpq_info("VPQ_EXTERN_IOC cmd=0x%x unknown\n", cmd);
		return -1;

	}
	return ret;

}

static int VPQEX_set_Auto_ACC_1024_idx(unsigned int bit_number, unsigned int *pInData, unsigned int *pOutData)
{
	unsigned int i;
	signed char shiftbit = 0;

	if(bit_number < 14)
	{
		shiftbit = 14 - bit_number;
		for (i=0;i<(1024*3);i++)
			pOutData[i] = pInData[i]<<shiftbit;
	}
	else
	{
		shiftbit = bit_number - 14;
		for (i=0;i<(1024*3);i++)
			pOutData[i] = pInData[i]>>shiftbit;

	}
	rtd_pr_vpq_info("VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para VPQEX_set_Auto_ACC_1024_idx, bit_number=%d,shiftbit=%d,indata[0,512,1023]=%d,%d,%d,\n", 
		bit_number, shiftbit, pInData[0], pInData[512], pInData[1023]);

	return 0;
}

static int VPQEX_set_Auto_ACC(SLR_VIP_TABLE_AUTO_ACC_CTRL *pInAUTO_ACC_CTRL, SLR_VIP_TABLE_AUTO_ACC_CTRL *pOutAUTO_ACC_CTRL)
{
	int ret = 0;
	unsigned int index_number;
	unsigned int bit_number;
	unsigned int *pInData; 
	unsigned int *pOutData;
	
	if (pInAUTO_ACC_CTRL == NULL || pOutAUTO_ACC_CTRL == NULL)
	{
		rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para VPQEX_set_Auto_ACC,VPQEX_set_Auto_ACC, NULL\n");
		ret = -1;
	}
	else
	{
		index_number = pInAUTO_ACC_CTRL->Header[AutoACC_CTRL_index_num];
		bit_number = pInAUTO_ACC_CTRL->Header[AutoACC_CTRL_bit_num];
		pInData = &pInAUTO_ACC_CTRL->OUTGAMMA_TBL[0][0];
		pOutData = &pOutAUTO_ACC_CTRL->OUTGAMMA_TBL[0][0];
	
		if (index_number == 1024)
		{
			ret = VPQEX_set_Auto_ACC_1024_idx(bit_number, pInData, pOutData);
		}
		else
		{
			rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para VPQEX_set_Auto_ACC,index_number fail index_number=%d,\n", index_number);
			ret = -1;
		}
	}
	return ret;
}
extern unsigned char DCC_Dynamic_Active;
VIP_DEM_PANEL_INI_Panel_Luma_Facotry gHDR_EOTF_OETF_Facotry_Set = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, };
VIP_DEM_PANEL_INI_Color_Chromaticity_Facotry gColor_Chromaticity_Facotry_Set = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, };
extern SLR_HDR_TABLE* ptr_HDR_bin;
extern SLR_HDR_TABLE_v4* ptr_HDR_bin_v4;

long vpqex_ioctl(struct file *file, unsigned int cmd,  unsigned long arg)
{

	int ret = 0, i, j, k, ii, idx;
	HAL_VPQ_DATA_EXTERN_T hal_VPQ_DATA_EXTERN;
	unsigned long Data_addr;
	unsigned char *pBuff_tmp;
	unsigned short crc_cal, crc_get;
	unsigned int CRC_nByte;
	unsigned int sizeByte;
	//unsigned int *pTemp32;
	unsigned int crc_16_tmp_32;
	unsigned char temp8;
	unsigned int *pTemp32;
	unsigned short *pTemp16;
	unsigned char tbl_sel;
	unsigned char tmp0, tmp1;
	unsigned int demura_HEADER_tmp[DEMURA_TBL_HEADER_TBL_Max];
	unsigned int addrB, addrG, addrR, addrRGB;	
	unsigned int R_val, G_val, B_val;
	GAMUT_3D_LUT_17x17x17_T *pD3D_LUT_17x17x17 = NULL;	
	
	if(vpq_get_VPQ_TSK_Stop() ==1){
		rtd_pr_vpq_emerg( "vpq hal function block (%s:%d) /n", __FUNCTION__, __LINE__);
		return 0;
	}


	if (vpq_extern_ioctl_get_stop_run(cmd))
		return 0;

	if (_IOC_TYPE(cmd) == VPQ_EXTERN_IOC_MAGIC_TV030)
		return vpqex_ioctl_tv030(file, cmd, arg);

//	rtd_pr_vpq_info("vpqex_ioctl %d\n", __LINE__);
		rtd_pr_vpq_debug("vpqex_ioctl : %d \n",__LINE__);	

	if (VPQ_EXTERN_IOC_INIT == cmd) {
		sm_vpq_extern_init();
        return 0;
	} else if (VPQ_EXTERN_IOC_UNINIT == cmd) {
		PQ_Dev_Extern_Status = PQ_DEV_EXTERN_UNINIT;
		return 0;
	} else if (VPQ_EXTERN_IOC_OPEN == cmd || VPQ_EXTERN_IOC_CLOSE == cmd) {
		return 0;
	} else if (VPQ_EXTERN_IOC_INIT_PRJ_ID == cmd) {
		VIP_Customer_Project_ID_ENUM prj_id;
		if (copy_from_user(&prj_id, (int __user *)arg, sizeof(unsigned int))) {
			rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_INIT_PRJ_ID fail\n");
			return -1;
		} else {
			fwif_VIP_set_Project_ID(prj_id);
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_INIT_PRJ_ID = %d\n", prj_id);
			return 0;
		}
	}

//	if (PQ_Dev_Extern_Status != PQ_DEV_EXTERN_INIT_DONE)
//		return -1;

//	rtd_pr_vpq_info("vpqex_ioctl %d\n", __LINE__);
#ifdef Merlin3_rock_mark	//Merlin3 rock mark
	if (fwif_color_get_pq_demo_flag_rpc())
		return 0;
#endif
#ifndef SEND_VIP_MIDDLE_WARE_PIC_MODE_DATA
	if (vpqex_skip_middle_ware_picmode_data(cmd))
		return 0;
#endif

	switch (cmd) {
	/*
	case VPQ_EXTERN_IOC_INIT:
		if (PQ_DEV_EXTERN_UNINIT == PQ_Dev_Extern_Status || PQ_DEV_EXTERN_NOTHING == PQ_Dev_Extern_Status) {
			fwif_color_inv_gamma_control_back(SLR_MAIN_DISPLAY, 1);
			fwif_color_gamma_control_back(SLR_MAIN_DISPLAY, 1);
			PQ_Dev_Extern_Status = PQ_DEV_EXTERN_INIT_DONE;
		} else
			return -1;
		break;

	case VPQ_EXTERN_IOC_UNINIT:
		if (PQ_DEV_EXTERN_CLOSE == PQ_Dev_Extern_Status || PQ_DEV_EXTERN_INIT_DONE == PQ_Dev_Extern_Status) {
			PQ_Dev_Extern_Status = PQ_DEV_EXTERN_UNINIT;
		} else
			return -1;
		break;

	case VPQ_EXTERN_IOC_OPEN:
		if (PQ_DEV_EXTERN_CLOSE == PQ_Dev_Extern_Status || PQ_DEV_EXTERN_INIT_DONE == PQ_Dev_Extern_Status) {
			PQ_Dev_Extern_Status = PQ_DEV_EXTERN_OPEN_DONE;
		} else
			return -1;
		break;

	case VPQ_EXTERN_IOC_CLOSE:
		if (PQ_DEV_EXTERN_OPEN_DONE == PQ_Dev_Extern_Status) {
			PQ_Dev_Extern_Status = PQ_DEV_EXTERN_CLOSE;
		} else
			 return -1;
		break;
	*/

/************************************************************************
 *  TV001 External variables
 ************************************************************************/
 		case VPQ_EXTERN_IOC_REGISTER_NEW_VIPTABLE:
		{
			ret = xRegisterTable();
			if (ret) {
				ret = 0;
				rtd_pr_vpq_debug("viptable xRegisterTable success\n");
			}
			else {
				ret = -1;
				rtd_pr_vpq_debug("viptable xRegisterTable fail\n");
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_VGA_ADCGainOffset:
		{
			extern ADCGainOffset m_vgaGainOffset;
			if (copy_from_user(&m_vgaGainOffset, (int __user *)arg, sizeof(ADCGainOffset))) {
				rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_VGA_ADCGainOffset fail\n");
				ret = -1;
				break;
			} else {
				rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_VGA_ADCGainOffset success\n");
				ret = 0;
				#if 1
				rtd_pr_vpq_info("m_vgaGainOffset={\n");
				rtd_pr_vpq_info("%x %x %x ", m_vgaGainOffset.Gain_R, m_vgaGainOffset.Gain_G, m_vgaGainOffset.Gain_B);
				rtd_pr_vpq_info("} end\n");
				#endif
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_YPBPR_ADCGainOffset:
		{
			extern ADCGainOffset m_yPbPrGainOffsetData[SR_MAX_YPBPR_GAINOFFSET_MODE] ;
			if (copy_from_user(&m_yPbPrGainOffsetData, (int __user *)arg, sizeof(m_yPbPrGainOffsetData))) {
				rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_YPBPR_ADCGainOffset fail\n");
				ret = -1;
				break;
			} else {
				rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_YPBPR_ADCGainOffset success, sizeof(m_yPbPrGainOffsetData)=%zu\n", sizeof(m_yPbPrGainOffsetData));
				ret = 0;
				#if 1
				rtd_pr_vpq_debug("m_yPbPrGainOffsetData[16]={\n");
				for (i=0; i<SR_MAX_YPBPR_GAINOFFSET_MODE; i++) {
					//for (j=0; j<VIP_QUALITY_FUNCTION_TOTAL_CHECK; j++) {
						rtd_pr_vpq_info("%x %x %x ", m_yPbPrGainOffsetData[i].Gain_R, m_yPbPrGainOffsetData[i].Gain_G, m_yPbPrGainOffsetData[i].Gain_B);
					//}
					rtd_pr_vpq_info("\n");
				}
				rtd_pr_vpq_info("} end\n");
				#endif
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_SLR_VIP_TABLE_CUSTOM_TV001:
		{
			extern void* m_pVipTableCustom; /* SLR_VIP_TABLE_CUSTOM_TV001  */

			if (copy_from_user((SLR_VIP_TABLE_CUSTOM_TV001*)m_pVipTableCustom, (int __user *)arg, sizeof(SLR_VIP_TABLE_CUSTOM_TV001))) {
				rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_SLR_VIP_TABLE_CUSTOM_TV001 fail\n");
				ret = -1;
				break;
			} else {
				SLR_VIP_TABLE_CUSTOM_TV001* tmp_pVipTableCustom = (SLR_VIP_TABLE_CUSTOM_TV001*)m_pVipTableCustom;
				rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_SLR_VIP_TABLE_CUSTOM_TV001 success\n");
				ret = 0;
				#if 1
				rtd_pr_vpq_info("m_pVipTableCustom->InitParam[16]={\n");
				for (i=0; i<16; i++) {
					//for (j=0; j<VIP_QUALITY_FUNCTION_TOTAL_CHECK; j++) {
						rtd_pr_vpq_info("%x ", tmp_pVipTableCustom->InitParam[i]);
					//}
					//rtd_pr_vpq_info("\n");
				}
				rtd_pr_vpq_info("} end\n");

				rtd_pr_vpq_info("m_pVipTableCustom->DCR_TABLE[][][]={\n");
				for (i=0; i<DCR_TABLE_ROW_NUM; i++) {
					for (j=0; j<DCR_TABLE_COL_NUM; j++) {
						rtd_pr_vpq_info("%x ", tmp_pVipTableCustom->DCR_TABLE[New_DCR_TABLE_NUM-1][i][j]);
					}
					rtd_pr_vpq_info("\n");
				}
				rtd_pr_vpq_info("} end\n");
				#endif
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_SLR_VIP_TABLE:
		{
			extern SLR_VIP_TABLE* m_pVipTable;
            #ifdef yc_tbl
            unsigned char vd_src=0;
            unsigned char YCDLEAY_index=0;
            #endif
			rtd_pr_vpq_info("%s %d\n", __FUNCTION__, __LINE__);

			#if 0
				rtd_pr_vpq_info("original m_defaultVipTable.VIP_QUALITY_Coef[][]={\n");
				for (i=0; i<3; i++) {
					for (j=0; j<VIP_QUALITY_FUNCTION_TOTAL_CHECK; j++) {
						rtd_pr_vpq_info("%x ", m_defaultVipTable.VIP_QUALITY_Coef[i][j]);
					}
					rtd_pr_vpq_info("\n");
				}
				rtd_pr_vpq_info("} end\n");

				rtd_pr_vpq_info("m_defaultVipTable.DrvSetting_Skip_Flag[]={\n");
				for (i=0; i<DrvSetting_Skip_Flag_item_Max; i++) {
					//for (j=0; j<VIP_QUALITY_FUNCTION_TOTAL_CHECK; j++) {
						rtd_pr_vpq_info("%x ", m_defaultVipTable.DrvSetting_Skip_Flag[i]);
					//}
					//rtd_pr_vpq_info("\n");
				}
				rtd_pr_vpq_info("} end\n");
			#endif
			if (copy_from_user(m_pVipTable, (int __user *)arg, sizeof(SLR_VIP_TABLE))) {
				rtd_pr_vpq_err("kernel copy SLR_VIP_TABLE fail\n");
				rtd_pr_vpq_info("%s %d\n", __FUNCTION__, __LINE__);

				ret = -1;
				break;
			} else {
				rtd_pr_vpq_debug("kernel copy SLR_VIP_TABLE success\n");
				rtd_pr_vpq_info("%s %d\n", __FUNCTION__, __LINE__);
				ret = 0;


				#if 1
				rtd_pr_vpq_info("m_pVipTable->VIP_QUALITY_Coef[][]={\n");
				for (i=0; i<3; i++) {
					for (j=0; j<VIP_QUALITY_FUNCTION_TOTAL_CHECK; j++) {
						rtd_pr_vpq_info("%x ", m_pVipTable->VIP_QUALITY_Coef[i][j]);
					}
					rtd_pr_vpq_info("\n");
				}
				rtd_pr_vpq_info("} end\n");

				rtd_pr_vpq_info("m_pVipTable->DrvSetting_Skip_Flag[]={\n");
				for (i=0; i<DrvSetting_Skip_Flag_item_Max; i++) {
					//for (j=0; j<VIP_QUALITY_FUNCTION_TOTAL_CHECK; j++) {
						rtd_pr_vpq_info("%x ", m_pVipTable->DrvSetting_Skip_Flag[i]);
					//}
					//rtd_pr_vpq_info("\n");
				}
				rtd_pr_vpq_info("} end\n");
				#endif
				DCC_Dynamic_Active = 1;
			}

            #ifdef yc_tbl

                for (vd_src=0; vd_src<VDSRC_MAX; vd_src++){
                      ATV_VD_Table[vd_src][VD_PARA_HUE]  = m_pVipTable->VD_ConBriHueSat[vd_src].VD_hue       ;
                      ATV_VD_Table[vd_src][VD_PARA_SAT]  = m_pVipTable->VD_ConBriHueSat[vd_src].VD_saturation;
                      ATV_VD_Table[vd_src][VD_PARA_CON]  = m_pVipTable->VD_ConBriHueSat[vd_src].VD_contrast  ;
                      ATV_VD_Table[vd_src][VD_PARA_BRI]  = m_pVipTable->VD_ConBriHueSat[vd_src].VD_brightness;              
                }
                for (vd_src=0; vd_src<VDSRC_MAX; vd_src++){
                      AV_VD_Table[vd_src][VD_PARA_HUE]   = m_pVipTable->VD_ConBriHueSat[vd_src+VDSRC_MAX].VD_hue       ;
                      AV_VD_Table[vd_src][VD_PARA_SAT]   = m_pVipTable->VD_ConBriHueSat[vd_src+VDSRC_MAX].VD_saturation;
                      AV_VD_Table[vd_src][VD_PARA_CON]   = m_pVipTable->VD_ConBriHueSat[vd_src+VDSRC_MAX].VD_contrast  ;
                      AV_VD_Table[vd_src][VD_PARA_BRI]   = m_pVipTable->VD_ConBriHueSat[vd_src+VDSRC_MAX].VD_brightness;              
                }  
                
                for (vd_src=0; vd_src<FORMAT_MAX; vd_src++){
                    for(YCDLEAY_index=0; YCDLEAY_index<YCDLEAY_MAX; YCDLEAY_index++){
                      VPQ_YCBCR_DELAY_TABLE[0][YCSEP_STATUS_1D][INPUT_SRC_AV][vd_src][YCDLEAY_index]    = m_pVipTable->VPQ_YCBCR_DELAY_TABLE[YCSEP_STATUS_1D][INPUT_SRC_AV][vd_src][YCDLEAY_index];//cvbs 1D
                      VPQ_YCBCR_DELAY_TABLE[0][YCSEP_STATUS_1D][INPUT_SRC_TV][vd_src][YCDLEAY_index]    = m_pVipTable->VPQ_YCBCR_DELAY_TABLE[YCSEP_STATUS_1D][INPUT_SRC_TV][vd_src][YCDLEAY_index];//tv 1D
                      VPQ_YCBCR_DELAY_TABLE[0][YCSEP_STATUS_2D3D][INPUT_SRC_AV][vd_src][YCDLEAY_index]  = m_pVipTable->VPQ_YCBCR_DELAY_TABLE[YCSEP_STATUS_2D3D][INPUT_SRC_AV][vd_src][YCDLEAY_index];//cvbs 2D
                      VPQ_YCBCR_DELAY_TABLE[0][YCSEP_STATUS_2D3D][INPUT_SRC_TV][vd_src][YCDLEAY_index]  = m_pVipTable->VPQ_YCBCR_DELAY_TABLE[YCSEP_STATUS_2D3D][INPUT_SRC_TV][vd_src][YCDLEAY_index];//tv 2D
                    }
                }   
                rtd_pr_vpq_info("VD from PQ.bin\n");
            #endif
		}
		break;

		case VPQ_EXTERN_IOC_SET_PIC_MODE_DATA:
		{
			extern PIC_MODE_DATA*  m_picModeTable;

			if (copy_from_user(m_picModeTable, (int __user *)arg, sizeof(PIC_MODE_DATA))) {
				rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_PIC_MODE_DATA fail\n");
				ret = -1;
				break;
			} else {
				rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_PIC_MODE_DATA success\n");
				ret = 0;
				#if 1
				rtd_pr_vpq_info("m_picModeTable={\n");
				rtd_pr_vpq_info("%x %x %x \n", m_picModeTable->picMode[PICTURE_MODE_USER].Brightness, m_picModeTable->picMode[PICTURE_MODE_USER].Contrast, m_picModeTable->picMode[PICTURE_MODE_USER].Saturation);
				rtd_pr_vpq_info("%x %x %x \n", m_picModeTable->picMode[PICTURE_MODE_VIVID].Brightness, m_picModeTable->picMode[PICTURE_MODE_VIVID].Contrast, m_picModeTable->picMode[PICTURE_MODE_VIVID].Saturation);
				rtd_pr_vpq_info("%x %x %x \n", m_picModeTable->picMode[PICTURE_MODE_STD].Brightness, m_picModeTable->picMode[PICTURE_MODE_STD].Contrast, m_picModeTable->picMode[PICTURE_MODE_STD].Saturation);
				rtd_pr_vpq_info("} end\n");
				#endif
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_PIC_MODE_DATA_defaultPictureModeSet:
		{
			extern SLR_PICTURE_MODE_DATA  m_defaultPictureModeSet[];

			if (copy_from_user(&m_defaultPictureModeSet, (int __user *)arg, sizeof(SLR_PICTURE_MODE_DATA)*7)) {
				rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_PIC_MODE_DATA_defaultPictureModeSet fail\n");
				ret = -1;
				break;
			} else {
				rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_PIC_MODE_DATA_defaultPictureModeSet success\n");
				ret = 0;
				#if 1
				rtd_pr_vpq_info("m_defaultPictureModeSet={\n");
				rtd_pr_vpq_info("%x %x %x \n", m_defaultPictureModeSet[PICTURE_MODE_USER].Brightness, m_defaultPictureModeSet[PICTURE_MODE_USER].Contrast, m_defaultPictureModeSet[PICTURE_MODE_USER].Saturation);
				rtd_pr_vpq_info("%x %x %x \n", m_defaultPictureModeSet[PICTURE_MODE_VIVID].Brightness, m_defaultPictureModeSet[PICTURE_MODE_VIVID].Contrast, m_defaultPictureModeSet[PICTURE_MODE_VIVID].Saturation);
				rtd_pr_vpq_info("%x %x %x \n", m_defaultPictureModeSet[PICTURE_MODE_GAME].Brightness, m_defaultPictureModeSet[PICTURE_MODE_GAME].Contrast, m_defaultPictureModeSet[PICTURE_MODE_GAME].Saturation);
				rtd_pr_vpq_info("} end\n");
				#endif
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_PIC_MODE_DATA_DolbyHDRPictureModeSet:
		{
			extern SLR_PICTURE_MODE_DATA  m_DolbyHDRPictureModeSet[];

			if (copy_from_user(&m_DolbyHDRPictureModeSet, (int __user *)arg, sizeof(SLR_PICTURE_MODE_DATA)*7)) {
				rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_PIC_MODE_DATA_DolbyHDRPictureModeSet fail\n");
				ret = -1;
				break;
			} else {
				rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_PIC_MODE_DATA_DolbyHDRPictureModeSet success\n");
				ret = 0;
				#if 1
				rtd_pr_vpq_info("m_DolbyHDRPictureModeSet={\n");
				rtd_pr_vpq_info("%x %x %x \n", m_DolbyHDRPictureModeSet[PICTURE_MODE_USER].Brightness, m_DolbyHDRPictureModeSet[PICTURE_MODE_USER].Contrast, m_DolbyHDRPictureModeSet[PICTURE_MODE_USER].Saturation);
				rtd_pr_vpq_info("%x %x %x \n", m_DolbyHDRPictureModeSet[PICTURE_MODE_VIVID].Brightness, m_DolbyHDRPictureModeSet[PICTURE_MODE_VIVID].Contrast, m_DolbyHDRPictureModeSet[PICTURE_MODE_VIVID].Saturation);
				rtd_pr_vpq_info("%x %x %x \n", m_DolbyHDRPictureModeSet[PICTURE_MODE_STD].Brightness, m_DolbyHDRPictureModeSet[PICTURE_MODE_STD].Contrast, m_DolbyHDRPictureModeSet[PICTURE_MODE_STD].Saturation);
				rtd_pr_vpq_info("} end\n");
				#endif
			}
		}
		break;




		case VPQ_EXTERN_IOC_SET_StructColorDataType:
		{
			extern StructColorDataType*  m_colorDataTable;

			if (copy_from_user(m_colorDataTable, (int __user *)arg, sizeof(StructColorDataType))) {
				rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataType fail\n");
				ret = -1;
				break;
			} else {
				rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataType success\n");
				ret = 0;
				#if 1
				rtd_pr_vpq_info("m_colorDataTable={\n");
				rtd_pr_vpq_info("%x %x %x ", m_colorDataTable->MBPeaking, m_colorDataTable->Intensity, m_colorDataTable->Gamma);
				rtd_pr_vpq_info("} end\n");
				#endif
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_StructColorDataType_defaultSDColorData:
		{
			extern StructColorDataType  m_defaultSDColorData;

			if (copy_from_user(&m_defaultSDColorData, (int __user *)arg, sizeof(StructColorDataType))) {
				rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataType_defaultSDColorData fail\n");
				ret = -1;
				break;
			} else {
				rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataType_defaultSDColorData success\n");
				ret = 0;
				#if 1
				rtd_pr_vpq_info("m_defaultSDColorData={\n");
				rtd_pr_vpq_info("%x %x %x ", m_defaultSDColorData.MBPeaking, m_defaultSDColorData.Intensity, m_defaultSDColorData.Gamma);
				rtd_pr_vpq_info("} end\n");
				#endif
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_StructColorDataType_defaultHDColorData:
		{
			extern StructColorDataType  m_defaultHDColorData;

			if (copy_from_user(&m_defaultHDColorData, (int __user *)arg, sizeof(StructColorDataType))) {
				rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataType_defaultHDColorData fail\n");
				ret = -1;
				break;
			} else {
				rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataType_defaultHDColorData success\n");
				ret = 0;
				#if 1
				rtd_pr_vpq_info("m_defaultHDColorData={\n");
				rtd_pr_vpq_info("%x %x %x ", m_defaultHDColorData.MBPeaking, m_defaultHDColorData.Intensity, m_defaultHDColorData.Gamma);
				rtd_pr_vpq_info("} end\n");
				#endif
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_COLOR_TEMP_DATA:
		{
			extern COLOR_TEMP_DATA*  m_colorTempData;

			if (copy_from_user(m_colorTempData, (int __user *)arg, sizeof(COLOR_TEMP_DATA))) {
				rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_COLOR_TEMP_DATA fail\n");
				ret = -1;
				break;
			} else {
				rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_COLOR_TEMP_DATA success\n");
				ret = 0;
				#if 1
				rtd_pr_vpq_info("m_colorTempData={\n");
				rtd_pr_vpq_info("%x %x %x \n", m_colorTempData->colorTempData[SLR_COLORTEMP_USER].R_val, m_colorTempData->colorTempData[SLR_COLORTEMP_USER].G_val, m_colorTempData->colorTempData[SLR_COLORTEMP_USER].B_val);
				rtd_pr_vpq_info("%x %x %x \n", m_colorTempData->colorTempData[SLR_COLORTEMP_NORMAL].R_val, m_colorTempData->colorTempData[SLR_COLORTEMP_NORMAL].G_val, m_colorTempData->colorTempData[SLR_COLORTEMP_NORMAL].B_val);
				rtd_pr_vpq_info("%x %x %x \n", m_colorTempData->colorTempData[SLR_COLORTEMP_WARMER].R_val, m_colorTempData->colorTempData[SLR_COLORTEMP_WARMER].G_val, m_colorTempData->colorTempData[SLR_COLORTEMP_WARMER].B_val);
				rtd_pr_vpq_info("} end\n");
				#endif
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_COLOR_TEMP_DATA_defColorTempSet:
		{
			extern SLR_COLORTEMP_DATA  m_defColorTempSet[];
			if (copy_from_user(&m_defColorTempSet, (int __user *)arg, sizeof(SLR_COLORTEMP_DATA)*SLR_COLORTEMP_MAX_NUM)) {
				rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_COLOR_TEMP_DATA_defColorTempSet fail\n");
				ret = -1;
				break;
			} else {

				rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_COLOR_TEMP_DATA_defColorTempSet success\n");
				ret = 0;
				#if 1
				rtd_pr_vpq_info("m_defColorTempSet={\n");
				rtd_pr_vpq_info("%x %x %x \n", m_defColorTempSet[SLR_COLORTEMP_USER].R_val, m_defColorTempSet[SLR_COLORTEMP_USER].G_val, m_defColorTempSet[SLR_COLORTEMP_USER].B_val);
				rtd_pr_vpq_info("%x %x %x \n", m_defColorTempSet[SLR_COLORTEMP_NORMAL].R_val, m_defColorTempSet[SLR_COLORTEMP_NORMAL].G_val, m_defColorTempSet[SLR_COLORTEMP_NORMAL].B_val);
				rtd_pr_vpq_info("%x %x %x \n", m_defColorTempSet[SLR_COLORTEMP_WARMER].R_val, m_defColorTempSet[SLR_COLORTEMP_WARMER].G_val, m_defColorTempSet[SLR_COLORTEMP_WARMER].B_val);
				rtd_pr_vpq_info("} end\n");
				#endif
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_StructColorDataFacModeType:
		{
			extern StructColorDataFacModeType* m_colorFacModeTable;

			if (copy_from_user(m_colorFacModeTable, (int __user *)arg, sizeof(StructColorDataFacModeType))) {
				rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataFacModeType fail\n");
				ret = -1;
				break;
			} else {
				rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataFacModeType success\n");
				ret = 0;
				#if 1
				rtd_pr_vpq_info("m_colorFacModeTable={\n");
				rtd_pr_vpq_info("%x %x %x ", m_colorFacModeTable->Brightness_0, m_colorFacModeTable->Brightness_50, m_colorFacModeTable->Brightness_100);
				rtd_pr_vpq_info("} end\n");
				#endif
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_defaultColorFacMode:
		{
			extern StructColorDataFacModeType m_defaultColorFacMode;

			if (copy_from_user(&m_defaultColorFacMode, (int __user *)arg, sizeof(StructColorDataFacModeType))) {
				rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_defaultColorFacMode fail\n");
				ret = -1;
				break;
			} else {
				rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_defaultColorFacMode success\n");
				ret = 0;
				#if 1
				rtd_pr_vpq_info("m_defaultColorFacMode={\n");
				rtd_pr_vpq_info("%x %x %x ", m_defaultColorFacMode.Brightness_0, m_defaultColorFacMode.Brightness_50, m_defaultColorFacMode.Brightness_100);
				rtd_pr_vpq_info("} end\n");
				#endif
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_AvColorFacMode:
		{
			extern StructColorDataFacModeType m_AvColorFacMode;

			if (copy_from_user(&m_AvColorFacMode, (int __user *)arg, sizeof(StructColorDataFacModeType))) {
				rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_AvColorFacMode fail\n");
				ret = -1;
				break;
			} else {
				rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_AvColorFacMode success\n");
				ret = 0;
				#if 1
				rtd_pr_vpq_info("m_AvColorFacMode={\n");
				rtd_pr_vpq_info("%x %x %x ", m_AvColorFacMode.Brightness_0, m_AvColorFacMode.Brightness_50, m_AvColorFacMode.Brightness_100);
				rtd_pr_vpq_info("} end\n");
				#endif
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_YppColorFacMode:
		{
			extern StructColorDataFacModeType m_YppColorFacMode;
			if (copy_from_user(&m_YppColorFacMode, (int __user *)arg, sizeof(StructColorDataFacModeType))) {
				rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_YppColorFacMode fail\n");
				ret = -1;
				break;
			} else {
				rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_YppColorFacMode success\n");
				ret = 0;
				#if 1
				rtd_pr_vpq_info("m_YppColorFacMode={\n");
				rtd_pr_vpq_info("%x %x %x ", m_YppColorFacMode.Brightness_0, m_YppColorFacMode.Brightness_50, m_YppColorFacMode.Brightness_100);
				rtd_pr_vpq_info("} end\n");
				#endif
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_VgaColorFacMode:
		{
			extern StructColorDataFacModeType m_VgaColorFacMode;

			if (copy_from_user(&m_VgaColorFacMode, (int __user *)arg, sizeof(StructColorDataFacModeType))) {
				rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_VgaColorFacMode fail\n");
				ret = -1;
				break;
			} else {
				rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_VgaColorFacMode success\n");
				ret = 0;
				#if 1
				rtd_pr_vpq_info("m_VgaColorFacMode={\n");
				rtd_pr_vpq_info("%x %x %x ", m_VgaColorFacMode.Brightness_0, m_VgaColorFacMode.Brightness_50, m_VgaColorFacMode.Brightness_100);
				rtd_pr_vpq_info("} end\n");
				#endif
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_HdmiColorFacMode:
		{
			extern StructColorDataFacModeType m_HdmiColorFacMode;

			if (copy_from_user(&m_HdmiColorFacMode, (int __user *)arg, sizeof(StructColorDataFacModeType))) {
				rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_HdmiColorFacMode fail\n");
				ret = -1;
				break;
			} else {
				rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_HdmiColorFacMode success\n");
				ret = 0;
				#if 1
				rtd_pr_vpq_info("m_HdmiColorFacMode={\n");
				rtd_pr_vpq_info("%x %x %x ", m_HdmiColorFacMode.Brightness_0, m_HdmiColorFacMode.Brightness_50, m_HdmiColorFacMode.Brightness_100);
				rtd_pr_vpq_info("} end\n");
				#endif
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_HdmiSDColorFacMode:
		{
			extern StructColorDataFacModeType m_HdmiSDColorFacMode;

			if (copy_from_user(&m_HdmiSDColorFacMode, (int __user *)arg, sizeof(StructColorDataFacModeType))) {
				rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_HdmiSDColorFacMode fail\n");
				ret = -1;
				break;
			} else {
				rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataFacModeType_HdmiSDColorFacMode success\n");
				ret = 0;
				#if 1
				rtd_pr_vpq_info("m_HdmiSDColorFacMode={\n");
				rtd_pr_vpq_info("%x %x %x ", m_HdmiSDColorFacMode.Brightness_0, m_HdmiSDColorFacMode.Brightness_50, m_HdmiSDColorFacMode.Brightness_100);
				rtd_pr_vpq_info("} end\n");
				#endif
			}
		}
		break;

		case VPQ_EXTERN_IOC_GET_CUR_SRC_FMT:
		{
			UINT32 srcfmt = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_CUR_SRC_FMT\n");
			srcfmt = Scaler_InputSrcFormat(Scaler_color_get_pq_src_idx());
			if (copy_to_user((void __user *)arg, (void *)&srcfmt, sizeof(UINT32))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_CUR_SRC_FMT2 fail\n");
				ret = -1;
			} else
				ret = 0;
		}
		break;

		case VPQ_EXTERN_IOC_GET_VIP_SRC_TIMING:
		{
			unsigned int vipsource = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_VIP_SRC_TIMING\n");

			vipsource = fwif_vip_source_check(3, NOT_BY_DISPLAY);
			if (copy_to_user((void __user *)arg, (void *)&vipsource, sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_VIP_SRC_TIMING fail\n");
				ret = -1;
			} else
				ret = 0;
		}
		break;

		case VPQ_EXTERN_IOC_ACCESS_OSD_byParameter:
		{
			extern SLR_VIP_TABLE_OSDSR VIP_TABLE_OSDSR;
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_ACCESS_OSD_byParameter hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} else {
				Data_addr = hal_VPQ_DATA_EXTERN.pData;

				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_ACCESS_OSD_byParameter,version=%d,cmd=%d,length=%d,pData=%x,\n",
					hal_VPQ_DATA_EXTERN.version, hal_VPQ_DATA_EXTERN.wId, hal_VPQ_DATA_EXTERN.length, hal_VPQ_DATA_EXTERN.pData);
				
				if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Set) 
				{
					if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_ACCESS_OSD_byParameter_OSDSR_table_from_ini) 
					{
						if (hal_VPQ_DATA_EXTERN.length != sizeof(SLR_VIP_TABLE_OSDSR)) 
						{
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_ACCESS_OSD_byParameter len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						} 
						else 
						{
							if(copy_from_user(&VIP_TABLE_OSDSR, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
							{
								rtd_pr_vpq_err("VPQ_EXTERN_IOC_ACCESS_OSD_byParameter table copy fail\n");
								ret = -1;
							} else {
								rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_ACCESS_OSD_byParameter=%d,%d,\n", 
									VIP_TABLE_OSDSR.osdsr_table.data[0][0], 
									VIP_TABLE_OSDSR.osdsr_table.data[VIP_DRV_OSDSR_TBL_NUM-1][VIP_DRV_OSDSR_ITEMS_MAX-1]);
								
							}
						}

					}
					else 
					{
						rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_ACCESS_OSD_byParameter ver error ver=%d,\n", hal_VPQ_DATA_EXTERN.version);
						ret = -1;
					}
				}	
				else if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Get) 
				{
					rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_OSD_byParameter NO get\n");
					ret = -1;				
				}
				else 
				{
					rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_OSD_byParameter only get or set\n");
					ret = -1;
				}				
			}	
		}
		break;

		case VPQ_EXTERN_IOC_SET_StructColorDataFacMode_EXTERN:
		{
			extern unsigned char g_bUseMiddleWareOSDmap; //for Driver Base MiddleWare OSD map
			extern StructColorDataFacModeType g_curColorFacTableEx; //for Driver Base MiddleWare OSD map

			if (copy_from_user(&g_curColorFacTableEx, (void __user *)arg, sizeof(StructColorDataFacModeType))) {
				rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataFacMode_EXTERN fail\n");
				ret = -1;
				break;
			} else {
				//rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_StructColorDataFacMode_EXTERN success\n");
				g_bUseMiddleWareOSDmap = 1;
				if ((g_curColorFacTableEx.Hue_0<g_curColorFacTableEx.Hue_25) && 
						(g_curColorFacTableEx.Hue_0<g_curColorFacTableEx.Hue_50) &&
						(g_curColorFacTableEx.Hue_0<g_curColorFacTableEx.Hue_75) &&
						(g_curColorFacTableEx.Hue_0<g_curColorFacTableEx.Hue_100)) {	// center is 128
					
					rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_StructColorDataFacMode_EXTERN, ap center is 128\n");
				} else {	// center is 0
					rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_StructColorDataFacMode_EXTERN, ap center is 0\n");
					g_curColorFacTableEx.Hue_0 = g_curColorFacTableEx.Hue_0 + 128;
					g_curColorFacTableEx.Hue_25 = g_curColorFacTableEx.Hue_25 + 128;
					g_curColorFacTableEx.Hue_50 = g_curColorFacTableEx.Hue_50 + 128;
					g_curColorFacTableEx.Hue_75 = g_curColorFacTableEx.Hue_75 + 128;
					g_curColorFacTableEx.Hue_100 = g_curColorFacTableEx.Hue_100 + 128;
				}
				ret = 0;
			}
		}
		break;

		case VPQ_EXTERN_IOC_GET_VIP_SRC_RESYNC:
		{
			unsigned int vip_src_resync = 0;
			vip_src_resync = Scaler_Get_VIP_src_resync_flag();

			if (copy_to_user((void __user *)arg, (void *)&vip_src_resync, sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_VIP_SRC_RESYNC fail\n");
				ret = -1;
			} else {
				ret = 0;
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_VIP_SRC_RESYNC:
		{
			Scaler_Set_PicMode_VIP_src(fwif_vip_source_check(3, NOT_BY_DISPLAY));
		}
		break;

		case VPQ_EXTERN_IOC_SET_VIP_SET_VIP_HANDLER_RESET:
		{
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_VIP_SET_VIP_HANDLER_RESET\n");

			ret = Scaler_Set_VIP_HANDLER_RESET();

			if (ret) {
				ret = 0;
				rtd_pr_vpq_info("Scaler_Set_VIP_HANDLER_RESET success\n");
			}
			else {
				ret = -1;
				rtd_pr_vpq_info("Scaler_Set_VIP_HANDLER_RESET fail\n");
			}
		}
		break;

		case VPQ_EXTERN_IOC_GET_HDR3DLUT_OFFSET:
		{
			int offset = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_HDR3DLUT_OFFSET\n");

			offset = (int)Scaler_GetHDR3DLUT_Offset();
			if (copy_to_user((void __user *)arg, (void *)&offset, sizeof(int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_HDR3DLUT_OFFSET fail\n");
				ret = -1;
			} else
				ret = 0;
		}
		break;

		case VPQ_EXTERN_IOC_SET_HDR3DLUT_OFFSET: 
		{
			int args = 0;

			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_HDR3DLUT_OFFSET\n");
			if (copy_from_user(&args,  (int __user *)arg,sizeof(int))) {
				ret = -1;
			} else {
				ret = 0;
				Scale_SetHDR3DLUT_Offset(args);
			}
		}
		break;

		case VPQ_EXTERN_IOC_GET_BRIGHTNESS:
		{
			unsigned int brightness = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_BRIGHTNESS\n");

			brightness = (unsigned int)Scaler_GetBrightness();
			if (copy_to_user((void __user *)arg, (void *)&brightness, sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_BRIGHTNESS fail\n");
				ret = -1;
			} else
				ret = 0;

		}
		break;
		case VPQ_EXTERN_IOC_SET_BRIGHTNESS:
		{
			unsigned int args = 0;

			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_BRIGHTNESS\n");
			if (copy_from_user(&args,  (int __user *)arg,sizeof(unsigned int))) {
				ret = -1;
			} else {
				ret = 0;
				Scaler_SetBrightness((unsigned char)args);
			}
		}
		break;
		case VPQ_EXTERN_IOC_GET_CONTRAST:
		{
			unsigned int contrast = 0;

			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_CONTRAST\n");
			contrast = (unsigned int)Scaler_GetContrast();
			if (copy_to_user((void __user *)arg, (void *)&contrast, sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_CONTRAST fail\n");
				ret = -1;
			} else
				ret = 0;
		}
		break;
		case VPQ_EXTERN_IOC_SET_CONTRAST:
		{
			unsigned int args = 0;

			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_CONTRAST\n");
			if (copy_from_user(&args,  (int __user *)arg,sizeof(unsigned int))) {

				ret = -1;
			} else {
				ret = 0;
				Scaler_SetContrast((unsigned char)args);
			}
		}
		break;
		case VPQ_EXTERN_IOC_GET_SATURATION:
		{
			unsigned int saturation = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_SATURATION\n");
			saturation = (unsigned int)Scaler_GetSaturation();

			if (copy_to_user((void __user *)arg, (void *)&saturation, sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_SATURATION fail\n");
				ret = -1;
			} else
				ret = 0;
		}
		break;
		case VPQ_EXTERN_IOC_SET_SATURATION:
		{
			unsigned int args = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_SATURATION\n");
			if (copy_from_user(&args,  (int __user *)arg,sizeof(unsigned int))) {

				ret = -1;
			} else {
				ret = 0;
				Scaler_SetSaturation((unsigned char)args);
			}
		}
		break;
		case VPQ_EXTERN_IOC_GET_HUE:
		{
			unsigned int hue = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_HUE\n");
			hue = (unsigned int)Scaler_GetHue();

			if (copy_to_user((void __user *)arg, (void *)&hue, sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_HUE fail\n");
				ret = -1;
			} else
				ret = 0;
		}
		break;

		case VPQ_EXTERN_IOC_SET_HUE:
		{
			unsigned int args = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_HUE\n");

			if (copy_from_user(&args,  (int __user *)arg,sizeof(unsigned int))) {

				ret = -1;
			} else {
				ret = 0;
				Scaler_SetHue((unsigned char)args);
			}
		}
		break;
		case VPQ_EXTERN_IOC_GET_NR:
		{
			unsigned int nr = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_NR\n");
			nr = (unsigned int)Scaler_GetDNR();

			if (copy_to_user((void __user *)arg, (void *)&nr, sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_NR fail\n");
				ret = -1;
			} else
				ret = 0;
		}
		break;
		case VPQ_EXTERN_IOC_SET_NR:
		{
			unsigned int args = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_NR\n");

			if (copy_from_user(&args,  (int __user *)arg,sizeof(unsigned int))) {

				ret = -1;
			} else {
				ret = 0;
				Scaler_SetDNR((unsigned char)args);
			}
		}
		break;
		case VPQ_EXTERN_IOC_GET_SHARPNESS:
		{
			unsigned int sharpness = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_SHARPNESS\n");

			sharpness = (unsigned int)Scaler_GetSharpness();
			if (copy_to_user((void __user *)arg, (void *)&sharpness, sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_SHARPNESS fail\n");
				ret = -1;
			} else
				ret = 0;
		}
		break;
		case VPQ_EXTERN_IOC_SET_SHARPNESS_EXTERN:
		{
			unsigned int args = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_SHARPNESS_EXTERN\n");

			if (copy_from_user(&args,  (int __user *)arg,sizeof(unsigned int))) {

				ret = -1;
			} else {
				ret = 0;
				if(is_QS_pq_enable()==1){
					//Scaler_SetSharpness((unsigned char)args);//pq_quick_show workaround , please fixed me

				}else{
					Scaler_SetSharpness((unsigned char)args);
				}
			}
		}
		break;
		case VPQ_EXTERN_IOC_GET_DE_BLOCKING:
		{
			TV001_LEVEL_E dbLevel = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_DE_BLOCKING\n");


			dbLevel = (TV001_LEVEL_E)Scaler_GetMPEGNR();
			if (copy_to_user((void __user *)arg, (void *)&dbLevel, sizeof(TV001_LEVEL_E))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_DE_BLOCKING fail\n");
				ret = -1;
			} else
				ret = 0;
		}
		break;
		case VPQ_EXTERN_IOC_SET_DE_BLOCKING:
		{
			unsigned int args = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_DE_BLOCKING\n");

			if (copy_from_user(&args,  (int __user *)arg,sizeof(unsigned int))) {

				ret = -1;
			} else {
				ret = 0;
				Scaler_SetMPEGNR((unsigned char)args,0);
				Scaler_Set_I_De_Contour(Scaler_Get_I_De_Contour());
			}
		}
		break;
		case VPQ_EXTERN_IOC_GET_COLOR_TEMP:
		{
			TV001_COLORTEMP_E colorTemp = 0;
			TV001_COLOR_TEMP_DATA_S args = {0};
			if(get_mach_type() == MACH_ARCH_RTK2851F) {
				rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_COLOR_TEMP, center value is {2048,2048,2048,0,0,0}\n");
				// AP will sent {512,512,512,512,512,512} tp kernel if TV001_COLOR_TEMP_DATA_S is not {2048,2048,2048,0,0,0}
				args.redGain=2048;
				args.greenGain=2048;
				args.blueGain=2048;
				args.redOffset=0;
				args.greenOffset=0;
				args.blueOffset=0;
				args.gamma_curve_index = 0;
				if (copy_to_user((void __user *)arg, (void *)&args, sizeof(TV001_COLOR_TEMP_DATA_S))) {
					rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_COLOR_TEMP fail\n");
					ret = -1;
				} else {
					ret = 0;
				}

			} else {
				rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_COLOR_TEMP\n");
				if (Scaler_GetColorTemp_level_type(&colorTemp) == FALSE) {
					rtd_pr_vpq_err("kernel Scaler_GetColorTemp_level_type fail\n");
					ret = -1;
				} else {
					if (copy_to_user((void __user *)arg, (void *)&colorTemp, sizeof(TV001_COLORTEMP_E))) {
						rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_COLOR_TEMP fail\n");
						ret = -1;
					} else
						ret = 0;
				}
			}
		}
		break;
		case VPQ_EXTERN_IOC_SET_COLOR_TEMP_EXTERN:
		{
			TV001_COLOR_TEMP_DATA_S args;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_COLOR_TEMP_EXTERN\n");

			if (copy_from_user(&args,  (int __user *)arg,sizeof(TV001_COLOR_TEMP_DATA_S))) {

				ret = -1;
			} else {
				ret = 0;
				rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_COLOR_TEMP_EXTERN, outgamma table index = %d\n", args.gamma_curve_index);

				Scaler_set_APDEM(DEM_ARG_Gamma_idx_set, &args.gamma_curve_index);
				
				if(is_QS_pq_enable()==1){
				}else{					
					Scaler_SetColorTempData((TV001_COLOR_TEMP_DATA_S*) &args); 
                }
				#if defined(CONFIG_RTK_AI_DRV)
				memcpy(&vpqex_color_temp, &args, sizeof(TV001_COLOR_TEMP_DATA_S));
				#endif
			}
		}
		break;

		case VPQ_EXTERN_IOC_GET_BACKLIGHT:
		{
			unsigned int backlight = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_BACKLIGHT\n");
			backlight = (unsigned int)Scaler_GetBacklight();

			if (copy_to_user((void __user *)arg, (void *)&backlight, sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_BACKLIGHT fail\n");
				ret = -1;
			} else
				ret = 0;
		}
		break;
		case VPQ_EXTERN_IOC_SET_BACKLIGHT:
		{
			unsigned int args = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_BACKLIGHT\n");
			if (copy_from_user(&args,  (int __user *)arg,sizeof(unsigned int))) {

				ret = -1;
			} else {
				ret = 0;
				Scaler_SetBackLight((unsigned char)args);
			}
		}
		break;
		case VPQ_EXTERN_IOC_SET_PANEL_BL:
		{
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_PANEL_BL\n");
		}
		break;
		case VPQ_EXTERN_IOC_SET_GAMMA:
		{
			unsigned int args = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_GAMMA\n");
			if (copy_from_user(&args,  (int __user *)arg,sizeof(unsigned int))) {

				ret = -1;
			} else {
				ret = 0;
				if(is_QS_pq_enable()==1){
					//Scaler_SetGamma((unsigned char)args); //pq_quick_show workaround , please fixed me
				}else{
					Scaler_SetGamma((unsigned char)args);
				}
			}
		}
		break;
		case VPQ_EXTERN_IOC_GET_PQ_MODULE:
		{
			RTK_PQModule_T PQModule_T;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_PQ_MODULE\n");

			if (copy_from_user(&PQModule_T,  (int __user *)arg,sizeof(RTK_PQModule_T))) {

				ret = -1;
			} else {
				ret = 0;
				if (Scaler_GetPQModule((TV001_PQ_MODULE_E) PQModule_T.PQModule,(unsigned char *) &PQModule_T.onOff) == FALSE) {
					rtd_pr_vpq_err("kernel Scaler_GetPQModule fail\n");
					ret = -1;
				} else {
					if (copy_to_user((void __user *)arg, (void *)&PQModule_T, sizeof(RTK_PQModule_T))) {
						rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_PQ_MODULE fail\n");
						ret = -1;
					} else
						ret = 0;
				}
			}
		}
		break;
		case VPQ_EXTERN_IOC_SET_PQ_MODULE:
		{
			RTK_PQModule_T PQModule_T;
			rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_SET_PQ_MODULE\n");
			//unsigned int args = 0;

			if (copy_from_user(&PQModule_T,  (int __user *)arg,sizeof(RTK_PQModule_T))) {

				ret = -1;
			} else {
				ret = 0;
                if(PQModule_T.PQModule==TV001_PQ_MODULE_444MODE ||PQModule_T.PQModule==TV001_PQ_MODULE_RGB444MODE){
                    if (Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_HDMI){
                        if ((drvif_Hdmi_GetColorSpace() == COLOR_RGB && !is_dolby_vision_tunnel_mode()) && !Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_INTERLACE)){
                                Scaler_SetPQModule(TV001_PQ_MODULE_RGB444MODE,0);
                        }else{
                                Scaler_SetPQModule(TV001_PQ_MODULE_444MODE,0);
                        }
                    }else if (Get_DisplayMode_Src(SLR_MAIN_DISPLAY) == VSC_INPUTSRC_DP){  
                    #if IS_ENABLED(CONFIG_RTK_DPRX)
                        if ((drvif_Dprx_GetColorSpace() == DP_COLOR_SPACE_RGB && !is_dolby_vision_tunnel_mode()) && !Scaler_DispGetStatus(SLR_MAIN_DISPLAY, SLR_DISP_INTERLACE)){
                                Scaler_SetPQModule(TV001_PQ_MODULE_RGB444MODE,0);
                        }else{
                                Scaler_SetPQModule(TV001_PQ_MODULE_444MODE,0);
                        }
                    #endif
                    }               
                }else
                    Scaler_SetPQModule((TV001_PQ_MODULE_E) PQModule_T.PQModule,(unsigned char)PQModule_T.onOff);
			}
		}
		break;
		case VPQ_EXTERN_IOC_SET_DEMO:
		{
			RTK_DEMO_MODE_T DEMO_MODE_T;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_DEMO\n");
			//unsigned int args = 0;

			if (copy_from_user(&DEMO_MODE_T,  (int __user *)arg,sizeof(RTK_DEMO_MODE_T))) {

				ret = -1;
			} else {
				ret = 0;
				Scaler_SetDemoMode((TV001_DEMO_MODE_E) DEMO_MODE_T.DEMO_MODE,(unsigned char)DEMO_MODE_T.onOff);
			}
		}
		break;
		case VPQ_EXTERN_IOC_GET_DEBUG_MODE:
		{
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_DEBUG_MODE\n");
		}
		break;
		case VPQ_EXTERN_IOC_SET_DEBUG_MODE:
		{
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_DEBUG_MODE\n");
		}
		break;
		case VPQ_EXTERN_IOC_GET_HISTOGRAM:
		{
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_HISTOGRAM\n");


			if (fwif_color_get_HistoData_Countbins(TV006_VPQ_chrm_bin, &(HistoData_bin[0])) == FALSE) {
				rtd_pr_vpq_err("kernel VPQ_IOC_GET_HistoData_Countbins fail\n");
				ret = -1;
			} else {
				if (copy_to_user((void __user *)arg, (void *)&HistoData_bin[0], sizeof(HistoData_bin))) {
					rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_HISTOGRAM fail\n");
					ret = -1;
				} else
					ret = 0;
			}

		}
		break;
		case VPQ_EXTERN_IOC_GET_HUE_HISTOGRAM:
		{
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_HUE_HISTOGRAM\n");

			if (fwif_color_get_HistoData_huebins(COLOR_HUE_HISTOGRAM_LEVEL, &(HistoData_Hue_bin[0])) == FALSE) {
				rtd_pr_vpq_err("kernel fwif_color_get_HistoData_huebins fail\n");
				ret = -1;
			} else {
				if (copy_to_user((void __user *)arg, (void *)&HistoData_Hue_bin[0], sizeof(HistoData_Hue_bin))) {
					rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_HUE_HISTOGRAM fail\n");
					ret = -1;
				} else
					ret = 0;
			}

		}
		break;
		case VPQ_EXTERN_IOC_GET_SAT_HISTOGRAM:
		{
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_SAT_HISTOGRAM\n");


			if (fwif_color_get_HistoData_chrmbins(COLOR_AutoSat_HISTOGRAM_LEVEL, &(HistoData_Sat_bin[0])) == FALSE) {
				rtd_pr_vpq_err("kernel fwif_color_get_HistoData_chrmbins fail\n");
				ret = -1;
			} else {
				if (copy_to_user((void __user *)arg, (void *)&HistoData_Sat_bin[0], sizeof(HistoData_Sat_bin))) {
					rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_SAT_HISTOGRAM fail\n");
					ret = -1;
				} else
					ret = 0;
			}

		}
		break;
		case VPQ_EXTERN_IOC_SET_BLACK_PATTERN_OUTPUT:
		{
			unsigned int args = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_BLACK_PATTERN_OUTPUT\n");
			if (copy_from_user(&args,  (int __user *)arg,sizeof(unsigned int))) {

				ret = -1;
			} else {
				ret = 0;
				if (args > 4)
					args = 0;
				Scaler_SetBlackPatternOutput((unsigned char)args);
			}
		}
		break;
		case VPQ_EXTERN_IOC_SET_WHITE_PATTERN_OUTPUT:
		{
			unsigned int args = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_WHITE_PATTERN_OUTPUT\n");
			if (copy_from_user(&args,  (int __user *)arg,sizeof(unsigned int))) {

				ret = -1;
			} else {
				ret = 0;
				if (args > 4)
					args = 0;
				Scaler_SetWhitePatternOutput((unsigned char)args);
			}
		}
		break;
		case VPQ_EXTERN_IOC_GET_SRC_HDR_INFO:
		{
			unsigned int GammaType = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_SRC_HDR_INFO\n");

			if (Scaler_GetSrcHdrInfo(&GammaType) == FALSE) {
				rtd_pr_vpq_err("kernel Scaler_GetSrcHdrInfo fail\n");
				ret = -1;
			} else {
				if (copy_to_user((void __user *)arg, (void *)&GammaType, sizeof(unsigned int))) {
					rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_SRC_HDR_INFO fail\n");
					ret = -1;
				} else
					ret = 0;
			}
		}
		break;
		case VPQ_EXTERN_IOC_GET_SDR2HDR:
		{
			unsigned char onoff = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_SDR2HDR\n");

			if (Scaler_GetSDR2HDR(&onoff) == FALSE) {
				rtd_pr_vpq_err("kernel Scaler_GetSDR2HDR fail\n");
				ret = -1;
			} else {
				if (copy_to_user((void __user *)arg, (void *)&onoff, sizeof(unsigned char))) {
					rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_SDR2HDR fail\n");
					ret = -1;
				} else
					ret = 0;
			}
		}
		break;
		case VPQ_EXTERN_IOC_SET_SDR2HDR:
		{
			unsigned int args = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_SDR2HDR\n");

			if (copy_from_user(&args,  (int __user *)arg,sizeof(unsigned int))) {

				ret = -1;
			} else {
				ret = 0;
				Scaler_SetSDR2HDR((unsigned char)args);
			}
		}
		break;
		case VPQ_EXTERN_IOC_GET_HDR_TYPE:
		{
			//rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_HDR_TYPE\n");
			TV001_HDR_TYPE_E HdrType = 0;

			if (Scaler_GetHdrType(&HdrType) == FALSE) {
				rtd_pr_vpq_err("kernel Scaler_GetHdrType fail\n");
				ret = -1;
			} else {
				if (copy_to_user((void __user *)arg, (void *)&HdrType, sizeof(unsigned int))) {
					rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_HDR_TYPE fail\n");
					ret = -1;
				} else
					ret = 0;
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_DCI_TYPE:
		{
			unsigned int args = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_DCI_TYPE\n");

			if (copy_from_user(&args,  (int __user *)arg,sizeof(unsigned int))) {

				ret = -1;
			} else {
				ret = 0;
				Scaler_SetDCC_Table((unsigned char)args);
			}
		}
		break;
		case VPQ_EXTERN_IOC_GET_DOLBY_HDR_PIC_MODE:
		{
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_DOLBY_HDR_PIC_MODE\n");
		}
		break;
		case VPQ_EXTERN_IOC_SET_DOLBY_HDR_PIC_MODE:
		{
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_DOLBY_HDR_PIC_MODE\n");
		}
		break;


		case VPQ_EXTERN_IOC_GET_HDR10_ENABLE:
		{
			unsigned char bEnable = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_HDR10_ENABLE\n");

			if (Scaler_GetHdr10Enable(&bEnable) == FALSE) {
				rtd_pr_vpq_err("kernel Scaler_GetHdr10Enable fail\n");
				ret = -1;
			} else {
				if (copy_to_user((void __user *)arg, (void *)&bEnable, sizeof(unsigned char))) {
					rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_HDR10_ENABLE fail\n");
					ret = -1;
				} else
					ret = 0;
			}
		}
		break;
		case VPQ_EXTERN_IOC_SET_HDR10_ENABLE:
		{
			unsigned int args = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_HDR10_ENABLE\n");
			if (copy_from_user(&args,  (int __user *)arg,sizeof(unsigned int))) {

				ret = -1;
			} else {
				ret = 0;
				Scaler_SetHdr10Enable((unsigned char)args);
			}
		}
		break;

		case VPQ_EXTERN_IOC_GET_DOLBYHDR_ENABLE:
		{
			unsigned char bEnable = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_HDR10_ENABLE\n");

			if (Scaler_GetDOLBYHDREnable(&bEnable) == FALSE) {
				rtd_pr_vpq_err("kernel Scaler_GetDOLBYHDREnable fail\n");
				ret = -1;
			} else {
				if (copy_to_user((void __user *)arg, (void *)&bEnable, sizeof(unsigned char))) {
					rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_DOLBYHDR_ENABLE fail\n");
					ret = -1;
				} else
					ret = 0;
			}
		}
		break;
		case VPQ_EXTERN_IOC_SET_DOLBYHDR_ENABLE:
		{
			unsigned int args = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_DOLBYHDR_ENABLE\n");
			if (copy_from_user(&args,  (int __user *)arg,sizeof(unsigned int))) {

				ret = -1;
			} else {
				ret = 0;
				Scaler_SetDOLBYHDREnable((unsigned char)args);
			}
		}
		break;


		case VPQ_EXTERN_IOC_SET_DREAM_PANEL_LOG_ENABLE:
		{
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_DREAM_PANEL_LOG_ENABLE\n");
		}
		break;
		case VPQ_EXTERN_IOC_GET_DREAM_PANEL_BACKLIGHT:
		{
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_DREAM_PANEL_BACKLIGHT\n");
		}
		break;
		case VPQ_EXTERN_IOC_SET_DREAM_PANEL_BACKLIGHT:
		{
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_DREAM_PANEL_BACKLIGHT\n");
		}
		break;
		case VPQ_EXTERN_IOC_GET_DCC_MODE:
		{
			unsigned int dcc_mode = 0;
			dcc_mode = (unsigned int)Scaler_GetDCC_Mode();
			if (copy_to_user((void __user *)arg, (void *)&dcc_mode, sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_DCC_MODE fail\n");
				ret = -1;
			} else
				ret = 0;
		}
		break;
		case VPQ_EXTERN_IOC_SET_DCC_MODE:
		{
			unsigned int dcc_mode = 0;

			if (copy_from_user(&dcc_mode,  (int __user *)arg,sizeof(unsigned int))) {

				ret = -1;
			} else {
				ret = 0;
				
				rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_DCC_MODE \n");
				Scaler_SetDCC_Mode(dcc_mode&0xff);
			}
		}
		break;
		case VPQ_EXTERN_IOC_GET_ICM_TBL_IDX:
		{
			unsigned int icm_table_idx = 0;
			icm_table_idx = Scaler_GetICMTable();
			if (copy_to_user((void __user *)arg, (void *)&icm_table_idx, sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_ICM_TBL_IDX fail\n");
				ret = -1;
			} else
				ret = 0;

		}
		break;
		case VPQ_EXTERN_IOC_SET_ICM_TBL_IDX:
		{
			unsigned int icm_table_idx = 0;

			if (copy_from_user(&icm_table_idx,  (int __user *)arg,sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_ICM_TBL_IDX fail\n");
				ret = -1;
			} else {
				rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_ICM_TBL_IDX \n");
				Scaler_SetICMTable(icm_table_idx&0xff);
				ret = 0;
			}
		}
		break;
		case VPQ_EXTERN_IOC_GET_FILM_MODE:
		{
			unsigned int film_mode_enable;
			film_mode_enable = film_mode.film_status?1:0;

			if (copy_to_user((void __user *)arg, (void *)&film_mode_enable, sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_FILM_MODE fail\n");
				ret = -1;
			} else
				ret = 0;

		}
		break;
		case VPQ_EXTERN_IOC_SET_FILM_MODE:
		{
			unsigned int film_mode_enable = 0;

			if (copy_from_user(&film_mode_enable,  (int __user *)arg,sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_FILM_MODE fail\n");
				ret = -1;
			} else {
				rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_FILM_MODE \n");
		
				film_mode.film_status = film_mode_enable?28:0;
				down(get_DI_semaphore());
				drvif_module_film_mode((DRV_film_mode *) &film_mode);
				up(get_DI_semaphore());
				ret = 0;
			}
		}
		break;
		case VPQ_EXTERN_IOC_GET_HDMI_CSC_DATA_RANGE_MODE:
		{
			HDMI_CSC_DATA_RANGE_MODE_T range_mode = Scaler_GetHDMI_CSC_DataRange_Mode();

			if (copy_to_user((void __user *)arg, (void *)&range_mode, sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_HDMI_CSC_DATA_RANGE_MODE fail\n");
				ret = -1;
			} else
				ret = 0;
		}
		break;
		case VPQ_EXTERN_IOC_SET_HDMI_CSC_DATA_RANGE_MODE:
		{
			unsigned int range_mode = 0;

			if (copy_from_user(&range_mode,  (int __user *)arg,sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_FILM_MODE fail\n");
				ret = -1;
			} else {
				rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_HDMI_CSC_DATA_RANGE_MODE \n");
				Scaler_SetHDMI_CSC_DataRange_Mode((HDMI_CSC_DATA_RANGE_MODE_T)range_mode);
				Scaler_SetDataFormatHandler();
				ret = 0;
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_Local_Contrast_byIndex:
		{			
			unsigned int DEM_idx = 0;
			if (copy_from_user(&DEM_idx,  (int __user *)arg,sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_Local_Contrast_byIndex fail\n");
				ret = -1;
			} else {
				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_Local_Contrast_byIndex %d,\n", DEM_idx);
				ret = Scaler_set_APDEM(DEM_ARG_LC_Idx, (void *)&DEM_idx);
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_DCC_byIndex:
		{			
			unsigned int DEM_idx = 0;
			if (copy_from_user(&DEM_idx,  (int __user *)arg,sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_DCC_byIndex fail\n");
				ret = -1;
			} else {
				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_DCC_byIndex %d,\n", DEM_idx);
				ret = Scaler_set_APDEM(DEM_ARG_DCC_Idx, (void *)&DEM_idx);
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_ICM_byIndex:
		{
			unsigned int DEM_idx = 0;
			if (copy_from_user(&DEM_idx,  (int __user *)arg,sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_ICM_byIndex fail\n");
				ret = -1;
			} else {
				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_ICM_byIndex %d,\n", DEM_idx);
				ret = Scaler_set_APDEM(DEM_ARG_ICM_Idx, (void *)&DEM_idx);
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_Local_Contrast_byLevel:
		{
			unsigned int DEM_Lv = 0;
			if (copy_from_user(&DEM_Lv,  (int __user *)arg,sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_Local_Contrast_byLevel fail\n");
				ret = -1;
			} else {
				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_Local_Contrast_byLevel %d,\n", DEM_Lv);
				ret = Scaler_set_APDEM(DEM_ARG_LC_level, (void *)&DEM_Lv);
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_After_Filter:
		{
			unsigned int DEM_Val = 0;
			unsigned char DEM_Val8 = 0;
			if (copy_from_user(&DEM_Val8,  (int __user *)arg,sizeof(char))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_After_Filter fail\n");
				ret = -1;
			} else {
				DEM_Val = DEM_Val8;
				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_After_Filter %d,\n", DEM_Val);
				ret = Scaler_set_APDEM(DEM_ARG_After_Filter_Val, (void *)&DEM_Val);
			}		
		}
		break;

		case VPQ_EXTERN_IOC_SET_After_Filter_En:
		{
			unsigned int DEM_en = 0;			
			unsigned char DEM_en8 = 0;
			if (copy_from_user(&DEM_en8,  (int __user *)arg,sizeof(char))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_After_Filter_En fail\n");
				ret = -1;
			} else {
				DEM_en = DEM_en8;
				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_After_Filter_En %d,\n", DEM_en);
				ret = Scaler_set_APDEM(DEM_ARG_After_Filter_En, (void *)&DEM_en);
			}		
		}
		break;

		case VPQ_EXTERN_IOC_SET_Blue_Stretch:
		{
			unsigned int DEM_lv = 0;
			if (copy_from_user(&DEM_lv,  (int __user *)arg,sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_Blue_Stretch fail\n");
				ret = -1;
			} else {
				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_Blue_Stretch %d,\n", DEM_lv);
				ret = Scaler_set_APDEM(DEM_ARG_Blue_Stretch_level, (void *)&DEM_lv);
			}		
		}
		break;

		case VPQ_EXTERN_IOC_SET_Black_Extension_Level:
		{
			unsigned int DEM_lv = 0;
			unsigned char DEM_lv8 = 0;
			if (copy_from_user(&DEM_lv8,  (int __user *)arg,sizeof(unsigned char))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_Black_Extension_Level fail\n");
				ret = -1;
			} else {
				DEM_lv = DEM_lv8;
				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_Black_Extension_Level %d,\n", DEM_lv);
				ret = Scaler_set_APDEM(DEM_ARG_Black_Extension_level, (void *)&DEM_lv);
			}		
		}
		break;

		case VPQ_EXTERN_IOC_SET_White_Extension_Level:
		{
			unsigned int DEM_lv = 0;
			unsigned char DEM_lv8 = 0;
			if (copy_from_user(&DEM_lv8,  (int __user *)arg,sizeof(char))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_White_Extension_Level fail\n");
				ret = -1;
			} else {
				DEM_lv = DEM_lv8;
				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_White_Extension_Level %d,\n", DEM_lv);
				ret = Scaler_set_APDEM(DEM_ARG_White_Extension_level, (void *)&DEM_lv);
			}		
		}
		break;

		case VPQ_EXTERN_IOC_SET_Data_Range:
		{
			unsigned int DEM_arg = 0;
			unsigned char DEM_arg8 = 0;
			if (copy_from_user(&DEM_arg8,  (int __user *)arg,sizeof(char))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_Data_Range fail\n");
				ret = -1;
			} else {
				DEM_arg = DEM_arg8;
				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_Data_Range %d,\n", DEM_arg);
				ret = Scaler_set_APDEM(DEM_ARG_Data_Range, (void *)&DEM_arg);
			}		
		}
		break;

		case VPQ_EXTERN_IOC_SET_Gamma_byOffset:
		{
			unsigned int DEM_arg = 0;
			if (copy_from_user(&DEM_arg,  (int __user *)arg,sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_Gamma_byOffset fail\n");
				ret = -1;
			} else {
				DEM_arg = DEM_arg + Gamma_exp_2p2;	// Gamma_exp_2p2 is center
				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_Gamma_byOffset %d,\n", DEM_arg);
				ret = Scaler_set_APDEM(DEM_ARG_Gamma_exp_byOffset, (void *)&DEM_arg);
			}		
		}
		break;

		case VPQ_EXTERN_IOC_SET_10p_Gamma_Offset:
		{
			unsigned int *DEM_arg;
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_10p_Gamma_Offset hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} else {
				if (hal_VPQ_DATA_EXTERN.length != VIP_DEM_10p_Gamma_Offset_Num * 3 * sizeof(int)) {
					rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_10p_Gamma_Offset len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
					ret = -1;
				} else {
					Data_addr = hal_VPQ_DATA_EXTERN.pData;
					DEM_arg = (unsigned int *)dvr_malloc_specific(hal_VPQ_DATA_EXTERN.length, GFP_DCU2_FIRST);
					if (DEM_arg == NULL) {
						rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_10p_Gamma_Offset alloc fail, len=%d,\n", hal_VPQ_DATA_EXTERN.length);
						ret = -1;
					} else {		
						if(copy_from_user(DEM_arg, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
						{
							rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_10p_Gamma_Offset table copy fail\n");
							ret = -1;
						} else {
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_10p_Gamma_Offset %d,%d,%d,%d,%d,%d,%d,%d,%d,\n", 
								DEM_arg[0], DEM_arg[1], DEM_arg[2], DEM_arg[12], DEM_arg[13], DEM_arg[14], DEM_arg[27], DEM_arg[28], DEM_arg[29]);
							ret = Scaler_set_APDEM(DEM_ARG_10p_Gamma_Offset, (void *)DEM_arg);
						}
						dvr_free((void *)DEM_arg);
					}
				}
			}	
		}		
		break;

		case VPQ_EXTERN_IOC_SET_Color_Space:
		{
			unsigned int DEM_arg;
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_Color_Space hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} else {
				if (hal_VPQ_DATA_EXTERN.length != sizeof(int)) {
					rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_Color_Space len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
					ret = -1;
				} else {
					Data_addr = hal_VPQ_DATA_EXTERN.pData;
					if(copy_from_user(&DEM_arg, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
					{
						rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_Color_Space table copy fail\n");
						ret = -1;
					} else {
						rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_Color_Space %d,\n", DEM_arg);
						ret = Scaler_set_APDEM(DEM_ARG_Color_Space, (void *)&DEM_arg);
					}
				}
			}	
		}		
		break;

		case VPQ_EXTERN_IOC_SET_Local_Dimming_byLevel:
		{
			unsigned int DEM_arg = 0;
			if (copy_from_user(&DEM_arg,  (int __user *)arg,sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_Local_Dimming_byLevel fail\n");
				ret = -1;
			} else {
				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_Local_Dimming_byLevel %d,\n", DEM_arg);
				//ret = Scaler_set_APDEM(DEM_ARG_Gamma_exp_byOffset, (void *)&DEM_arg);
			}		
		}
		break;

		case VPQ_EXTERN_IOC_SET_D_3DLUT_Offset:
		{
			unsigned int DEM_arg[VIP_DEM_3D_LUT_Offset_Num] = {0};	// R/G/B/Y/C/M 6 axis=>RTK_VPQ_COLORSPACE_INDEX  
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_D_3DLUT_Offset hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} 
			else 
			{				
				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset,version=%d,cmd=%d,length=%d,pData=%x,\n",
					hal_VPQ_DATA_EXTERN.version, hal_VPQ_DATA_EXTERN.wId, hal_VPQ_DATA_EXTERN.length, hal_VPQ_DATA_EXTERN.pData);				

				Data_addr = hal_VPQ_DATA_EXTERN.pData;
				if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Set)
				{
					if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_SET_D_3DLUT_Offset_Set_6Axis)
					{
						if (hal_VPQ_DATA_EXTERN.length != VIP_DEM_3D_LUT_Offset_Num * sizeof(int)) {
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						} else {
							if(copy_from_user(&DEM_arg[0], (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
							{
								rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_D_3DLUT_Offset table copy fail\n");
								ret = -1;
							} else {
								rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset %d,%d,%d,\n", DEM_arg[0], DEM_arg[5], DEM_arg[9]);
								ret = Scaler_set_APDEM(DEM_ARG_D_3DLUT_Offset, (void *)DEM_arg);
							}
						}
					}
					else if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_SET_D_3DLUT_Offset_D3DLUTbyCS_6Axis_TBL)
					{
						if (hal_VPQ_DATA_EXTERN.length != sizeof(VIP_D3DLUTbyCS_6axis)) {
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						} else {
							// alloc and init for fac/tmp buff
							if (gD3DLUTbyCS_6axis_tmp == NULL)
							{
								gD3DLUTbyCS_6axis_tmp = dvr_malloc_specific(sizeof(VIP_D3DLUTbyCS_6axis), GFP_DCU2_FIRST);
								if (gD3DLUTbyCS_6axis_tmp == NULL)
								{
									rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset ini/alloc gD3DLUTbyCS_6axis_tmp fail\n");
									ret = -1;	
								}
								else
								{									
									memset(gD3DLUTbyCS_6axis_tmp, 0, sizeof(VIP_D3DLUTbyCS_6axis));
									//gD3DLUTbyCS_6axis_tmp.index = ii;
									rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset ini/alloc gD3DLUTbyCS_6axis_tmp done\n");
								}
							}
						
							if (gD3DLUTbyCS_6axis_tmp == NULL)
							{
								rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset alloc gD3DLUTbyCS_6axis_tmp fail\n");
								ret = -1;	
							}
							else
							{
								if(copy_from_user(gD3DLUTbyCS_6axis_tmp, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
								{
									rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_SET_D_3DLUT_Offset table copy fail\n");
									ret = -1;
								} else {
									idx = gD3DLUTbyCS_6axis_tmp->index;
									pBuff_tmp = (unsigned char *)gD3DLUTbyCS_6axis_tmp;
									if (idx >= VIP_DEM_Color_Space_Apply_MAX)
									{
										rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_SET_D_3DLUT_Offset, idx error, idx = %d,\n", idx);
										ret = -1;
									}
									else if (idx == Color_Space_D3DLUTbyCS_Last_index)	// set from factory
									{								
										Scaler_Set_ColorSpace_D_3dlut_APDEM(VIP_D3DLUTbyCS_setByFactory6Axis);
										rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset set from fac, idx=%d, size=%d,\n", idx, sizeof(VIP_D3DLUTbyCS_6axis));
										ret = 0;	//Scaler_set_APDEM(DEM_ARG_D_3DLUT_Offset, (void *)DEM_arg);
									}
									else
									{
										// alloc and init
										if (gD3DLUTbyCS_6axis_TBL == NULL)
										{
											gD3DLUTbyCS_6axis_TBL = dvr_malloc_specific(sizeof(VIP_D3DLUTbyCS_6axis_TBL), GFP_DCU2_FIRST);
											if (gD3DLUTbyCS_6axis_TBL == NULL)
											{
												rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset ini/alloc gD3DLUTbyCS_6axis_TBL fail\n");
												ret = -1;	
											}
											else
											{									
												memset(gD3DLUTbyCS_6axis_TBL, 0, sizeof(VIP_D3DLUTbyCS_6axis_TBL));
												for (ii=0;ii<VIP_DEM_Color_Space_Apply_MAX;ii++)
												{
													gD3DLUTbyCS_6axis_TBL->D3DLUTbyCS_6axis[ii].index = ii;
												}
												rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset ini/alloc gD3DLUTbyCS_6axis_TBL done\n");
											}
										}

										if (gD3DLUTbyCS_6axis_TBL == NULL)
										{
											rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset alloc gD3DLUTbyCS_6axis_TBL fail\n");
											ret = -1;	
										}
										else
										{
											memcpy(&gD3DLUTbyCS_6axis_TBL->D3DLUTbyCS_6axis[idx], pBuff_tmp, hal_VPQ_DATA_EXTERN.length);
											rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset set to table gD3DLUTbyCS_6axis_TBL, idx=%d, siez=%d,\n", idx, sizeof(VIP_D3DLUTbyCS_6axis));
											ret = 0;	//Scaler_set_APDEM(DEM_ARG_D_3DLUT_Offset, (void *)DEM_arg);
										}
									}
								}
							}
						}
					}
					else if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_SET_D_3DLUT_Offset_D3DLUTbyCS_Decode_TBL)
					{
						if (hal_VPQ_DATA_EXTERN.length != sizeof(GAMUT_3D_LUT_17x17x17_T)) {
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						} else {
							// alloc and init for fac/tmp
							if (gD3DLUTbyCS_Decode_TBL_tmp == NULL)
							{
								gD3DLUTbyCS_Decode_TBL_tmp = dvr_malloc_specific(sizeof(GAMUT_3D_LUT_17x17x17_T), GFP_DCU2_FIRST);
								if (gD3DLUTbyCS_Decode_TBL_tmp == NULL)
								{
									rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset ini/alloc gD3DLUTbyCS_Decode_TBL_tmp fail\n");
									ret = -1;	
								}
								else
								{									
									rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset ini/alloc gD3DLUTbyCS_Decode_TBL_tmp done\n");
								}
							}
														
							if (gD3DLUTbyCS_Decode_TBL_tmp == NULL)
							{
								rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset alloc gD3DLUTbyCS_Decode_TBL_tmp fail\n");
								ret = -1;	
							}
							else
							{
								if(copy_from_user(gD3DLUTbyCS_Decode_TBL_tmp, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
								{
									rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_SET_D_3DLUT_Offset table copy fail\n");
									ret = -1;
								} else {
									idx = gD3DLUTbyCS_Decode_TBL_tmp->index;
									pBuff_tmp = (unsigned char *)gD3DLUTbyCS_Decode_TBL_tmp;
									if (idx >= VIP_DEM_Color_Space_Apply_MAX)
									{
										rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_SET_D_3DLUT_Offset, idx error, idx = %d,\n", idx);
										ret = -1;
									}
									else if (idx == Color_Space_D3DLUTbyCS_Last_index)
									{
										Scaler_Set_ColorSpace_D_3dlut_APDEM(VIP_D3DLUTbyCS_setByFactoryDecodeTBL);
										rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset factory gD3DLUTbyCS_Decode_TBL_tmp, idx=%d, siez=%d,\n", idx, sizeof(VIP_D3DLUTbyCS_6axis));
										ret = 0;	//Scaler_set_APDEM(DEM_ARG_D_3DLUT_Offset, (void *)DEM_arg);
									}
									else
									{
										if (gD3DLUTbyCS_Decode_TBL == NULL)
										{
											gD3DLUTbyCS_Decode_TBL = dvr_malloc_specific(sizeof(VIP_D3DLUTbyCS_Decode_TBL), GFP_DCU2_FIRST);
											if (gD3DLUTbyCS_Decode_TBL == NULL)
											{
												rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset ini/alloc gD3DLUTbyCS_Decode_TBL fail\n");
												ret = -1;	
											}
											else
											{									
												for (i=0;i<VIP_D_3D_LUT_INDEX;i++)
												{		
													addrB = i*VIP_D_3D_LUT_INDEX*VIP_D_3D_LUT_INDEX;
													B_val = ((i * 4096)>65535)?(65535):(i * 4096);
													for (j=0;j<VIP_D_3D_LUT_INDEX;j++)
													{												
														addrG = j*VIP_D_3D_LUT_INDEX;
														G_val = ((j * 4096)>65535)?(65535):(j * 4096);
														for (k=0;k<VIP_D_3D_LUT_INDEX;k++)
														{												
															addrR = k;
															R_val = ((k * 4096)>65535)?(65535):(k * 4096);
															addrRGB = addrB + addrG + addrR;
															// assigned linear value to memory
															for (ii=0;ii<VIP_DEM_Color_Space_Apply_MAX;ii++)
															{
																gD3DLUTbyCS_Decode_TBL->D3DLUTbyCS_Decode_TBL[ii].index = ii;												
																gD3DLUTbyCS_Decode_TBL->D3DLUTbyCS_Decode_TBL[ii].pt[addrRGB].r_data = R_val;
																gD3DLUTbyCS_Decode_TBL->D3DLUTbyCS_Decode_TBL[ii].pt[addrRGB].g_data = G_val;
																gD3DLUTbyCS_Decode_TBL->D3DLUTbyCS_Decode_TBL[ii].pt[addrRGB].b_data = B_val;
															}
														}
													}
												}
												rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset ini/alloc gD3DLUTbyCS_Decode_TBL done\n");
											}
										}							

										if (gD3DLUTbyCS_Decode_TBL == NULL)
										{
											rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset ini/alloc gD3DLUTbyCS_Decode_TBL fail\n");
											ret = -1;	
										}
										else
										{
											memcpy(&gD3DLUTbyCS_Decode_TBL->D3DLUTbyCS_Decode_TBL[idx], pBuff_tmp, hal_VPQ_DATA_EXTERN.length);
											rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset set to gD3DLUTbyCS_Decode_TBL, idx=%d, siez=%d,\n", idx, sizeof(GAMUT_3D_LUT_17x17x17_T));
											ret = 0;	//Scaler_set_APDEM(DEM_ARG_D_3DLUT_Offset, (void *)DEM_arg);
										}
									}
								}
							}
						}
					}
					else
					{
						rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset ver error ver=%d,\n", hal_VPQ_DATA_EXTERN.version);
						ret = -1;	
					}
				}
				else if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Get)
				{
					if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_SET_D_3DLUT_Offset_D3DLUTbyCS_Decode_TBL)
					{
						if (hal_VPQ_DATA_EXTERN.length != sizeof(GAMUT_3D_LUT_17x17x17_T)) {
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						} else {
							pD3D_LUT_17x17x17 = dvr_malloc_specific(sizeof(GAMUT_3D_LUT_17x17x17_T), GFP_DCU2_FIRST);
							if (pD3D_LUT_17x17x17 == NULL)
							{
								rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset alloc D3D_LUT_17x17x17 fail\n");
								ret = -1;	
							}
							else
							{
								for (i=0;i<VIP_D_3D_LUT_INDEX;i++)
								{		
									for (j=0;j<VIP_D_3D_LUT_INDEX;j++)
									{												
										for (k=0;k<VIP_D_3D_LUT_INDEX;k++)
										{	
											addrRGB = i*VIP_D_3D_LUT_INDEX*VIP_D_3D_LUT_INDEX+j*VIP_D_3D_LUT_INDEX+k;
											pD3D_LUT_17x17x17->pt[addrRGB].r_data = D_3DLUT_Reseult[addrRGB][VIP_8VERTEX_R];
											pD3D_LUT_17x17x17->pt[addrRGB].g_data = D_3DLUT_Reseult[addrRGB][VIP_8VERTEX_G];
											pD3D_LUT_17x17x17->pt[addrRGB].b_data = D_3DLUT_Reseult[addrRGB][VIP_8VERTEX_B];
										}
									}
								}
								pD3D_LUT_17x17x17->index = Color_Space_D3DLUTbyCS_Last_index;
								
								if(copy_to_user(to_user_ptr(Data_addr), pD3D_LUT_17x17x17, hal_VPQ_DATA_EXTERN.length)) {
									rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_SET_D_3DLUT_Offset table copy fail\n");
									ret = -1;
								} else {
									rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset cp to user\n");
								}
								dvr_free((void *)pD3D_LUT_17x17x17);
							}
						}
					}
					else
					{
						rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset ver error ver=%d,\n", hal_VPQ_DATA_EXTERN.version);
						ret = -1;	
					}
				}
				else
				{
					rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_D_3DLUT_Offset only get or set,\n");
					ret = -1;
				}
			}	
		}		
		break;

		case VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA:
		{
			extern VIP_DEM_TBL VIP_AP_DEM_TBL;
			unsigned int *DEM_arg = NULL;
			VIP_DEM_PANEL_INI_TBL *pPANEL_INI_TBL = NULL;
			unsigned int panel_ini_size;						
			unsigned int CS_mode, dynamic_hdr, isCalmanCurve;
			_system_setting_info *VIP_system_info_structure_table = (_system_setting_info *)Scaler_GetShareMemVirAddr(SCALERIOC_VIP_system_info_structure);
			
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} else {				
				Data_addr = hal_VPQ_DATA_EXTERN.pData;
				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA,version=%d,cmd=%d,length=%d,pData=%x,\n",
					hal_VPQ_DATA_EXTERN.version, hal_VPQ_DATA_EXTERN.wId, hal_VPQ_DATA_EXTERN.length, hal_VPQ_DATA_EXTERN.pData);
				
				if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Set)
				{
					if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_APDEM_INIT_PARA_Set_Ini_Table) 
					{
						panel_ini_size = sizeof(VIP_DEM_PANEL_INI_TBL);
						if (hal_VPQ_DATA_EXTERN.length != panel_ini_size) {
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA len error, len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						} else {
							DEM_arg = (unsigned int *)dvr_malloc_specific(hal_VPQ_DATA_EXTERN.length, GFP_DCU2_FIRST);
							if (DEM_arg == NULL) {
								rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA alloc fail, len=%d,\n", hal_VPQ_DATA_EXTERN.length);
								ret = -1;
							} else {
								if(copy_from_user(DEM_arg, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
								{
									rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA table copy fail\n");
									ret = -1;
								} else {
									ret = Scaler_set_APDEM(DEM_ARG_Gamma_CURVE_TBL_PANEL_Chromaticity, (void *)DEM_arg);
									rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA done done\n");
								}
								dvr_free((void *)DEM_arg);
							}
							
						}
					}
					else if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_APDEM_INIT_PARA_Set_Factory_Color_Chromaticity)
					{
						if (hal_VPQ_DATA_EXTERN.length != sizeof(VIP_DEM_PANEL_INI_Color_Chromaticity_Facotry)) {
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA len error, len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						} else {
							pPANEL_INI_TBL = (VIP_DEM_PANEL_INI_TBL *)dvr_malloc_specific(sizeof(VIP_DEM_PANEL_INI_TBL), GFP_DCU2_FIRST);
							if (pPANEL_INI_TBL == NULL) {
								rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA alloc fail, len=%d,\n", hal_VPQ_DATA_EXTERN.length);
								ret = -1;
							} else {
								memcpy(pPANEL_INI_TBL, &VIP_AP_DEM_TBL.PANEL_DATA_, sizeof(VIP_DEM_PANEL_INI_TBL));
								if(copy_from_user(&gColor_Chromaticity_Facotry_Set, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
								{
									rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA table copy fail\n");
									ret = -1;
								} else {
									pPANEL_INI_TBL->DEM_PANEL_INI_Color_Chromaticity.Panel_red_x = gColor_Chromaticity_Facotry_Set.Panel_red_x;
									pPANEL_INI_TBL->DEM_PANEL_INI_Color_Chromaticity.Panel_red_y = gColor_Chromaticity_Facotry_Set.Panel_red_y;
									pPANEL_INI_TBL->DEM_PANEL_INI_Color_Chromaticity.Panel_green_x = gColor_Chromaticity_Facotry_Set.Panel_green_x;
									pPANEL_INI_TBL->DEM_PANEL_INI_Color_Chromaticity.Panel_green_y = gColor_Chromaticity_Facotry_Set.Panel_green_y;
									pPANEL_INI_TBL->DEM_PANEL_INI_Color_Chromaticity.Panel_blue_x = gColor_Chromaticity_Facotry_Set.Panel_blue_x;
									pPANEL_INI_TBL->DEM_PANEL_INI_Color_Chromaticity.Panel_blue_y = gColor_Chromaticity_Facotry_Set.Panel_blue_y;
									ret = Scaler_set_APDEM(DEM_ARG_Gamma_CURVE_TBL_PANEL_Chromaticity, (void *)pPANEL_INI_TBL);
									CS_mode = Scaler_APDEM_Arg_Access(DEM_ARG_Color_Space, 0, 0); 	
									ret = Scaler_set_APDEM(DEM_ARG_Color_Space, (void *)&CS_mode);
								}
								dvr_free((void *)pPANEL_INI_TBL);
							}
							
						}
					}
					else if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_APDEM_INIT_PARA_Set_Factory_EOTF_OETF)
					{
						if (hal_VPQ_DATA_EXTERN.length != sizeof(VIP_DEM_PANEL_INI_Panel_Luma_Facotry)) {
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA len error, len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						} else {
							pPANEL_INI_TBL = (VIP_DEM_PANEL_INI_TBL *)dvr_malloc_specific(sizeof(VIP_DEM_PANEL_INI_TBL), GFP_DCU2_FIRST);
							if (pPANEL_INI_TBL == NULL) {
								rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA alloc fail, len=%d,\n", hal_VPQ_DATA_EXTERN.length);
								ret = -1;
							} else {
								if(copy_from_user(&gHDR_EOTF_OETF_Facotry_Set, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
								{
									rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA table copy fail\n");
									ret = -1;
								} else {									
									memcpy(pPANEL_INI_TBL, &VIP_AP_DEM_TBL.PANEL_DATA_, sizeof(VIP_DEM_PANEL_INI_TBL));
									pPANEL_INI_TBL->DEM_PANEL_INI_Color_Chromaticity.Panel_luma_max = gHDR_EOTF_OETF_Facotry_Set.Panel_luma_max;
									ret = Scaler_set_APDEM(DEM_ARG_Gamma_CURVE_TBL_PANEL_Chromaticity, (void *)pPANEL_INI_TBL);

									dynamic_hdr = 0;
									isCalmanCurve = gHDR_EOTF_OETF_Facotry_Set.CalmanCurve;
									ret = Scaler_set_APDEM(DEM_ARG_HDR_OETF_Setting_Ctrl, (void *)&dynamic_hdr);		
									ret = Scaler_set_APDEM(DEM_ARG_HDR_OETF_Curve_Ctrl, (void *)&isCalmanCurve); 

									fwif_color_DM2_EOTF_OETF_3D_LUT_Handler(VIP_system_info_structure_table->HDR_flag, gHDR_EOTF_OETF_Facotry_Set.Max_CLL_set, gHDR_EOTF_OETF_Facotry_Set.Max_CLL_set);
									
									rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA factory set,Panel_luma_max=%d,CalmanCurve=%d,HDR_Type=%d,Max_CLL_set=%d,\n", 
										gHDR_EOTF_OETF_Facotry_Set.Panel_luma_max, gHDR_EOTF_OETF_Facotry_Set.CalmanCurve,
										gHDR_EOTF_OETF_Facotry_Set.HDR_Type, gHDR_EOTF_OETF_Facotry_Set.Max_CLL_set);						
								}
								dvr_free((void *)pPANEL_INI_TBL);
							}
							
						}
					}
					else
					{
						rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA ver error ver=%d,\n", hal_VPQ_DATA_EXTERN.version);
						ret = -1;
					}
				}
				else if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Get)
				{
					if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_APDEM_INIT_PARA_Set_Factory_Color_Chromaticity)
					{
						if (hal_VPQ_DATA_EXTERN.length != sizeof(VIP_DEM_PANEL_INI_Color_Chromaticity_Facotry)) {
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA len error, len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						} else {
							gColor_Chromaticity_Facotry_Set.Panel_red_x = VIP_AP_DEM_TBL.PANEL_DATA_.DEM_PANEL_INI_Color_Chromaticity.Panel_red_x;
							gColor_Chromaticity_Facotry_Set.Panel_red_y = VIP_AP_DEM_TBL.PANEL_DATA_.DEM_PANEL_INI_Color_Chromaticity.Panel_red_y;
							gColor_Chromaticity_Facotry_Set.Panel_green_x = VIP_AP_DEM_TBL.PANEL_DATA_.DEM_PANEL_INI_Color_Chromaticity.Panel_green_x;
							gColor_Chromaticity_Facotry_Set.Panel_green_y = VIP_AP_DEM_TBL.PANEL_DATA_.DEM_PANEL_INI_Color_Chromaticity.Panel_green_y;
							gColor_Chromaticity_Facotry_Set.Panel_blue_x = VIP_AP_DEM_TBL.PANEL_DATA_.DEM_PANEL_INI_Color_Chromaticity.Panel_blue_x;
							gColor_Chromaticity_Facotry_Set.Panel_blue_y = VIP_AP_DEM_TBL.PANEL_DATA_.DEM_PANEL_INI_Color_Chromaticity.Panel_blue_y;
							if(copy_to_user(to_user_ptr(Data_addr), &gColor_Chromaticity_Facotry_Set, hal_VPQ_DATA_EXTERN.length)) {
								rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA table copy fail\n");
								ret = -1;
							}
							else
							{
								rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA factory get,%d,%d,%d,%d,\n", 
									gColor_Chromaticity_Facotry_Set.Panel_red_x, gColor_Chromaticity_Facotry_Set.Panel_red_y,
									gColor_Chromaticity_Facotry_Set.Panel_blue_x, gColor_Chromaticity_Facotry_Set.Panel_blue_y);						
								ret = 0;
							}
						}
					}
					else if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_APDEM_INIT_PARA_Set_Factory_EOTF_OETF)
					{
						if (hal_VPQ_DATA_EXTERN.length != sizeof(VIP_DEM_PANEL_INI_Panel_Luma_Facotry)) {
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA len error, len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						} else {
							gHDR_EOTF_OETF_Facotry_Set.Panel_luma_max = VIP_AP_DEM_TBL.PANEL_DATA_.DEM_PANEL_INI_Color_Chromaticity.Panel_luma_max;
							gHDR_EOTF_OETF_Facotry_Set.CalmanCurve = Scaler_APDEM_Arg_Access(DEM_ARG_HDR_OETF_Curve_Ctrl, 0, 0);
							gHDR_EOTF_OETF_Facotry_Set.HDR_Type = VIP_system_info_structure_table->HDR_flag;
							if(copy_to_user(to_user_ptr(Data_addr), &gHDR_EOTF_OETF_Facotry_Set, hal_VPQ_DATA_EXTERN.length)) {
								rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA table copy fail\n");
								ret = -1;
							}
							else
							{
								rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA factory get,Panel_luma_max=%d,CalmanCurve=%d,HDR_Type=%d,Max_CLL_set=%d,\n", 
									gHDR_EOTF_OETF_Facotry_Set.Panel_luma_max, gHDR_EOTF_OETF_Facotry_Set.CalmanCurve,
									gHDR_EOTF_OETF_Facotry_Set.HDR_Type, gHDR_EOTF_OETF_Facotry_Set.Max_CLL_set);						
								ret = 0;
							}
						}
					}
					else
					{
						rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA ver error ver=%d,\n", hal_VPQ_DATA_EXTERN.version);
						ret = -1;
					}
				}
				else
				{
					rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_APDEM_INIT_PARA only get or set\n");
					ret = -1;
				}
			}	
		}		
		break;

		case VPQ_EXTERN_IOC_SET_After_Filter_LPF_gain_ColorDataFac:
		{
			unsigned int DEM_arg[5] = {0};	// lpf_0/25/50/75/100  
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_After_Filter_LPF_gain_ColorDataFac hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} else {
				if (hal_VPQ_DATA_EXTERN.length != sizeof(VIP_DEM_ColorDataFac)) {
					rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_After_Filter_LPF_gain_ColorDataFac len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
					ret = -1;
				} else {
					Data_addr = hal_VPQ_DATA_EXTERN.pData;
					if(copy_from_user(&DEM_arg[0], (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
					{
						rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_After_Filter_LPF_gain_ColorDataFac table copy fail\n");
						ret = -1;
					} else {
						ret = Scaler_set_APDEM(DEM_ARG_LPF_Gain_ColorDataFac, (void *)DEM_arg);
					}
				}
			}	
		}		
		break;		

		case VPQ_EXTERN_IOC_SET_OverDriver_Para:
		{
			extern unsigned char bODInited;
			extern SLR_VIP_TABLE_OD mVIP_ODTable[VIP_OD_LEVEL_MAX];
			unsigned int DEM_arg[VIP_APDEM_OD_Ch_Max] = {0};	//  
			unsigned int OD_OnOff;
			
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_OverDriver_Para hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} else {
				Data_addr = hal_VPQ_DATA_EXTERN.pData;

				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_OverDriver_Para,version=%d,cmd=%d,length=%d,pData=%x,\n",
					hal_VPQ_DATA_EXTERN.version, hal_VPQ_DATA_EXTERN.wId, hal_VPQ_DATA_EXTERN.length, hal_VPQ_DATA_EXTERN.pData);
				
				if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Set) 
				{
					if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_OverDriver_Para_OD_Gain) 
					{
						if (hal_VPQ_DATA_EXTERN.length == sizeof(int)) 
						{
							if(copy_from_user(&DEM_arg[0], (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
							{
								rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_OverDriver_Para table copy fail\n");
								ret = -1;
							} else {
								rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_OverDriver_Para %d,\n", DEM_arg[0]);
								ret = Scaler_set_APDEM(DEM_ARG_OD_Gain, (void *)&DEM_arg[0]);
							}
						} 
						else if (hal_VPQ_DATA_EXTERN.length == (sizeof(int)*2)) 
						{
							if(copy_from_user(&DEM_arg[0], (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
							{
								rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_OverDriver_Para table copy fail\n");
								ret = -1;
							} else {
								rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_OverDriver_Para %d,%d,\n", DEM_arg[0], DEM_arg[1]);
								ret = Scaler_set_APDEM(DEM_ARG_OD_Gain, (void *)&DEM_arg[0]);
								ret = Scaler_set_APDEM(DEM_ARG_OD_TBL_set_by_idx_Sel, (void *)&DEM_arg[1]);
							}
						} 
						else 
						{							
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_OverDriver_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
							
						}
					} 
					else if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_OverDriver_Para_OD_BIN_TBL_SET) 
					{
						if (hal_VPQ_DATA_EXTERN.length == sizeof(SLR_VIP_TABLE_OD)) 
						{	
							//Data_addr = hal_VPQ_DATA_EXTERN.pData;
							if(copy_from_user(&mVIP_ODTable[0], (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
							{	
								rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_OverDriver_Para bin table copy fail\n");
								ret = -1;
							} else {
								// 1x table, all use 1x
								memcpy(&mVIP_ODTable[VIP_OD_LEVEL_LOW], &mVIP_ODTable[VIP_OD_LEVEL_OFF], sizeof(SLR_VIP_TABLE_OD)); 							
								memcpy(&mVIP_ODTable[VIP_OD_LEVEL_MID], &mVIP_ODTable[VIP_OD_LEVEL_OFF], sizeof(SLR_VIP_TABLE_OD));
								memcpy(&mVIP_ODTable[VIP_OD_LEVEL_HIGH], &mVIP_ODTable[VIP_OD_LEVEL_OFF], sizeof(SLR_VIP_TABLE_OD));

								rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_OverDriver_Para bin 1x ok, struct size=%d,\n", sizeof(SLR_VIP_TABLE_OD));
								ret = 0;
							}
						}
						else if (hal_VPQ_DATA_EXTERN.length == (sizeof(SLR_VIP_TABLE_OD)*VIP_OD_LEVEL_MAX))
						{	
							//Data_addr = hal_VPQ_DATA_EXTERN.pData;
							if(copy_from_user(&mVIP_ODTable[0], (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
							{	
								rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_OverDriver_Para bin table copy fail\n");
								ret = -1;
							} else {
								// protection: only use table 0 for DMA setting
								mVIP_ODTable[VIP_OD_LEVEL_LOW].OD_Main_Ctrl.OD_Ctrl.od_en = mVIP_ODTable[VIP_OD_LEVEL_OFF].OD_Main_Ctrl.OD_Ctrl.od_en;
								mVIP_ODTable[VIP_OD_LEVEL_LOW].OD_Main_Ctrl.OD_Ctrl.Y_mode_en = mVIP_ODTable[VIP_OD_LEVEL_OFF].OD_Main_Ctrl.OD_Ctrl.Y_mode_en;
								mVIP_ODTable[VIP_OD_LEVEL_MID].OD_Main_Ctrl.OD_Ctrl.od_en = mVIP_ODTable[VIP_OD_LEVEL_OFF].OD_Main_Ctrl.OD_Ctrl.od_en;
								mVIP_ODTable[VIP_OD_LEVEL_MID].OD_Main_Ctrl.OD_Ctrl.Y_mode_en = mVIP_ODTable[VIP_OD_LEVEL_OFF].OD_Main_Ctrl.OD_Ctrl.Y_mode_en;
								mVIP_ODTable[VIP_OD_LEVEL_HIGH].OD_Main_Ctrl.OD_Ctrl.od_en = mVIP_ODTable[VIP_OD_LEVEL_OFF].OD_Main_Ctrl.OD_Ctrl.od_en;
								mVIP_ODTable[VIP_OD_LEVEL_HIGH].OD_Main_Ctrl.OD_Ctrl.Y_mode_en = mVIP_ODTable[VIP_OD_LEVEL_OFF].OD_Main_Ctrl.OD_Ctrl.Y_mode_en;

								rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_OverDriver_Para bin 4x ok, struct size=%d,\n", sizeof(SLR_VIP_TABLE_OD));
								ret = 0;
							}
						}
						else
						{
							// no bin file, disable OD					
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_OverDriver_Para, no bin file here. use kernel default len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = 0;
						}
						vpq_do_OD_init_from_tbl();
					}
					else if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_OverDriver_Para_OD_OnOff) 
					{
						if (hal_VPQ_DATA_EXTERN.length != sizeof(int)) {	//
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_OverDriver_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						} else {
							Data_addr = hal_VPQ_DATA_EXTERN.pData;
							if(copy_from_user(&OD_OnOff, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
							{	
								rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_OverDriver_Para bin table copy fail\n");
								ret = -1;
							} else {
								if (OD_OnOff == 1 && bODInited == 1 && mVIP_ODTable[0].OD_Main_Ctrl.OD_Ctrl.od_en)
								{
									fwif_color_set_od_dma_enable(1);
									usleep_range(21000, 22000);
									fwif_color_set_od_enable(1);
								}
								else
								{
									fwif_color_set_od_dma_enable(0);
									fwif_color_set_od_enable(0);
								}		
								rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_OverDriver_Para onoff=%d,bODInited=%d,od_en=%d,\n", 
									OD_OnOff, bODInited, mVIP_ODTable[0].OD_Main_Ctrl.OD_Ctrl.od_en);
								ret = 0;
							}
						}
					}
					else 
					{
						rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_OverDriver_Para ver error ver=%d,\n", hal_VPQ_DATA_EXTERN.version);
						ret = -1;
					}
				}	
				else if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Get) 
				{
					if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_OverDriver_Para_OD_BIN_TBL_CRC_Get) {
						if (hal_VPQ_DATA_EXTERN.length != sizeof(int)) {						
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_OverDriver_Para, error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = 0;
						} else {
							// crc cal
							sizeByte = hal_VPQ_DATA_EXTERN.length;
							CRC_nByte = sizeof(SLR_VIP_TABLE_OD)*VIP_OD_LEVEL_MAX; // CRC is not include Header
							pBuff_tmp = (unsigned char*)&mVIP_ODTable[0];
							crc_cal = VPQEX_VPQ_CalcCRC16(pBuff_tmp, CRC_nByte);
							crc_16_tmp_32 = crc_cal;
							if(copy_to_user(to_user_ptr(Data_addr), &crc_16_tmp_32, hal_VPQ_DATA_EXTERN.length)) {
								rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_SET_OverDriver_Para table copy fail\n");
								ret = -1;
							}
							else
							{
								rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_OverDriver_Para bin crc get, sizeByte=%d, CRC_nByte=%d, crc_cal=%d,\n", 
									sizeByte, CRC_nByte, crc_cal);						
								ret = 0;
							}									
						}
					} 
					else 
					{
						rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_OverDriver_Para version error\n");
						ret = -1;
					}
				}
				else 
				{
					rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_OverDriver_Para only get or set\n");
					ret = -1;
				}				
			}	
		}		
		break;

#if 1	// tconless_kdriver_OUTGAMMA_flow_
		case VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para:
		{
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} else {
				extern SLR_VIP_TABLE_OUTGAMMA mVIP_OutputGamma;
				extern SLR_VIP_TABLE_AUTO_ACC_CTRL mVIP_TABLE_AUTO_ACC_CTRL;
				SLR_VIP_TABLE_AUTO_ACC_CTRL *pAUTO_ACC_CTRL_temp = NULL;
				
				Data_addr = hal_VPQ_DATA_EXTERN.pData;
				
				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para,version=%d,cmd=%d,length=%d,pData=%x,\n",
					hal_VPQ_DATA_EXTERN.version, hal_VPQ_DATA_EXTERN.wId, hal_VPQ_DATA_EXTERN.length, hal_VPQ_DATA_EXTERN.pData);
				
				if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Set) 
				{
					if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_OUTGAMMA_BIN_TBL_Set) 
					{
						if (hal_VPQ_DATA_EXTERN.length == sizeof(SLR_VIP_TABLE_OUTGAMMA)) {
							if(copy_from_user(&mVIP_OutputGamma, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
							{	
								rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para bin table copy fail\n");
								ret = -1;
							} else {
	
								// update AutoACC_CTRL_OG_En, AutoACC_CTRL_OG_Loc
								mVIP_TABLE_AUTO_ACC_CTRL.Header[AutoACC_CTRL_OG_En] = mVIP_OutputGamma.OutputGamma_Main_Ctrl.OutputGamma_Ctrl.OG_En;
								mVIP_TABLE_AUTO_ACC_CTRL.Header[AutoACC_CTRL_OG_Loc] = mVIP_OutputGamma.OutputGamma_Main_Ctrl.OutputGamma_Ctrl.OG_Loc;								
	
								rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para bin ok, ,mOG_en=%d,mOG_loc=%d val=%d,,\n",
									mVIP_OutputGamma.OutputGamma_Main_Ctrl.OutputGamma_Ctrl.OG_En, mVIP_OutputGamma.OutputGamma_Main_Ctrl.OutputGamma_Ctrl.OG_Loc, 
									mVIP_OutputGamma.OUTGAMMA_TBL_Extend[SLR_VIP_OUTGAMMA_TBL_Num-1][VIP_CHANNEL_RGB_MAX-1][SLR_VIP_OUTGAMMA_Index_Num-1]);
								ret = 0;
							}							
						} else {
							rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;	
						}
					} 
#if 1 // tconless_kdriver_flow_Enable_later	
					else if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_OUTGAMMA_Freq_VRR_Mode_Set) 
					{
						if (hal_VPQ_DATA_EXTERN.length == 4 * sizeof(int)) 
						{
							pTemp32 = (unsigned int *)kmalloc(hal_VPQ_DATA_EXTERN.length * sizeof(char), GFP_KERNEL);
							pTemp16 = (unsigned short *)kmalloc(3 * 1024 * sizeof(short), GFP_KERNEL);
							if (pTemp32 == NULL || pTemp16 == NULL)
							{
								rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para kmalloc fail\n");
								if (pTemp32 != NULL)
									kfree(pTemp32);
								if (pTemp16 != NULL)
									kfree(pTemp16);
								
								ret = -1;
							}
							else
							{
								if(copy_from_user(pTemp32, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
								{	
									rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para bin table copy fail\n");
									ret = -1;
								} else {
	
									pTemp32[1] = fwif_color_Get_D_Domain_Freq(2); //pTemp32[1];
									
									if (pTemp32[0] == AUTO_ACC_Mode_AUTOACC_Fix_Freq)
									{
										for (i=0;i<SLR_VIP_OUTGAMMA_Index_Num;i++)
										{
											pTemp16[i] = mVIP_TABLE_AUTO_ACC_CTRL.OUTGAMMA_TBL[VIP_CHANNEL_R][i];
											pTemp16[i+SLR_VIP_OUTGAMMA_Index_Num] = mVIP_TABLE_AUTO_ACC_CTRL.OUTGAMMA_TBL[VIP_CHANNEL_G][i];
											pTemp16[i+SLR_VIP_OUTGAMMA_Index_Num*2] = mVIP_TABLE_AUTO_ACC_CTRL.OUTGAMMA_TBL[VIP_CHANNEL_B][i];
										}
	
										//tmp0 = mVIP_TABLE_AUTO_ACC_CTRL.Header[AutoACC_CTRL_OG_En];
										//tmp1 = mVIP_TABLE_AUTO_ACC_CTRL.Header[AutoACC_CTRL_OG_Loc];
									}
									else if (pTemp32[0] == AUTO_ACC_Mode_OUTGAMMA_BIN_Fix_Freq)
									{
										tbl_sel = fwif_color_AuoACC_OutGamma_Table_sel_byFreq(&mVIP_OutputGamma.OUTGAMMA_TH_Extend[0], 
											pTemp32[1], 1);
										//pTemp16 = &mVIP_OutputGamma.OUTGAMMA_TBL_Extend[i][0][0];
										memcpy(pTemp16, &mVIP_OutputGamma.OUTGAMMA_TBL_Extend[tbl_sel][0][0], 
											sizeof(short)*VIP_CHANNEL_RGB_MAX*SLR_VIP_OUTGAMMA_Index_Num);
										//tmp0 = mVIP_OutputGamma.OutputGamma_Main_Ctrl.OutputGamma_Ctrl.OG_En;
										//tmp1 = mVIP_OutputGamma.OutputGamma_Main_Ctrl.OutputGamma_Ctrl.OG_Loc;										
									}
									else
									{
										tbl_sel = fwif_color_AuoACC_OutGamma_Table_sel_byFreq(&mVIP_OutputGamma.OUTGAMMA_TH[0], 0xFF, 1);
										//pTemp16 = &mVIP_OutputGamma.OUTGAMMA_TBL[i][0][0];
										memcpy(pTemp16, &mVIP_OutputGamma.OUTGAMMA_TBL[tbl_sel][0][0], 
											sizeof(short)*VIP_CHANNEL_RGB_MAX*SLR_VIP_OUTGAMMA_Index_Num);
										//tmp0 = mVIP_OutputGamma.OutputGamma_Main_Ctrl.OutputGamma_Ctrl.OG_En;
										//tmp1 = mVIP_OutputGamma.OutputGamma_Main_Ctrl.OutputGamma_Ctrl.OG_Loc;										
									}
									tmp0 = mVIP_TABLE_AUTO_ACC_CTRL.Header[AutoACC_CTRL_OG_En];
									tmp1 = mVIP_TABLE_AUTO_ACC_CTRL.Header[AutoACC_CTRL_OG_Loc];

									for (i=0;i<SLR_VIP_OUTGAMMA_Index_Num;i++)
									{
										mVIP_TABLE_AUTO_ACC_CTRL.OUTGAMMA_TBL[VIP_CHANNEL_R][i] = pTemp16[i];
										mVIP_TABLE_AUTO_ACC_CTRL.OUTGAMMA_TBL[VIP_CHANNEL_G][i] = pTemp16[i+SLR_VIP_OUTGAMMA_Index_Num];
										mVIP_TABLE_AUTO_ACC_CTRL.OUTGAMMA_TBL[VIP_CHANNEL_B][i] = pTemp16[i+SLR_VIP_OUTGAMMA_Index_Num*2];
									}
									mVIP_TABLE_AUTO_ACC_CTRL.Header[AutoACC_CTRL_index_num] = 1024; // only support 1024
									mVIP_TABLE_AUTO_ACC_CTRL.Header[AutoACC_CTRL_bit_num] = 14; 	// all convert to 14 bit									
									mVIP_TABLE_AUTO_ACC_CTRL.Header[AutoACC_CTRL_Freq_for_Fix_TBL] = pTemp32[1];//fwif_color_Get_D_Domain_Freq(2); //pTemp32[1];
									mVIP_TABLE_AUTO_ACC_CTRL.Header[AutoACC_CTRL_mode] = pTemp32[0];
									
									
									rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para set mode=%d,freq=%d,mOG_en=%d,mOG_loc=%d,\n", 
										mVIP_TABLE_AUTO_ACC_CTRL.Header[AutoACC_CTRL_mode], mVIP_TABLE_AUTO_ACC_CTRL.Header[AutoACC_CTRL_Freq_for_Fix_TBL], tmp0, tmp1);
#if 1							
									fwif_color_colorwrite_Output_gamma_Enhance(tmp0, tmp1, pTemp16, pTemp16+SLR_VIP_OUTGAMMA_Index_Num,
										pTemp16+2*SLR_VIP_OUTGAMMA_Index_Num);
#endif																	
									ret = 0;
								}
								kfree(pTemp32);
								kfree(pTemp16);
							}							
						}
						else 
						{
							rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;	
						}
					}
					else if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_OUTGAMMA_AUTO_ACC_Set) 
					{
						if (hal_VPQ_DATA_EXTERN.length == sizeof(SLR_VIP_TABLE_AUTO_ACC_CTRL)) 
						{
							pAUTO_ACC_CTRL_temp = (SLR_VIP_TABLE_AUTO_ACC_CTRL *)kmalloc(hal_VPQ_DATA_EXTERN.length, GFP_KERNEL);
							if (pAUTO_ACC_CTRL_temp == NULL)
							{
								rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para kmalloc fail\n");
								ret = -1;
							}
							else
							{
								if(copy_from_user(pAUTO_ACC_CTRL_temp, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
								{	
									rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para bin table copy fail\n");
									ret = -1;
								} else {
									if (VPQEX_set_Auto_ACC(pAUTO_ACC_CTRL_temp, &mVIP_TABLE_AUTO_ACC_CTRL) != 0)
									{
										rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para acc set fail\n");
										ret = -1;
									}
									else
									{
										mVIP_TABLE_AUTO_ACC_CTRL.Header[AutoACC_CTRL_index_num] = 1024;	// only support 1024
										mVIP_TABLE_AUTO_ACC_CTRL.Header[AutoACC_CTRL_bit_num] = 14;		// all convert to 14 bit								
										ret = 0;
									}
								}
								kfree(pAUTO_ACC_CTRL_temp);
							}							
						} 
						else 
						{
							rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;	
						}
					}
#endif				
					else 
					{
						rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para version error\n");
						ret = -1;
					}
				} 
				else if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Get)
				{
#if 1 // tconless_kdriver_flow_Enable_later
					if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_OUTGAMMA_Get) 
					{
						if (hal_VPQ_DATA_EXTERN.length == sizeof(SLR_VIP_TABLE_AUTO_ACC_CTRL)) 
						{
							CRC_nByte = sizeof(int) * VIP_CHANNEL_RGB_MAX * SLR_VIP_OUTGAMMA_Index_Num;
							crc_cal = VPQEX_VPQ_CalcCRC16((unsigned char*)&mVIP_TABLE_AUTO_ACC_CTRL.OUTGAMMA_TBL[0][0], CRC_nByte);
							mVIP_TABLE_AUTO_ACC_CTRL.Header[AutoACC_CTRL_CRC] = crc_cal;
							mVIP_TABLE_AUTO_ACC_CTRL.Header[AutoACC_CTRL_OG_TBL_Size] = CRC_nByte;
							
							if(copy_to_user(to_user_ptr(Data_addr), &mVIP_TABLE_AUTO_ACC_CTRL, hal_VPQ_DATA_EXTERN.length)) {
								rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para table copy fail\n");
								ret = -1;
							}
							else
							{
								rtd_pr_vpq_info("VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para to AP, sizeByte is fix, CRC_nByte=%d, crc=%d,\n", 
									CRC_nByte, crc_cal);					
								ret = 0;
							}
						} 
						else 
						{
							rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;	
						}
					}
					else if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_OUTGAMMA_BIN_TBL_CRC_Get) 
					{
						if (hal_VPQ_DATA_EXTERN.length != sizeof(int)) {					
							rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para, error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = 0;
						} else {
							// crc cal
							sizeByte = hal_VPQ_DATA_EXTERN.length;
							CRC_nByte = sizeof(SLR_VIP_TABLE_OUTGAMMA); // CRC is not include Header
							pBuff_tmp = (unsigned char*)&mVIP_OutputGamma;
							crc_cal = VPQEX_VPQ_CalcCRC16(pBuff_tmp, CRC_nByte);
							crc_16_tmp_32 = crc_cal;
							if(copy_to_user(to_user_ptr(Data_addr), &crc_16_tmp_32, hal_VPQ_DATA_EXTERN.length)) {
								rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para table copy fail\n");
								ret = -1;
							}
							else
							{
								rtd_pr_vpq_info("VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para bin crc get, sizeByte=%d, CRC_nByte=%d, crc_cal=%d,\n", 
									sizeByte, CRC_nByte, crc_cal);						
								ret = 0;
							}									
						}
					} 					
					else 
					{
						rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para version error\n");
						ret = -1;
					}
#endif				
				}
				else 
				{
					rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_OUTGAMMA_Para only get or set,\n");
					ret = -1;
				}
			}
		}
		break;
#endif

#if 1	// tconless_kdriver_LINEOD_flow_
		case VPQ_EXTERN_IOC_ACCESS_TCON_LINOD_Para:
		{
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_LINOD_Para hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} else {
				extern SLR_VIP_TABLE_LINEOD mVIP_PCIDTable;
				SLR_VIP_TBL_HEADER_TBL_LINEOD *pHEADER_TBL_LINEOD;
				Data_addr = hal_VPQ_DATA_EXTERN.pData;
				
				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_ACCESS_TCON_LINOD_Para,version=%d,cmd=%d,length=%d,pData=%x,\n",
					hal_VPQ_DATA_EXTERN.version, hal_VPQ_DATA_EXTERN.wId, hal_VPQ_DATA_EXTERN.length, hal_VPQ_DATA_EXTERN.pData);
				
				if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Set) 
				{
					if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_LINEOD_BIN_TBL_Set) {
						if (hal_VPQ_DATA_EXTERN.length == sizeof(SLR_VIP_TABLE_LINEOD)) {
							if(copy_from_user(&mVIP_PCIDTable, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
							{	
								rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_LINOD_Para bin table copy fail\n");
								ret = -1;
							} else {
								drvif_color_get_pcid_enable(&temp8);
								rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_ACCESS_TCON_LINOD_Para bin ok,val=%d,%d,en=%d,\n",
									mVIP_PCIDTable.VIP_PCID_En_Ctrl.LINEOD_EN, 
									mVIP_PCIDTable.PCID_TBL.PCID_1D2D_TBL.LUT2_Blue[SLR_VIP_PCID_TBL_SIZE-1], temp8); 
#if 1	// tconless_kdriver_flow_Enable_later								
								if (temp8 == 0)	// boot code enable line od, skip
									fwif_color_set_LINEOD_by_pSLRTBL(&mVIP_PCIDTable);
#endif
								ret = 0;
							}
						} else {
							rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_LINOD_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						}
					} else {
						rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_LINOD_Para version error\n");
						ret = -1;
					}
				}
				else if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Get) 
				{
					if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_LINEOD_TBL_Size_Get) 
					{
						if (hal_VPQ_DATA_EXTERN.length == sizeof(int))
						{
							sizeByte = sizeof(SLR_VIP_TBL_HEADER_TBL_LINEOD);
							if(copy_to_user(to_user_ptr(Data_addr), &sizeByte, hal_VPQ_DATA_EXTERN.length)) {
								rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_LINOD_Para table copy fail\n");
								ret = -1;
							}
							else
							{
								rtd_pr_vpq_info("VPQ_EXTERN_IOC_ACCESS_TCON_LINOD_Para structure size=%d,\n", sizeByte);						
								ret = 0;
							}								
						}
						else
						{
							rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_LINOD_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						}
					}
					else if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_LINEOD_TBL_Get) 
					{
						if (hal_VPQ_DATA_EXTERN.length == sizeof(SLR_VIP_TBL_HEADER_TBL_LINEOD))
						{
							pHEADER_TBL_LINEOD = (SLR_VIP_TBL_HEADER_TBL_LINEOD *)kmalloc(sizeof(SLR_VIP_TBL_HEADER_TBL_LINEOD), GFP_KERNEL);
							if (pHEADER_TBL_LINEOD == NULL)
							{
								rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_LINOD_Para table copy fail\n");
								ret = -1;
							}
							else 
							{
								// table copy
								memcpy(&pHEADER_TBL_LINEOD->Table_LINEOD, &mVIP_PCIDTable, sizeof(SLR_VIP_TABLE_LINEOD));
								// crc cal
								sizeByte = hal_VPQ_DATA_EXTERN.length;
								CRC_nByte = sizeof(SLR_VIP_TABLE_LINEOD); // HEADER is not include Header
								pBuff_tmp = (unsigned char*)&pHEADER_TBL_LINEOD->Table_LINEOD;
								crc_cal = VPQEX_VPQ_CalcCRC16(pBuff_tmp, CRC_nByte);
								pHEADER_TBL_LINEOD->HEADER[LINEOD_TBL_HEADER_TBL_CRC16] = crc_cal;
								pHEADER_TBL_LINEOD->HEADER[LINEOD_TBL_HEADER_TBL_SIZE] = CRC_nByte;
								
								if(copy_to_user(to_user_ptr(Data_addr), pHEADER_TBL_LINEOD, hal_VPQ_DATA_EXTERN.length)) {
									rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_LINOD_Para table copy fail\n");
									ret = -1;
								}
								else
								{
									rtd_pr_vpq_info("VPQ_EXTERN_IOC_ACCESS_TCON_LINOD_Para sizeByte=%d, CRC_nByte=%d, crc_cal=%d,\n", 
										sizeByte, CRC_nByte, crc_cal);						
									ret = 0;
								}
								kfree(pHEADER_TBL_LINEOD);
							}
						}
						else
						{
							rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_LINOD_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						}
					} 
					else if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_LINEOD_BIN_TBL_CRC_Get) 
					{
						if (hal_VPQ_DATA_EXTERN.length != sizeof(int)) {						
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_ACCESS_TCON_LINOD_Para, error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = 0;
						} else {
							// crc cal
							sizeByte = hal_VPQ_DATA_EXTERN.length;
							CRC_nByte = sizeof(SLR_VIP_TABLE_LINEOD); // CRC is not include Header
							pBuff_tmp = (unsigned char*)&mVIP_PCIDTable;
							crc_cal = VPQEX_VPQ_CalcCRC16(pBuff_tmp, CRC_nByte);
							crc_16_tmp_32 = crc_cal;
							if(copy_to_user(to_user_ptr(Data_addr), &crc_16_tmp_32, hal_VPQ_DATA_EXTERN.length)) {
								rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_LINOD_Para table copy fail\n");
								ret = -1;
							}
							else
							{
								rtd_pr_vpq_info("VPQ_EXTERN_IOC_ACCESS_TCON_LINOD_Para bin crc get, sizeByte=%d, CRC_nByte=%d, crc_cal=%d,\n", 
									sizeByte, CRC_nByte, crc_cal);						
								ret = 0;
							}									
						}
					}					
					else 
					{
						rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_LINOD_Para version error\n");
						ret = -1;
					}
				}
				else 
				{
					rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_LINOD_Para only get or set,\n");
					ret = -1;
				}
			}
		}
		break;
#endif

#if 1	// tconless_kdriver_VALC_flow_
		case VPQ_EXTERN_IOC_ACCESS_TCON_VALC_Para:
		{
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_VALC_Para hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} else {
				extern SLR_VIP_TABLE_VALC mVIP_VALC_Table;
				SLR_VIP_TBL_HEADER_TBL_VALC *pHEADER_TBL_VALC;
				Data_addr = hal_VPQ_DATA_EXTERN.pData;
				
				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_ACCESS_TCON_VALC_Para,version=%d,cmd=%d,length=%d,pData=%x,\n",
					hal_VPQ_DATA_EXTERN.version, hal_VPQ_DATA_EXTERN.wId, hal_VPQ_DATA_EXTERN.length, hal_VPQ_DATA_EXTERN.pData);
				
				if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Set) 
				{
					if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_VALC_BIN_TBL_Set) {
						if (hal_VPQ_DATA_EXTERN.length == sizeof(SLR_VIP_TABLE_VALC)) {
							if(copy_from_user(&mVIP_VALC_Table, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
							{	
								rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_VALC_Para bin table copy fail\n");
								ret = -1;
							} else {
								rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_ACCESS_TCON_VALC_Para bin ok, val = %d,%d,en=%d,%d,%d,\n",
									mVIP_VALC_Table.VIP_VALC_En_Ctrl.PCID_EN,
									mVIP_VALC_Table.VALC_TBL.VALC_1D2D_TBL.LUT2_Blue[SLR_VIP_VALC_TBL_SIZE-1],
									driver_color_get_valc_enable(), drvif_color_get_valc_enable(), drvif_color_get_valc_enable2());								
#if 1	// tconless_kdriver_flow_Enable_later								
								if ( (driver_color_get_valc_enable() == 0) && (drvif_color_get_valc_enable() == 0) &&
									(drvif_color_get_valc_enable2() == 0))	// boot code enable valc, skip								
									fwif_color_set_VALC_by_pSLRTBL(&mVIP_VALC_Table);
#endif
								ret = 0;
							}	
						} else {
							rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_VALC_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						}
					} else {
						rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_VALC_Para version error\n");
						ret = -1;
					}
				}
				else if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Get) 
				{
					if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_VALC_TBL_Size_Get) 
					{
						if (hal_VPQ_DATA_EXTERN.length == sizeof(int))
						{
							sizeByte = sizeof(SLR_VIP_TBL_HEADER_TBL_VALC);
							if(copy_to_user(to_user_ptr(Data_addr), &sizeByte, hal_VPQ_DATA_EXTERN.length)) {
								rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_VALC_Para table copy fail\n");
								ret = -1;
							}
							else
							{
								rtd_pr_vpq_info("VPQ_EXTERN_IOC_ACCESS_TCON_VALC_Para structure size=%d,\n", sizeByte);						
								ret = 0;
							}								
						}
						else
						{
							rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_VALC_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						}
					}
					else if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_VALC_TBL_Get) 
					{
						if (hal_VPQ_DATA_EXTERN.length == sizeof(SLR_VIP_TBL_HEADER_TBL_VALC))
						{
							pHEADER_TBL_VALC = (SLR_VIP_TBL_HEADER_TBL_VALC *)kmalloc(sizeof(SLR_VIP_TBL_HEADER_TBL_VALC), GFP_KERNEL);
							if (pHEADER_TBL_VALC == NULL)
							{
								rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_VALC_Para table copy fail\n");
								ret = -1;
							}
							else 
							{
								// table copy
								memcpy(&pHEADER_TBL_VALC->Table_VALC, &mVIP_VALC_Table, sizeof(SLR_VIP_TABLE_VALC));
								// crc cal
								sizeByte = hal_VPQ_DATA_EXTERN.length;
								CRC_nByte = sizeof(SLR_VIP_TABLE_VALC);	// HEADER is not include HEADER
								pBuff_tmp = (unsigned char*)&pHEADER_TBL_VALC->Table_VALC;
								crc_cal = VPQEX_VPQ_CalcCRC16(pBuff_tmp, CRC_nByte);
								pHEADER_TBL_VALC->HEADER[VALC_TBL_HEADER_TBL_CRC16] = crc_cal;
								pHEADER_TBL_VALC->HEADER[VALC_TBL_HEADER_TBL_SIZE] = CRC_nByte;

								rtd_pr_vpq_info("VPQ_EXTERN_IOC_ACCESS_TCON_VALC_Para size=%d,nByte=%d,crc_cal=%d,crc=%d,size=%d,\n", 
									sizeByte, CRC_nByte, crc_cal, pHEADER_TBL_VALC->HEADER[VALC_TBL_HEADER_TBL_CRC16], 
									pHEADER_TBL_VALC->HEADER[VALC_TBL_HEADER_TBL_SIZE]);						
								
								if(copy_to_user(to_user_ptr(Data_addr), pHEADER_TBL_VALC, hal_VPQ_DATA_EXTERN.length)) {
									rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_VALC_Para table copy fail\n");
									ret = -1;
								}
								else
								{
									rtd_pr_vpq_info("VPQ_EXTERN_IOC_ACCESS_TCON_VALC_Para sizeByte=%d, CRC_nByte=%d, crc_cal=%d,\n", 
										sizeByte, CRC_nByte, crc_cal);						
									ret = 0;
								}
								kfree(pHEADER_TBL_VALC);
							}
						}			
						else
						{
							rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_VALC_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						}
					} 
					else if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_VALC_BIN_TBL_CRC_Get) 
					{
						if (hal_VPQ_DATA_EXTERN.length != sizeof(int)) {						
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_ACCESS_TCON_VALC_Para, error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = 0;
						} else {
							// crc cal
							sizeByte = hal_VPQ_DATA_EXTERN.length;
							CRC_nByte = sizeof(SLR_VIP_TABLE_VALC); // CRC is not include Header
							pBuff_tmp = (unsigned char*)&mVIP_VALC_Table;
							crc_cal = VPQEX_VPQ_CalcCRC16(pBuff_tmp, CRC_nByte);
							crc_16_tmp_32 = crc_cal;
							if(copy_to_user(to_user_ptr(Data_addr), &crc_16_tmp_32, hal_VPQ_DATA_EXTERN.length)) {
								rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_VALC_Para table copy fail\n");
								ret = -1;
							}
							else
							{
								rtd_pr_vpq_info("VPQ_EXTERN_IOC_ACCESS_TCON_VALC_Para bin crc get, sizeByte=%d, CRC_nByte=%d, crc_cal=%d,\n", 
									sizeByte, CRC_nByte, crc_cal);						
								ret = 0;
							}									
						}
					}					
					else 
					{
						rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_VALC_Para version error\n");
						ret = -1;
					}
				}
				else 
				{
					rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_VALC_Para only get or set,\n");
					ret = -1;
				}
			}
		}
		break;
#endif

#if 1	// tconless_kdriver_Demura_flow_ // tconless_kdriver_flow_Enable_later	
			case VPQ_EXTERN_IOC_ACCESS_TCON_DEMURA_Para:
			{
				unsigned long long VIP_query_size;
				unsigned long long query_addr_sta;	
		
				if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) 
				{
					rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_DEMURA_Para hal_VPQ_DATA_EXTERN copy fail\n");
					ret = -1;
				} 
				else 
				{
					rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_ACCESS_TCON_DEMURA_Para,version=%d,cmd=%d,length=%d,pData=%x,\n",
						hal_VPQ_DATA_EXTERN.version, hal_VPQ_DATA_EXTERN.wId, hal_VPQ_DATA_EXTERN.length, hal_VPQ_DATA_EXTERN.pData);
		
					Data_addr = hal_VPQ_DATA_EXTERN.pData;
					if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Set) 
					{
						VIP_query_size = carvedout_buf_query(CARVEDOUT_SCALER_VIP, (void **)&query_addr_sta);				
						if (VIP_query_size >= (2*1024*1024)) 
						{
							if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_DEMURA_CSOTPDM) 
							{
								ret = VPQEX_CSOTPDM_to_internal_Demura(&hal_VPQ_DATA_EXTERN);
								rtd_pr_vpq_info("VPQ_EXTERN_IOC_ACCESS_TCON_DEMURA_Para,CSOTPDM, ret=%d,\n", ret);
							}
							else if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_DEMURA_HKCPDM) 
							{
								ret = VPQEX_HKCPDM_to_internal_Demura(&hal_VPQ_DATA_EXTERN);
								rtd_pr_vpq_info("VPQ_EXTERN_IOC_ACCESS_TCON_DEMURA_Para,HKCPDM, ret=%d,\n", ret);
							}
							else if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_DEMURA_CHOTPDM) 
							{
								ret = VPQEX_CHOTPDM_to_internal_Demura(&hal_VPQ_DATA_EXTERN);
								rtd_pr_vpq_info("VPQ_EXTERN_IOC_ACCESS_TCON_DEMURA_Para,CHOTPDM, ret=%d,\n", ret);
							}
							else if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_DEMURA_INNXPDM) 
							{
								ret = VPQEX_INNXPDM_to_internal_Demura(&hal_VPQ_DATA_EXTERN);
								rtd_pr_vpq_info("VPQ_EXTERN_IOC_ACCESS_TCON_DEMURA_Para,INNXPDM, ret=%d,\n", ret);
							}
							else if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_DEMURA_Encode_TBL) 
							{
								if (hal_VPQ_DATA_EXTERN.length == sizeof(SLR_VIP_TBL_HEADER_TBL_DeMura))
								{
		
									if(copy_from_user(&demura_HEADER_tmp[0], (int __user *)Data_addr, sizeof(demura_HEADER_tmp))){
										rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_DEMURA_Para table copy fail 1\n");
										ret = -1;
									} 
									else
									{
										if(copy_from_user(&DeMura_TBL, (int __user *)(Data_addr+sizeof(demura_HEADER_tmp)), hal_VPQ_DATA_EXTERN.length-sizeof(demura_HEADER_tmp))){
											rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_ACCESS_TCON_DEMURA_Para table copy fail 2\n");
											ret = -1;
										} 
										else
										{
											pBuff_tmp = (unsigned char*)&DeMura_TBL;
											//pDemura_crc16 = (unsigned short*)(&DeMura_TBL.DeMura_Adaptive_TBL.adaptiveScale_En);												
											//sizeByte = HAL_VPQ_DATA_EXTERN.length;
											CRC_nByte = demura_HEADER_tmp[DEMURA_TBL_HEADER_TBL_SIZE];	// CRC is not include last CRC
											crc_cal = VPQEX_VPQ_CalcCRC16(pBuff_tmp, CRC_nByte);
											crc_get = demura_HEADER_tmp[DEMURA_TBL_HEADER_TBL_CRC16];
			
											if (crc_get == crc_cal)
											{
												rtd_pr_vpq_info("VPQ_EXTERN_IOC_ACCESS_TCON_DEMURA_Para demura encode from AP, crc=%d, crc_get=%d,\n", 
													crc_cal, crc_get); 
											}
											else
											{
												rtd_pr_vpq_info("VPQ_EXTERN_IOC_ACCESS_TCON_DEMURA_Para demura encode from AP, crc compare fail, crc=%d, crc_get=%d,\n", 
													crc_cal, crc_get);					
												DeMura_TBL.DeMura_CTRL_TBL.demura_en = 0;
											}
											ret = 0;	
										}
		
									}
								}
								else
								{
									rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_DEMURA_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
									ret = -1;
								}							
							}					
							else 
							{
								rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_DEMURA_Para version error\n");
								ret = -1;
							}
						} 
						else
						{
							rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_DEMURA_Para carve out memory is not enough size = %llx\n", VIP_query_size);
							ret = -1;
						}
					}
					else if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Get)
					{
						if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_DEMURA_Encode_TBL_Size_Get) 
						{
							if (hal_VPQ_DATA_EXTERN.length == sizeof(int))
							{
								sizeByte = sizeof(SLR_VIP_TBL_HEADER_TBL_DeMura);
								if(copy_to_user(to_user_ptr(Data_addr), &sizeByte, hal_VPQ_DATA_EXTERN.length)) {
									rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_SET_AI_Para table copy fail\n");
									ret = -1;
								}
								else
								{
									rtd_pr_vpq_info("VPQ_EXTERN_IOC_ACCESS_TCON_DEMURA_Para demura structure size=%d,\n", sizeByte);						
									ret = 0;
								}
							}
							else
							{
								rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_DEMURA_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
								ret = -1;
							}					
						}
						else if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_DEMURA_Encode_TBL_Get) 
						{
							if (hal_VPQ_DATA_EXTERN.length == sizeof(SLR_VIP_TBL_HEADER_TBL_DeMura))
							{				
								pBuff_tmp = (unsigned char*)&DeMura_TBL;
								//pDemura_crc16 = (unsigned short*)(&DeMura_TBL.DeMura_Adaptive_TBL.adaptiveScale_En);
								CRC_nByte = sizeof(VIP_DeMura_TBL); 
								crc_cal = VPQEX_VPQ_CalcCRC16(pBuff_tmp, CRC_nByte);
								//DeMura_TBL.CRC16 = crc_cal;
								demura_HEADER_tmp[DEMURA_TBL_HEADER_TBL_CRC16] = crc_cal;
								demura_HEADER_tmp[DEMURA_TBL_HEADER_TBL_SIZE] = CRC_nByte;
		
								if(copy_to_user(to_user_ptr(Data_addr), &demura_HEADER_tmp[0], sizeof(demura_HEADER_tmp)))
								{
									rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_SET_AI_Para table copy fail 1\n");
									ret = -1;	
								}
								else
								{
									if(copy_to_user(to_user_ptr(Data_addr + sizeof(demura_HEADER_tmp)), pBuff_tmp, hal_VPQ_DATA_EXTERN.length-sizeof(demura_HEADER_tmp))) {
										rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_SET_AI_Para table copy fail 2\n");
										ret = -1;
									}
									else
									{
										rtd_pr_vpq_info("VPQ_EXTERN_IOC_ACCESS_TCON_DEMURA_Para demura encode to AP, crc=%d,CRC_nByte=%d,\n", 
											crc_cal, CRC_nByte);					
										ret = 0;
									}	
								}
							}
							else
							{
								rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_DEMURA_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
								ret = -1;
							}
						}					
						else
						{
							rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_DEMURA_Para get version error\n");
							ret = -1;
						}
					}
					else
					{
						rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_ACCESS_TCON_DEMURA_Para CMD error\n");
						ret = -1;
					}
				}
			}
			break;
#endif

		case VPQ_EXTERN_IOC_SET_Dynamic_Black_Equalize_Info:
		{
			unsigned int DEM_arg = 0;	//  
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_OverDriver_Para VPQ_EXTERN_IOC_SET_Dynamic_Black_Equalize_Info copy fail\n");
				ret = -1;
			} else {
				if (hal_VPQ_DATA_EXTERN.length != sizeof(int)) {
					rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_Dynamic_Black_Equalize_Info len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
					ret = -1;
				} else {
					Data_addr = hal_VPQ_DATA_EXTERN.pData;
					if(copy_from_user(&DEM_arg, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
					{
						rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_Dynamic_Black_Equalize_Info table copy fail\n");
						ret = -1;
					} else {
						ret = Scaler_set_APDEM(DEM_ARG_Dynamic_Black_Equalize_Gain, (void *)&DEM_arg);
					}
				}
			}	
		}		
		break;

		case VPQ_EXTERN_IOC_SET_Shadow_Detail_Info:
		{
			unsigned int DEM_arg[4] = {0};	//  
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_OverDriver_Para VPQ_EXTERN_IOC_SET_Shadow_Detail_Info copy fail\n");
				ret = -1;
			} else {
				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_Shadow_Detail_Info,version=%d,cmd=%d,length=%d,pData=%x,\n",
					hal_VPQ_DATA_EXTERN.version, hal_VPQ_DATA_EXTERN.wId, hal_VPQ_DATA_EXTERN.length, hal_VPQ_DATA_EXTERN.pData);
				
				Data_addr = hal_VPQ_DATA_EXTERN.pData;
				if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Set) 
				{
					if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_SET_Shadow_Detail_Info) 
					{
						if (hal_VPQ_DATA_EXTERN.length != sizeof(int)) {
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_Shadow_Detail_Info len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						} else {
							if(copy_from_user(&DEM_arg[0], (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
							{
								rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_Shadow_Detail_Info table copy fail\n");
								ret = -1;
							} else {
								ret = Scaler_set_APDEM(DEM_ARG_Shadow_detail_Gain, (void *)&DEM_arg[0]);
							}
						}
					}
					else if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_SET_Shadow_Boot_Info)
					{
						if (hal_VPQ_DATA_EXTERN.length != sizeof(int)*4) {
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_Shadow_Detail_Info len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						} else {
							if(copy_from_user(&DEM_arg[0], (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
							{
								rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_Shadow_Detail_Info table copy fail\n");
								ret = -1;
							} else {
								ret = Scaler_set_APDEM(DEM_ARG_Shadow_Boot_Offset, (void *)&DEM_arg[0]);
							}
						}
					}
					else
					{
						rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_Shadow_Detail_Info version error\n");
						ret = -1;
					}
				}
				else if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Get)
				{
					rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_Shadow_Detail_Info no get now\n");
					ret = -1;
				}
				else
				{
					rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_Shadow_Detail_Info CMD error\n");
					ret = -1;
				}
			}	
		}		
		break;

		case VPQ_EXTERN_IOC_SET_Gamma_byParameters:
		{
			unsigned int DEM_arg = 0;	//  
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_OverDriver_Para VPQ_EXTERN_IOC_SET_Gamma_byParameters copy fail\n");
				ret = -1;
			} else {
				Data_addr = hal_VPQ_DATA_EXTERN.pData;
				if ((hal_VPQ_DATA_EXTERN.length == sizeof(int)) && (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Set_B_CH) && 
					(hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_SET_Gamma_byParameters_Set_INV_OUT_GAMMA_byGain)) 
				{
					if(copy_from_user(&DEM_arg, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
					{
						rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_SET_Gamma_byParameters table copy fail\n");
						ret = -1;
					} else {
						ret = Scaler_set_APDEM(DEM_ARG_LowB_Gain, (void *)&DEM_arg);
					}
				} 
				else if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Set)
				{
					if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_SET_Gamma_byParameters_Set_INV_OUT_GAMMA)
					{
						if (hal_VPQ_DATA_EXTERN.length != sizeof(VIP_INI_Gamma_Curve))
						{
							rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_Shadow_Detail_Info len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						}
						else
						{
							if(copy_from_user(&AP_INI_Gamma_Curve, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
							{
								rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_SET_Gamma_byParameters table copy fail\n");
								ret = -1;
							} else {
								fwif_color_set_InvOutputGamma_System(0xFF);
							}					
						}
					}
					else
					{
						rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_Gamma_byParameters ver error ver=%d,\n", hal_VPQ_DATA_EXTERN.version);
						ret = -1;
					}				
				}
				else if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Get)
				{
					if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXT_IOC_SET_Gamma_byParameters_Set_INV_OUT_GAMMA)
					{
						if (hal_VPQ_DATA_EXTERN.length != sizeof(VIP_INI_Gamma_Curve))
						{
							rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_Gamma_byParameters len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						}
						else
						{
							if(copy_to_user(to_user_ptr(Data_addr), &AP_INI_Gamma_Curve, hal_VPQ_DATA_EXTERN.length)) {
								rtd_pr_vpq_emerg("VPQ_EXTERN_IOC_SET_Gamma_byParameters table copy fail\n");
								ret = -1;
							}
							else
							{
								rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_Gamma_byParameters inv gamma to ap, bit=%d,\n", AP_INI_Gamma_Curve.gamma_bit_num);					
								ret = 0;
							}
						}
					}
					else
					{
						rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_Gamma_byParameters ver error ver=%d,\n", hal_VPQ_DATA_EXTERN.version);
						ret = -1;
					}				
				}				
				else 
				{
					rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_Gamma_byParameters wId error len=%d,ver=%d,wId=%d,\n", 
						hal_VPQ_DATA_EXTERN.length, hal_VPQ_DATA_EXTERN.version, hal_VPQ_DATA_EXTERN.wId);
					ret = -1;				
				}
			}	
		}		
		break;

		case VPQ_EXTERN_IOC_Decontour_ByPara:
		{
			unsigned int DEM_arg = 0;
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_Decontour_ByPara hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} else {
				Data_addr = hal_VPQ_DATA_EXTERN.pData;

				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_Decontour_ByPara,version=%d,cmd=%d,length=%d,pData=%x,\n",
					hal_VPQ_DATA_EXTERN.version, hal_VPQ_DATA_EXTERN.wId, hal_VPQ_DATA_EXTERN.length, hal_VPQ_DATA_EXTERN.pData);
				
				if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Set) 
				{
					if (hal_VPQ_DATA_EXTERN.version == 0) 
					{
						if (hal_VPQ_DATA_EXTERN.length != sizeof(int)) 
						{
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_Decontour_ByPara len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						} 
						else 
						{
							if(copy_from_user(&DEM_arg, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
							{
								rtd_pr_vpq_err("VPQ_EXTERN_IOC_Decontour_ByPara table copy fail\n");
								ret = -1;
							} else {
								rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_Decontour_ByPara %d,\n", DEM_arg);
								ret = Scaler_set_APDEM(DEM_ARG_Smooth_Gradation, (void *)&DEM_arg);
							}
						}

					}
					else 
					{
						rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_Decontour_ByPara ver error ver=%d,\n", hal_VPQ_DATA_EXTERN.version);
						ret = -1;
					}
				}	
				else if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Get) 
				{
					rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_Decontour_ByPara NO get\n");
					ret = -1;				
				}
				else 
				{
					rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_Decontour_ByPara only get or set\n");
					ret = -1;
				}				
			}	
		}

		case VPQ_EXTERN_IOC_SET_AI_Face_Para:
		{
			unsigned int *pArg;
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_AI_Face_Para hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} else {
				if (hal_VPQ_DATA_EXTERN.length != sizeof(int)) {
					rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_AI_Face_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
					ret = -1;
				} else {
					Data_addr = hal_VPQ_DATA_EXTERN.pData;
					pArg = (unsigned int *)dvr_malloc_specific(hal_VPQ_DATA_EXTERN.length, GFP_DCU2_FIRST);
					if (pArg == NULL) {
						rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_AI_Face_Para alloc fail, len=%d,\n", hal_VPQ_DATA_EXTERN.length);
						ret = -1;
					} else {		
						if(copy_from_user(pArg, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
						{
							rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_AI_Face_Para table copy fail\n");
							ret = -1;
						} else {
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_AI_Face_Para %d,\n", pArg[0]);
						}
						dvr_free((void *)pArg);
					}
				}
			}	
		}		
		break;
		
		case VPQ_EXTERN_IOC_SET_AI_SQM_Para:
		{
			unsigned int *pArg;
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_AI_SQM_Para hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} else {
				if (hal_VPQ_DATA_EXTERN.length != sizeof(int)) {
					rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_AI_SQM_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
					ret = -1;
				} else {
					Data_addr = hal_VPQ_DATA_EXTERN.pData;
					pArg = (unsigned int *)dvr_malloc_specific(hal_VPQ_DATA_EXTERN.length, GFP_DCU2_FIRST);
					if (pArg == NULL) {
						rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_AI_SQM_Para alloc fail, len=%d,\n", hal_VPQ_DATA_EXTERN.length);
						ret = -1;
					} else {		
						if(copy_from_user(pArg, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
						{
							rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_AI_SQM_Para table copy fail\n");
							ret = -1;
						} else {
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_AI_SQM_Para %d,\n", pArg[0]);
						}
						dvr_free((void *)pArg);
					}
				}
			}	
		}
		break;
		
		case VPQ_EXTERN_IOC_SET_AI_Genre_Para:
		{
			unsigned int *pArg;
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_AI_Genre_Para hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} else {
				if (hal_VPQ_DATA_EXTERN.length != sizeof(int)) {
					rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_AI_Genre_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
					ret = -1;
				} else {
					Data_addr = hal_VPQ_DATA_EXTERN.pData;
					pArg = (unsigned int *)dvr_malloc_specific(hal_VPQ_DATA_EXTERN.length, GFP_DCU2_FIRST);
					if (pArg == NULL) {
						rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_AI_Genre_Para alloc fail, len=%d,\n", hal_VPQ_DATA_EXTERN.length);
						ret = -1;
					} else {		
						if(copy_from_user(pArg, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
						{
							rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_AI_Genre_Para table copy fail\n");
							ret = -1;
						} else {
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_AI_Genre_Para %d,\n", pArg[0]);
						}
						dvr_free((void *)pArg);
					}
				}
			}	
		}
		break;
		
		case VPQ_EXTERN_IOC_SET_AI_Content_Para:
		{
			unsigned int *pArg;
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_AI_Content_Para hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} else {
				if (hal_VPQ_DATA_EXTERN.length != sizeof(int)) {
					rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_AI_Content_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
					ret = -1;
				} else {
					Data_addr = hal_VPQ_DATA_EXTERN.pData;
					pArg = (unsigned int *)dvr_malloc_specific(hal_VPQ_DATA_EXTERN.length, GFP_DCU2_FIRST);
					if (pArg == NULL) {
						rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_AI_Content_Para alloc fail, len=%d,\n", hal_VPQ_DATA_EXTERN.length);
						ret = -1;
					} else {		
						if(copy_from_user(pArg, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
						{
							rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_AI_Content_Para table copy fail\n");
							ret = -1;
						} else {
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_AI_Content_Para %d,\n", pArg[0]);
						}
						dvr_free((void *)pArg);
					}
				}
			}	
		}
		break;
		
		case VPQ_EXTERN_IOC_SET_AI_Depth_Para:
		{
			unsigned int *pArg;
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_AI_Depth_Para hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} else {
				if (hal_VPQ_DATA_EXTERN.length != sizeof(int)) {
					rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_AI_Depth_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
					ret = -1;
				} else {
					Data_addr = hal_VPQ_DATA_EXTERN.pData;
					pArg = (unsigned int *)dvr_malloc_specific(hal_VPQ_DATA_EXTERN.length, GFP_DCU2_FIRST);
					if (pArg == NULL) {
						rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_AI_Depth_Para alloc fail, len=%d,\n", hal_VPQ_DATA_EXTERN.length);
						ret = -1;
					} else {		
						if(copy_from_user(pArg, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
						{
							rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_AI_Depth_Para table copy fail\n");
							ret = -1;
						} else {
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_AI_Depth_Para %d,\n", pArg[0]);
						}
						dvr_free((void *)pArg);
					}
				}
			}	
		}
		break;

		case VPQ_EXTERN_IOC_SET_AI_Semantic_Para:
		{
			unsigned int *pArg;
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_AI_Semantic_Para hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} else {
				if (hal_VPQ_DATA_EXTERN.length != sizeof(int)) {
					rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_AI_Semantic_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
					ret = -1;
				} else {
					Data_addr = hal_VPQ_DATA_EXTERN.pData;
					pArg = (unsigned int *)dvr_malloc_specific(hal_VPQ_DATA_EXTERN.length, GFP_DCU2_FIRST);
					if (pArg == NULL) {
						rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_AI_Semantic_Para alloc fail, len=%d,\n", hal_VPQ_DATA_EXTERN.length);
						ret = -1;
					} else {		
						if(copy_from_user(pArg, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
						{
							rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_AI_Semantic_Para table copy fail\n");
							ret = -1;
						} else {
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_AI_Semantic_Para %d,\n", pArg[0]);
						}
						dvr_free((void *)pArg);
					}
				}
			}	
		}
		break;

		case VPQ_EXTERN_IOC_SET_AI_Grain_Noise_Para:
		{
			unsigned int *pArg;
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_AI_Grain_Noise_Para hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} else {
				if (hal_VPQ_DATA_EXTERN.length != sizeof(int)) {
					rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_AI_Grain_Noise_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
					ret = -1;
				} else {
					Data_addr = hal_VPQ_DATA_EXTERN.pData;
					pArg = (unsigned int *)dvr_malloc_specific(hal_VPQ_DATA_EXTERN.length, GFP_DCU2_FIRST);
					if (pArg == NULL) {
						rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_AI_Grain_Noise_Para alloc fail, len=%d,\n", hal_VPQ_DATA_EXTERN.length);
						ret = -1;
					} else {		
						if(copy_from_user(pArg, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
						{
							rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_AI_Grain_Noise_Para table copy fail\n");
							ret = -1;
						} else {
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_AI_Grain_Noise_Para %d,\n", pArg[0]);
						}
						dvr_free((void *)pArg);
					}
				}
			}	
		}
		break;
		
		case VPQ_EXTERN_IOC_SET_DeFlick_Para:
		{
			unsigned int *pArg;
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_DeFlick_Para hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} else {
				if (hal_VPQ_DATA_EXTERN.length != sizeof(int)) {
					rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_DeFlick_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
					ret = -1;
				} else {
					Data_addr = hal_VPQ_DATA_EXTERN.pData;
					pArg = (unsigned int *)dvr_malloc_specific(hal_VPQ_DATA_EXTERN.length, GFP_DCU2_FIRST);
					if (pArg == NULL) {
						rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_DeFlick_Para alloc fail, len=%d,\n", hal_VPQ_DATA_EXTERN.length);
						ret = -1;
					} else {		
						if(copy_from_user(pArg, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
						{
							rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_DeFlick_Para table copy fail\n");
							ret = -1;
						} else {
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_DeFlick_Para %d,\n", pArg[0]);
						}
						dvr_free((void *)pArg);
					}
				}
			}	
		}
		break;
		
		case VPQ_EXTERN_IOC_SET_APDEM_PTG_Para:
		{
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_APDEM_PTG_Para hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} else {
				if (hal_VPQ_DATA_EXTERN.length != sizeof(VIP_APDEM_PTG_CTRL)) {
					rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_APDEM_PTG_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
					ret = -1;
				} else {
					Data_addr = hal_VPQ_DATA_EXTERN.pData;
					if(copy_from_user(&Hal_APDEM_PTG_CTRL, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
					{
						rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_APDEM_PTG_Para table copy fail\n");
						ret = -1;
					} else {
                        if(Hal_APDEM_PTG_CTRL.PTG_OnOff == 1 && Hal_APDEM_PTG_CTRL.PTG_Size == 100  ) {
                            IoReg_ClearBits(OSDOVL_Mixer_CTRL2_reg, OSDOVL_Mixer_CTRL2_mixer_en_mask);

                        } else {
                            IoReg_SetBits(OSDOVL_Mixer_CTRL2_reg, OSDOVL_Mixer_CTRL2_mixer_en_mask);
                        }
                        IoReg_SetBits(OSDOVL_OSD_Db_Ctrl_reg, OSDOVL_OSD_Db_Ctrl_db_load_mask);
						Scaler_APDEM_PTG_set(&Hal_APDEM_PTG_CTRL);
						rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_APDEM_PTG_Para PTG_OnOff=%d,PTG_R_val=%d,PTG_R_val=%d,\n", 
							Hal_APDEM_PTG_CTRL.PTG_OnOff, Hal_APDEM_PTG_CTRL.PTG_R_val, Hal_APDEM_PTG_CTRL.PTG_R_val);
					}
				}
			}	
		}
		break;
		
		case VPQ_EXTERN_IOC_SET_DCC_byParameters:
		{
			unsigned int DEM_arg[2] = {0};
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_DCC_byParameters hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} else {
				if (hal_VPQ_DATA_EXTERN.length != sizeof(int)*2) {
					rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_DCC_byParameters len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
					ret = -1;
				} else {
					Data_addr = hal_VPQ_DATA_EXTERN.pData;
					if(copy_from_user(&DEM_arg[0], (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
					{
						rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_DCC_byParameters table copy fail\n");
						ret = -1;
					} else {
						//Scaler_SetDCC_Mode(DEM_arg[1]&0xff);
						Scaler_update_DCC_Mode_Sel(DEM_arg[1]&0xff);
						ret = Scaler_set_APDEM(DEM_ARG_DCC_Idx, (void *)&DEM_arg[0]);
						rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_DCC_byParameters %d,%d,\n", DEM_arg[0], DEM_arg[1]);
					}
				}
			}	
		}
		break;
		
		case VPQ_EXTERN_IOC_SET_Local_Contrast_byParameters:
		{
			unsigned int DEM_arg[2] = {0};
			if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_Local_Contrast_byParameters hal_VPQ_DATA_EXTERN copy fail\n");
				ret = -1;
			} else {
				if (hal_VPQ_DATA_EXTERN.length != sizeof(int)*2) {
					rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_Local_Contrast_byParameters len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
					ret = -1;
				} else {
					Data_addr = hal_VPQ_DATA_EXTERN.pData;
					if(copy_from_user(&DEM_arg[0], (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
					{
						rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_Local_Contrast_byParameters table copy fail\n");
						ret = -1;
					} else {
						// for avoiding trasient noise,
						if((Scaler_DispGetInputInfoByDisp(SLR_MAIN_DISPLAY,SLR_INPUT_STATE) == _MODE_STATE_ACTIVE) &&
							(Scaler_APDEM_Arg_Access(DEM_ARG_LC_Idx, 0, 0) == DEM_arg[0]) && (Scaler_APDEM_Arg_Access(DEM_ARG_LC_level, 0, 0) == DEM_arg[1]))
						{
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_Local_Contrast_byParameters, same val skip, idx=%d,lv=%d,\n", DEM_arg[0], DEM_arg[1]);
							ret = 0;
						}
						else
						{							
							ret = Scaler_set_APDEM(DEM_ARG_LC_Idx, (void *)&DEM_arg[0]);
							ret = Scaler_set_APDEM(DEM_ARG_LC_level, (void *)&DEM_arg[1]);
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_Local_Contrast_byParameters %d,%d,\n", DEM_arg[0], DEM_arg[1]);
						}
					}
				}
			}	
		}	
		break;

		case VPQ_EXTERN_IOC_SET_HDR_FORCE_MODE:
		{
			unsigned int hdr_force_mode = 0;

			if (copy_from_user(&hdr_force_mode,  (int __user *)arg,sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_HDR_FORCE_MODE fail\n");
				ret = -1;
			} else {
				ret = 0;
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_CINEMA_PIC_MODE:
		{
#ifdef CONFIG_CINEMA_PQ_ENABLE
			unsigned int cinema_pic_mode = 0;
			if (copy_from_user(&cinema_pic_mode,  (int __user *)arg,sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_CINEMA_PIC_MODE fail\n");
				ret = -1;
			} else {
				rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_CINEMA_PIC_MODE \n");
				Scaler_Set_CinemaMode_PQ(cinema_pic_mode);
				ret = 0;
			}
#endif
		}
		break;

		case VPQ_EXTERN_IOC_SET_PQ_BYPASS_LV:
		{
			unsigned int pq_bypass_lv = 0;
			if (copy_from_user(&g_pq_bypass_lv_ctrl_by_AP,  (int __user *)arg,sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_PQ_BYPASS_LV fail\n");
				ret = -1;
			} else {
				rtd_pr_vpq_info("g_pq_bypass_lv_ctrl_by_AP = 0x%x,\n", g_pq_bypass_lv_ctrl_by_AP);				
				pq_bypass_lv = g_pq_bypass_lv_ctrl_by_AP&0x000000FF;
				//rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_PQ_BYPASS_LV \n");
				if (pq_bypass_lv < VIP_PQ_ByPass_TBL_Max)
					Scaler_color_Set_PQ_ByPass_Lv(pq_bypass_lv);
				ret = 0;
			}
		}
		break;

		case VPQ_EXTERN_IOC_GET_PQ_ENGMENU:
		{
			HAL_VPQ_ENG_STRUCT* ENGMENU = NULL;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_PQ_Engineer_menu\n");
			ENGMENU = Scaler_Get_ENGMENU();
			if (copy_to_user((void __user *)arg, (void *)ENGMENU, sizeof(HAL_VPQ_ENG_STRUCT))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_PQ_Engineer_menu fail\n");
				ret = -1;
			} else
				ret = 0;

		}
		break;

		case VPQ_EXTERN_IOC_GET_PQ_ENGMENU_size:
		{
			HAL_VPQ_ENG_SIZE* ENGMENU_size = NULL;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_PQ_Engineer_menu_size\n");
			ENGMENU_size = Scaler_Get_ENGMENU_size();
			if (copy_to_user((void __user *)arg, (void *)ENGMENU_size, sizeof(HAL_VPQ_ENG_SIZE))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_PQ_Engineer_menu_size fail\n");
				ret = -1;
			} else
				ret = 0;

		}
		break;

		case VPQ_EXTERN_IOC_GET_PQ_ENGMENU_item:
		{
			HAL_VPQ_ENG_STRUCT* ENGMENU = NULL;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_PQ_Engineer_menu\n");
			ENGMENU = Scaler_Get_ENGMENU();
			if (copy_to_user((void __user *)arg, (ENGMENU->item), sizeof(HAL_VPQ_ENG_ITEM_STRUCT)*HAL_VPQ_ENG_ITEM_MAX_NUM)) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_PQ_Engineer_menu fail\n");
				ret = -1;
			} else
				ret = 0;
		}
		break;

		case VPQ_EXTERN_IOC_GET_PQ_ENGMENU_ID:
		{
			HAL_VPQ_ENG_STRUCT* ENGMENU = NULL;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_PQ_Engineer_menu\n");
			ENGMENU = Scaler_Get_ENGMENU();
			if (copy_to_user((void __user *)arg, (ENGMENU->ID), sizeof(HAL_VPQ_ENG_ID_STRUCT)*HAL_VPQ_ENG_ID_MAX_NUM)) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_PQ_Engineer_menu fail\n");
				ret = -1;
			} else
				ret = 0;
		}
		break;

		case VPQ_EXTERN_IOC_GET_PQ_ENG_ID:
		{
			HAL_VPQ_ENG_ID ENG_ID;
			if (copy_from_user(&ENG_ID,  (int __user *)arg,sizeof(HAL_VPQ_ENG_ID))) {
				ret = -1;
			} else {
				ret = 0;
				ENG_ID.ID_value= Scaler_Get_ENGMENU_ID((unsigned int)ENG_ID.ID);
			}
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_PQ_ENG_ID\n");
			if (copy_to_user((void __user *)arg, (void *)(&ENG_ID), sizeof(HAL_VPQ_ENG_ID))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_PQ_ENG_ID fail\n");
				ret = -1;
			} else
				ret = 0;

		}
		break;

		case VPQ_EXTERN_IOC_SET_PQ_ENG_ID:
		{
			HAL_VPQ_ENG_ID ENG_ID;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_PQ_ENG_ID\n");
			if (copy_from_user(&ENG_ID,  (int __user *)arg,sizeof(HAL_VPQ_ENG_ID))) {
				ret = -1;
			} else {
				ret = 0;
				Scaler_Set_ENGMENU_ID(ENG_ID.ID, ENG_ID.ID_value);
			}
		}
		break;

		case VPQ_EXTERN_IOC_GET_PQ_ENG_TWOLAYER_MENU_item:
		{
			HAL_VPQ_ENG_TWOLAYER_STRUCT* ENG_TWOLAYER_MENU = NULL;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_PQ_Engineer_TWOLAYER_menu\n");
			ENG_TWOLAYER_MENU = Scaler_Get_ENG_TWOLAYER_MENU();
			if (copy_to_user((void __user *)arg, (ENG_TWOLAYER_MENU->item), sizeof(HAL_VPQ_ENG_ITEM_TWOLAYER_STRUCT)*HAL_VPQ_ENG_ITEM_MAX_NUM)) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_PQ_Engineer_menu fail\n");
				ret = -1;
			} else
				ret = 0;
		}
		break;

		case VPQ_EXTERN_IOC_GET_PQ_ENG_TWOLAYER_MENU_ID:
		{
			HAL_VPQ_ENG_TWOLAYER_STRUCT* ENG_TWOLAYER_MENU = NULL;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_PQ_Engineer_TWOLAYER_menu\n");
			ENG_TWOLAYER_MENU = Scaler_Get_ENG_TWOLAYER_MENU();
			if (copy_to_user((void __user *)arg, (ENG_TWOLAYER_MENU->ID), sizeof(HAL_VPQ_ENG_ID_STRUCT)*HAL_VPQ_ENG_ID_MAX_NUM)) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_PQ_Engineer_TWOLAYER_menu fail\n");
				ret = -1;
			} else
				ret = 0;
		}
		break;

		case VPQ_EXTERN_IOC_GET_PQ_ENG_TWOLAYER_ID:
		{
			HAL_VPQ_ENG_TWOLAYER_ID ENG_TWOLAYER_ID;
			if (copy_from_user(&ENG_TWOLAYER_ID,  (int __user *)arg,sizeof(HAL_VPQ_ENG_TWOLAYER_ID))) {
				ret = -1;
			} else {
				ret = 0;
				ENG_TWOLAYER_ID.ID_value= Scaler_Get_ENG_TWOLAYER_MENU_ID((unsigned int)ENG_TWOLAYER_ID.ITEM,(unsigned int)ENG_TWOLAYER_ID.ID);
			}
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_GET_PQ_TWOLAYER_ENG_ID\n");
			if (copy_to_user((void __user *)arg, (void *)(&ENG_TWOLAYER_ID), sizeof(HAL_VPQ_ENG_TWOLAYER_ID))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_PQ_ENG_TWOLAYER_ID fail\n");
				ret = -1;
			} else
				ret = 0;

		}
		break;

		case VPQ_EXTERN_IOC_SET_PQ_ENG_TWOLAYER_ID:
		{
			HAL_VPQ_ENG_TWOLAYER_ID ENG_TWOLAYER_ID;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_PQ_ENG_TWOLAYER_ID\n");
			if (copy_from_user(&ENG_TWOLAYER_ID,  (int __user *)arg,sizeof(HAL_VPQ_ENG_TWOLAYER_ID))) {
				ret = -1;
			} else {
				ret = 0;
				Scaler_Set_ENG_TWOLAYER_MENU_ID(ENG_TWOLAYER_ID.ITEM,ENG_TWOLAYER_ID.ID, ENG_TWOLAYER_ID.ID_value);
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_TV002_EXT_FUN_EN:
		{
			unsigned int iEn;
			if (copy_from_user(&iEn,  (void __user *)arg,sizeof(unsigned int))) {
				ret = -1;
			} else {
				g_tv002_demo_fun_flag = iEn&0xff;
				scalerVIP_access_tv002_style_demo_flag(1, &g_tv002_demo_fun_flag);
				ret = 0;
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_TV002_BLE:
		{
			int level;
			if (copy_from_user(&level,  (void __user *)arg,sizeof(int))) {
				ret = -1;
			} else {
				Scaler_set_BLE(level&0xff);
				ret = 0;
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_TV002_SGAMMA:
		{
			int level;
			if (copy_from_user(&level,  (void __user *)arg,sizeof(int))) {
				ret = -1;
			} else {
				Scaler_set_sGamma(level&0xff);
				ret = 0;
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_TV002_MAGIC_GAMMA:
		{

			if (copy_from_user(&data,  (void __user *)arg,sizeof(VIP_MAGIC_GAMMA_Curve_Driver_Data))) {
				ret = -1;
			} else {
				memcpy(&g_MagicGammaDriverDataSave, &data, sizeof(VIP_MAGIC_GAMMA_Curve_Driver_Data));
				fwif_color_set_gamma_from_MagicGamma_MiddleWare();
				ret = 0;
			}
		}
		break;

		case VPQ_EXTERN_IOC_GET_TV002_SR:
		{
			RTK_TV002_SR_T data;
			data.mode = Scaler_get_Clarity_RealityCreation();
			data.value = Scaler_get_Clarity_Resolution();
			if (copy_to_user((void __user *)arg, (void *)&data, sizeof(RTK_TV002_SR_T))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_TV002_SR fail\n");
				ret = -1;
			} else
				ret = 0;
		}
		break;

		case VPQ_EXTERN_IOC_SET_TV002_SR:
		{
			RTK_TV002_SR_T data;
			if (copy_from_user(&data,  (void __user *)arg,sizeof(RTK_TV002_SR_T))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_TV002_SR fail\n");
				ret = -1;
			} else {
				Scaler_set_Clarity_RealityCreation(data.mode);
				Scaler_set_Clarity_Resolution(data.value);
				fwif_color_set_clarity_resolution();
				ret = 0;
			}
		}
		break;

		case VPQ_EXTERN_IOC_GET_TV002_LIVE_COLOUR:
		{
			unsigned int live_colour = Scaler_get_LiveColor();
			if (copy_to_user((void __user *)arg, (void *)&live_colour, sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_TV002_LIVE_COLOUR fail\n");
				ret = -1;
			} else {
				ret = 0;
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_TV002_LIVE_COLOUR:
		{
			unsigned int live_colour;
			if (copy_from_user(&live_colour,  (void __user *)arg,sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_TV002_LIVE_COLOUR fail\n");
				ret = -1;
			} else {
				Scaler_set_LiveColor(live_colour);
				ret = 0;
			}
		}
		break;

		case VPQ_EXTERN_IOC_GET_TV002_MASTERED_4K:
		{
			unsigned int mastered_4k = Scaler_get_Clarity_Mastered4K();
			if (copy_to_user((void __user *)arg, (void *)&mastered_4k, sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_TV002_MASTERED_4K fail\n");
				ret = -1;
			} else {
				ret = 0;
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_TV002_MASTERED_4K:
		{
			unsigned int mastered_4k;
			if (copy_from_user(&mastered_4k, (void __user *)arg, sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_TV002_MASTERED_4K fail\n");
				ret = -1;
			} else {
				Scaler_set_Clarity_Mastered4K(mastered_4k);
				fwif_color_set_mastered_4k();
				ret = 0;
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_TV002_PROJECT_STYLE:
		{
			unsigned char TV002_PROJECT_STYLE;
			if (copy_from_user(&TV002_PROJECT_STYLE, (void __user *)arg, sizeof(unsigned char))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_TV002_PROJECT_STYLE fail\n");
				ret = -1;
			} else {
				Scaler_access_Project_TV002_Style(1, TV002_PROJECT_STYLE);
				ret = 0;
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_TV002_DBC_POWER_SAVING_MODE:
		{
			unsigned char TV002_DBC_POWER_SAVING_MODE;
			if (copy_from_user(&TV002_DBC_POWER_SAVING_MODE, (void __user *)arg, sizeof(unsigned char))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_TV002_DBC_POWER_SAVING_MODE fail\n");
				ret = -1;
			} else {
				Scaler_set_DBC_POWER_Saving_Mode_TV002(TV002_DBC_POWER_SAVING_MODE);
				ret = 0;
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_TV002_DBC_UI_BACKLIGHT:
		{
			unsigned char TV002_DBC_UI_BACKLIGHT;
			if (copy_from_user(&TV002_DBC_UI_BACKLIGHT, (void __user *)arg, sizeof(unsigned char))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_TV002_DBC_UI_BACKLIGHT fail\n");
				ret = -1;
			} else {
				Scaler_set_DBC_UI_blacklight_Value_TV002(TV002_DBC_UI_BACKLIGHT);
				ret = 0;
			}
		}
		break;

		case VPQ_EXTERN_IOC_GET_TV002_DBC_BACKLIGHT:
		{
			unsigned char TV002_DBC_BACKLIGHT = Scaler_get_DBC_blacklight_Value_TV002();
			if (copy_to_user((void __user *)arg, (void *)&TV002_DBC_BACKLIGHT, sizeof(unsigned char))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_TV002_DBC_BACKLIGHT fail\n");
				ret = -1;
			} else {
				ret = 0;
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_TV002_UI_PICTURE_MODE:
		{
			unsigned char TV002_UI_PICTURE_MODE;
			if (copy_from_user(&TV002_UI_PICTURE_MODE, (void __user *)arg, sizeof(unsigned char))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_TV002_UI_PICTURE_MODE fail\n");
				ret = -1;
			} else {
				Scaler_set_UI_Picture_Mode_TV002(TV002_UI_PICTURE_MODE);
				ret = 0;
			}
		}
		break;

	case VPQ_EXTERN_IOC_ACCESS_VPQ_byPara:
	{
		unsigned int VPQ_byPara_arg = 0;	//	
		if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
			rtd_pr_vpq_err("VPQ_EXTERN_IOC_ACCESS_VPQ_byPara copy fail\n");
			ret = -1;
		} else {
				Data_addr = hal_VPQ_DATA_EXTERN.pData;				
				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_ACCESS_VPQ_byPara,version=%d,cmd=%d,length=%d,pData=%x,\n",
					hal_VPQ_DATA_EXTERN.version, hal_VPQ_DATA_EXTERN.wId, hal_VPQ_DATA_EXTERN.length, hal_VPQ_DATA_EXTERN.pData);
				if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Set)
				{
					if (hal_VPQ_DATA_EXTERN.version == VPQ_DEV_EXTERN_ACCESS_VPQ_byPara_RTNR_CMA_Mode) 
					{
						if (hal_VPQ_DATA_EXTERN.length != sizeof(int)) {
							rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_ACCESS_VPQ_byPara len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
							ret = -1;
						} else {
							if(copy_from_user(&VPQ_byPara_arg, (int __user *)Data_addr, hal_VPQ_DATA_EXTERN.length))
							{
								rtd_pr_vpq_err("VPQ_EXTERN_IOC_ACCESS_VPQ_byPara table copy fail\n");
								ret = -1;
							} else {
								//fwif_color_RTNR_CMA_Size_AP_mode_update(VPQ_byPara_arg);								
								rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_ACCESS_VPQ_byPara VPQ_byPara_arg=%d, not support\n", VPQ_byPara_arg);
							}
						}
					}
					else
					{
						rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_Shadow_Detail_Info version error\n");
						ret = -1;
					}
				}
				else if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Get)
				{
					rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_Shadow_Detail_Info no get now\n");
					ret = -1;
				}
				else if (hal_VPQ_DATA_EXTERN.wId == VPQ_DEV_EXTERN_VPQ_DATA_EXTERN_CMD_Set_Sub)
				{
					rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_Shadow_Detail_Info no sub set now\n");
					ret = -1;
				}				
				else
				{
					rtd_pr_vpq_emerg("kernel VPQ_EXTERN_IOC_SET_Shadow_Detail_Info CMD error\n");
					ret = -1;
				}
		}	
	}		
			break;

		case VPQ_EXTERN_IOC_SET_PANORAMA_TYPE:
		{
			unsigned int args = 0;
			if (copy_from_user(&args,  (int __user *)arg,sizeof(unsigned int))) {
				ret = -1;
			} else {
				rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_PANORAMA_TYPE\n");
			
				//drvif_color_ultrazoom_config_scaling_up(Scaler_DispCheckRatio(SLR_RATIO_PANORAMA));
				Check_smooth_toggle_update_flag((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY));
				zoom_update_scaler_info_from_vo_smooth_toggle((SCALER_DISP_CHANNEL)Scaler_DispGetInputInfo(SLR_INPUT_DISPLAY), 0);
				drvif_color_ultrazoom_config_scaling_up((TV030_RATIO_TYPE_E)args);
				IoReg_SetBits(SCALEUP_DM_UZU_DB_CTRL_reg, SCALEUP_DM_UZU_DB_CTRL_db_apply_mask);
				//Scaler_SetPanoramaType((TV030_RATIO_TYPE_E)args);
				if(args)
					Scaler_DispSetRatioMode(SLR_RATIO_PANORAMA);
				else
					Scaler_DispSetRatioMode(SLR_RATIO_CUSTOM);

				//imd_smooth_main_double_buffer_apply(ZOOM_UZU);//mac7p pq compile fix
				ret = 0;
			}
			rtd_pr_vpq_err("=======VPQ_EXTERN_IOC_SET_PANORAMA_TYPE ret= %d args=%d \n", ret,args);
		}
		break;
		case VPQ_EXTERN_IOC_SET_WCG_MODE:
		{	
			unsigned int wcgmode_tmp;
			rtd_pr_vpq_info("[WCG] VPQ_EXTERN_IOC_SET_WCG_MODE\n");

			if (copy_from_user(&wcgmode_tmp, (int __user *)arg, sizeof(unsigned int))) {
				rtd_pr_vpq_info("kernel copy VPQ_EXTERN_IOC_SET_WCG_MODE fail\n");
				ret = -1;
				break;
			} else {
				rtd_pr_vpq_info("kernel copy VPQ_EXTERN_IOC_SET_WCG_MODE success WCG_mode[%d]\n", wcgmode_tmp);

				Scaler_Set_WCG_mode(wcgmode_tmp);
				ret = 0;
			}
		}
		break;

		case VPQ_EXTERN_IOC_GET_GDBC_MODE:
		{
			unsigned int dcr_mode = 0;
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_PQ_CMD_GET_GDBC_MODE\n");
			dcr_mode = (unsigned int)Scaler_GetDcrMode();
			if (copy_to_user((void __user *)arg, (void *)&dcr_mode, sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_DCC_MODE fail\n");
				ret = -1;
			} else
				ret = 0;

		}
		break;

		case VPQ_EXTERN_IOC_SET_GDBC_MODE:
		{

			unsigned int dcr_mode = 0;
			rtd_pr_vpq_notice("VPQ_EXTERN_IOC_PQ_CMD_SET_GDBC_MODE\n");
			
			if (copy_from_user(&dcr_mode,  (int __user *)arg,sizeof(unsigned int))) {
				ret = -1;
			} else {
				ret = 0;
			
				Scaler_SetDcrMode(dcr_mode&0xff);
			}
		}
		break;

		case VPQ_EXTERN_IOC_GET_DCR_BACKLIGHT:
		{
			//rtd_pr_vpq_notice("VPQ_EXTERN_IOC_GET_DCR_BACKLIGHT\n");
			unsigned int DCR_backlight = 0;
			DCR_backlight = (unsigned int)Scaler_Get_DCR_Backlight();

			if (copy_to_user((void __user *)arg, (void *)&DCR_backlight, sizeof(unsigned int))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_BACKLIGHT fail\n");
				ret = -1;
			} else
				ret = 0;
		}
		break;

		case VPQ_EXTERN_IOC_SET_OUTGAMMA_SCURVE:
		{
			unsigned int outgamma_scurve_en = 0;

			if (copy_from_user(&outgamma_scurve_en,  (int __user *)arg, sizeof(unsigned int))) {
				ret = -1;
			} else {
				rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_OUTGAMMA_SCURVE\n");
				Scaler_Set_OutGamma_SCurve(outgamma_scurve_en);
				ret = 0;
			}
		}
		break;

		case VPQ_EXTERN_IOC_PQ_CMD_Sync_PQ_table_bin_ENABLE:
		{
			unsigned int args = 0;

			rtd_pr_vpq_info("VPQ_EXTERN_IOC_PQ_CMD_Sync_PQ_table_bin_ENABLE\n");
			if 	(copy_from_user((void *)&args, (const void __user *)arg, sizeof(unsigned char)))
			{
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_PQ_CMD_Sync_PQ_table_bin_ENABLE fail\n");
				ret = -1;
			} else {
				ret = 0;
				Scaler_Set_PQ_table_bin_En((unsigned char)args);
			}
		}
		break;

		case VPQ_EXTERN_IOC_GET_Data_Access:
		{
			RTK_Data_Access_T data;
			if (copy_from_user(&data,  (void __user *)arg,sizeof(RTK_Data_Access_T))) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_Data_Access fail\n");
				ret = -1;
			} else {
				if(Scaler_Get_Data_Access(data.type, data.index, data.YUV)==0)
				{
					if (copy_to_user((void __user *)arg, (void *)&data, sizeof(RTK_Data_Access_T))) {
						rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_Data_Access fail\n");
						ret = -1;
					} else
						ret = 0;
				}else{
					ret = -1;
				}
			}
		}
		break;

		case VPQ_EXTERN_IOC_SET_DCL_W:
		{
			unsigned char src_idx = 0;
			unsigned char level = 0;
			unsigned char display = 0;
			int i = 0;
			SLR_VIP_TABLE *vipTable_ShareMem = fwif_color_GetShare_Memory_VIP_TABLE_Struct();
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_PQ_CMD_SET_DCL_W\n");

			if (copy_from_user(&level,  (int __user *)arg,sizeof(unsigned char))) {
				rtd_pr_vpq_emerg( "kernel VPQ_EXTERN_IOC_PQ_CMD_SET_DCL_W fail\n");
				ret = -1;
			} else {
				rtd_pr_vpq_info("VPQ_EXTERN_IOC_PQ_CMD_SET_DCL_W, level = %d\n", level);
				ret = 0;
				Scaler_Get_Display_info(&display, &src_idx);
				fwif_color_set_DCL_W(src_idx, level);
				for(i = 0; i < DCC_Level_and_Blend_Coef_Table_MAX; ++i)
					vipTable_ShareMem->DCC_Control_Structure.DCC_Level_and_Blend_Coef_Table[i].DCL_W_level = level;
			}
		}
		break;
		case VPQ_EXTERN_IOC_SET_DCL_B:
		{
			unsigned char src_idx = 0;
			unsigned char level = 0;
			unsigned char display = 0;
			int i = 0;
			SLR_VIP_TABLE *vipTable_ShareMem = fwif_color_GetShare_Memory_VIP_TABLE_Struct();
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_PQ_CMD_SET_DCL_B\n");

			if (copy_from_user(&level,  (int __user *)arg,sizeof(unsigned char))) {
				rtd_pr_vpq_emerg( "kernel VPQ_EXTERN_IOC_PQ_CMD_SET_DCL_B fail\n");
				ret = -1;
			} else {
				rtd_pr_vpq_info("VPQ_EXTERN_IOC_PQ_CMD_SET_DCL_B, level = %d\n", level);
				ret = 0;
				Scaler_Get_Display_info(&display, &src_idx);
				fwif_color_set_DCL_B(src_idx, level);
				for(i = 0; i < DCC_Level_and_Blend_Coef_Table_MAX; ++i)
					vipTable_ShareMem->DCC_Control_Structure.DCC_Level_and_Blend_Coef_Table[i].DCL_B_level = level;
			}
		}
		break;

		case VPQ_EXTERN_IOC_GET_HISTODATA_APL:
		{
			unsigned int DC_APL = 0;

			if (fwif_color_get_HistoData_APL(&DC_APL) == FALSE) {
				rtd_pr_vpq_err("kernel fwif_color_get_HistoData_APL fail\n");
				ret = -1;
			} else {
				if (drvif_color_get_WB_pattern_on())
					DC_APL = 940;
				if (copy_to_user((void __user *)arg, (void *)&DC_APL, sizeof(unsigned int))) {
					rtd_pr_vpq_err("kernel VPQ_IOC_GET_HistoData_APL fail\n");
					ret = -1;
				} else
					ret = 0;
			}
		}
		break;

	case VPQ_EXTERN_IOC_SET_COLOR_TEMP_LEVEL:
	{
		unsigned int level = 0;
		rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_COLOR_TEMP_LEVEL\n");

		if (copy_from_user(&level,  (int __user *)arg,sizeof(unsigned int))) {
			rtd_pr_vpq_emerg( "kernel VPQ_EXTERN_IOC_SET_COLOR_TEMP_LEVEL fail\n");
			ret = -1;
		} else {
			ret = 0;
			scaler_set_color_temp_level(level);
		}
	}
	break;
	case VPQ_EXTERN_IOC_SET_GAMMA_LEVEL_001:
	{
		unsigned int level = 0;
		rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_GAMMA_LEVEL_001\n");

		if (copy_from_user(&level,  (int __user *)arg,sizeof(unsigned int))) {
			rtd_pr_vpq_emerg( "kernel VPQ_EXTERN_IOC_SET_GAMMA_LEVEL_001 fail\n");
			ret = -1;
		} else {
			ret = 0;
			scaler_set_color_temp_level(level);
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_Panel_Dither:
	{
		HAL_VPQ_Panel_Dither_T Panel_Dither_Info;
		if (copy_from_user(&Panel_Dither_Info, (int __user *)arg, sizeof(HAL_VPQ_Panel_Dither_T))) {
			rtd_pr_vpq_err("kernel copy VPQ_IOC_SET_DeMura fail\n");
			ret = -1;
			break;
		} else {
			rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_Panel_Dither \n");
			fwif_color_set_Panel_Dither(Panel_Dither_Info.Enable, (VIP_PANEL_BIT)Panel_Dither_Info.default_vip_panel_bit, Panel_Dither_Info.nTableType);
			ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_TCON_ENABLE_CTRL:
	{
		//**// 1. od enable; 2,3,4 :reseved
		unsigned char Enable_Ctrl_OD_Demura_VALC_Outputgamma[4] = {0,0,0,0,};
		if (copy_from_user(&Enable_Ctrl_OD_Demura_VALC_Outputgamma, (int __user *)arg, sizeof(unsigned char) * 4)) {
			rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_TCON_ENABLE_CTRL fail\n");
			ret = -1;
			break;
		} else {
			rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_TCON_ENABLE_CTRL success\n");
			//rtd_pr_vpq_err("kernel copy Enable_Ctrl_OD_Demura_VALC_Outputgamma[0,1,2,3] = %d, %d, %d, %d\n", Enable_Ctrl_OD_Demura_VALC_Outputgamma[0], Enable_Ctrl_OD_Demura_VALC_Outputgamma[1], Enable_Ctrl_OD_Demura_VALC_Outputgamma[2], Enable_Ctrl_OD_Demura_VALC_Outputgamma[3]);
			fwif_color_set_od(Enable_Ctrl_OD_Demura_VALC_Outputgamma[0]);
			ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_OD_SETTING_TABLE:
	{
		if (copy_from_user(&m_pOD_setting_table, (int __user *)arg, sizeof(DRV_od_table_mode_t) * VIP_QUALITY_SOURCE_NUM)) {
			rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_OD_SETTING_TABLE fail\n");
			ret = -1;
			break;
		} else {
			rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_OD_SETTING_TABLE success\n");

			fwif_set_od_setting_table(m_pOD_setting_table);
			ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_OD_TABLE:
	{
		if (copy_from_user(&m_pOD_table, (int __user *)arg, sizeof(DRV_od_table_t))) {
			rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_OD_TABLE fail\n");
			ret = -1;
			break;
		} else {
			rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_OD_TABLE success\n");

			fwif_set_od_table(&m_pOD_table);
			ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_DeMura:
	{
		if (copy_from_user(&m_pDeMura_TBL, (int __user *)arg, sizeof(VIP_DeMura_TBL))) {
			rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_DeMura fail\n");
			ret = -1;
			break;
		} else {

			rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_DeMura success\n");
			fwif_set_DeMura(&m_pDeMura_TBL);
			ret = 0;
		}
	}
	break;


	case VPQ_EXTERN_IOC_SET_VALC:
	{
		if (copy_from_user(&m_pVALC_Table, (int __user *)arg, sizeof(unsigned char) * 6 * 17 * 17)) {
			rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_VALC fail\n");
			ret = -1;
			break;
		} else {
		
			rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_VALC success\n");
			fwif_set_VALC(m_pVALC_Table);
			ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_VALCprotectSetting:
	{
		if (copy_from_user(&m_pVALCprotectSetting, (int __user *)arg, sizeof(DRV_pcid_valc_t) * 2)) {
			rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_VALCprotectSetting fail\n");
			ret = -1;
			break;
		} else {
			rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_VALCprotectSetting success\n");
			fwif_set_VALCprotectSetting(m_pVALCprotectSetting);
			ret = 0;
		}
	}
	break;


	case VPQ_EXTERN_IOC_SET_LEDOutGamma:
	{
		if (copy_from_user(&m_pLEDOutGamma, (int __user *)arg, 257 * sizeof(unsigned short))) {
			rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_LEDOutGamma fail\n");
			ret = -1;
			break;
		} else {
		
			rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_LEDOutGamma success\n");
			fwif_set_LEDOutGamma(m_pLEDOutGamma);
			ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_Output_InvOutput_Gamma:
	{
		if (copy_from_user(&m_pOutput_InvOutput_Gamma, (int __user *)arg, sizeof(VIP_Output_InvOutput_Gamma) * Output_InvOutput_Gamma_TBL_MAX)) {
			rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_Output_InvOutput_Gamma fail\n");
			ret = -1;
			break;
		} else {

			rtd_pr_vpq_debug("kernel copy VPQ_EXTERN_IOC_SET_Output_InvOutput_Gamma success\n");
			fwif_set_Output_InvOutput_Gamma(m_pOutput_InvOutput_Gamma);
			ret = 0;
		}
	}
	break;
	case VPQ_EXTERN_IOC_SET_COLORTEMP_AFTER_GAMMA:
	{
		TV001_COLOR_TEMP_DATA_S color_temp_data_s;
		rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_COLORTEMP_AFTER_GAMMA\n");

		if (copy_from_user(&color_temp_data_s,	(int __user *)arg,sizeof(TV001_COLOR_TEMP_DATA_S))) {

			ret = -1;
		} else {
			ret = 0;
			fwif_color_set_color_temp_tv043(TRUE, color_temp_data_s.redGain, color_temp_data_s.greenGain, color_temp_data_s.blueGain, color_temp_data_s.redOffset, color_temp_data_s.greenOffset, color_temp_data_s.blueOffset);
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_SLR_HDR_TABLE:
	{
		unsigned int hdr_table_size = 0;

		// if this cmd success, will call Scaler_color_copy_HDR_table_from_AP() and get current EOTF?OETF table from AP panel luminance
		// if this cmd fail, we will call Scaler_color_Set_EOTF_OETF_Table to get current EOTF?OETF table from .c panel luminance

		if (copy_from_user(&hdr_table_size, (void __user *)arg, sizeof(unsigned int))) { //size check
			rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_SLR_HDR_TABLE fail\n");
		} else {
				rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_SLR_HDR_TABLE struct size, AP=%d, SLR_HDR_TABLE=%zu, SLR_HDR_TABLE_v4=%zu!!\n", hdr_table_size, sizeof(SLR_HDR_TABLE), sizeof(SLR_HDR_TABLE_v4));
			if ((hdr_table_size != sizeof(SLR_HDR_TABLE)) && (hdr_table_size != sizeof(SLR_HDR_TABLE_v4))) {
				rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_SLR_HDR_TABLE struct size miss match.\n");
				ret = -1;
			} else {
				if(hdr_table_size == sizeof(SLR_HDR_TABLE)){
					Scaler_color_set_HDRver(3);
					
					ptr_HDR_bin = (SLR_HDR_TABLE*)dvr_malloc_specific(sizeof(SLR_HDR_TABLE), GFP_DCU2_FIRST);
					if(ptr_HDR_bin == NULL){
						rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_SLR_HDR_TABLE ptr_HDR_bin = NULL\n");
						ret = -1;
					} else{
						if (copy_from_user(ptr_HDR_bin, (void __user *)arg, sizeof(SLR_HDR_TABLE))) {
							rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_SLR_HDR_TABLE v3 fail\n");
							ret = -1;
						} else {
							rtd_pr_vpq_info("kernel copy VPQ_EXTERN_IOC_SET_SLR_HDR_TABLE v3 success\n");
							Scaler_color_copy_HDR_table_from_AP(ptr_HDR_bin);
							ret = 0;
						}
					}
				} else if(hdr_table_size == sizeof(SLR_HDR_TABLE_v4)){
					Scaler_color_set_HDRver(4);

					ptr_HDR_bin_v4 = (SLR_HDR_TABLE_v4*)dvr_malloc_specific(sizeof(SLR_HDR_TABLE_v4), GFP_DCU2_FIRST);
					if(ptr_HDR_bin_v4 == NULL){
						rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_SLR_HDR_TABLE ptr_HDR_bin_v4 = NULL\n");
						ret = -1;
					} else{
						if (copy_from_user(ptr_HDR_bin_v4, (void __user *)arg, sizeof(SLR_HDR_TABLE_v4))) {
							rtd_pr_vpq_err("kernel copy VPQ_EXTERN_IOC_SET_SLR_HDR_TABLE v4 fail\n");
							ret = -1;
						} else {
							rtd_pr_vpq_info("kernel copy VPQ_EXTERN_IOC_SET_SLR_HDR_TABLE v4 success\n");
							Scaler_color_copy_HDR_table_from_AP_v4(ptr_HDR_bin_v4);
							ret = 0;
						}
					}
				}
			}
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_SLR_HDR_TABLE_PANEL_LUMINANCE:
	{
		unsigned short panel_luminance_from_AP = 0;
		rtd_pr_vpq_info("VPQ_EXTERN_IOC_SET_SLR_HDR_TABLE_PANEL_LUMINANCE\n");
		if (copy_from_user(&panel_luminance_from_AP,  (void __user *)arg, sizeof(unsigned short))) {
			rtd_pr_vpq_info(TAG_NAME, "kernel VPQ_EXTERN_IOC_SET_SLR_HDR_TABLE_PANEL_LUMINANCE fail\n");
			ret = -1;
		} else {
			//rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_SLR_HDR_TABLE_PANEL_LUMINANCE success. PANEL_LUMINANCE=%d\n", panel_luminance_from_AP);
			//Scaler_color_copy_HDR_table_panel_luminance_from_AP(panel_luminance_from_AP);
			ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_SLR_HDR_TABLE_OETF_Setting_Ctrl:
	{
		unsigned short oetf_setting_ctrl_from_AP = 0;
		if (copy_from_user(&oetf_setting_ctrl_from_AP,  (void __user *)arg, sizeof(unsigned short))) {
			rtd_pr_vpq_info(TAG_NAME, "kernel VPQ_EXTERN_IOC_SET_SLR_HDR_TABLE_OETF_Setting_Ctrl fail\n");
			ret = -1;
		} else {
			rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_SLR_HDR_TABLE_OETF_Setting_Ctrl success. enable=%d\n", oetf_setting_ctrl_from_AP);
			ret = Scaler_set_APDEM(DEM_ARG_HDR_OETF_Setting_Ctrl, (void *)&oetf_setting_ctrl_from_AP);
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_SLR_HDR_TABLE_OETF_Curve_Ctrl:
	{
		unsigned short oetf_curve_index_from_AP = 0;
		if (copy_from_user(&oetf_curve_index_from_AP,  (void __user *)arg, sizeof(unsigned short))) {
			rtd_pr_vpq_info(TAG_NAME, "kernel VPQ_EXTERN_IOC_SET_SLR_HDR_TABLE_OETF_Curve_Ctrl fail\n");
			ret = -1;
		} else {
			rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_SLR_HDR_TABLE_OETF_Curve_Ctrl success. index=%d\n", oetf_curve_index_from_AP);
			ret = Scaler_set_APDEM(DEM_ARG_HDR_OETF_Curve_Ctrl, (void *)&oetf_curve_index_from_AP);
		}
	}
	break;
	
	case VPQ_EXTERN_IOC_Sync_PQ_table_bin_ENABLE:
	{
		unsigned int args = 0;

		rtd_pr_vpq_info("VPQ_EXTERN_IOC_Sync_PQ_table_bin_ENABLE\n");
		if 	(copy_from_user((void *)&args, (const void __user *)arg, sizeof(unsigned char)))
		{
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_Sync_PQ_table_bin_ENABLE fail\n");
			ret = -1;
		} else {
			ret = 0;
			Scaler_Set_PQ_table_bin_En((unsigned char)args);
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_COLORSPACE://HSI
	{
		//rtd_pr_vpq_info("[Kernel]VPQ_EXTERN_IOC_SET_COLORSPACE\n");
		_system_setting_info *VIP_system_info_structure_table = NULL;
		static unsigned char pre_colorspaceisNative=0;
		RTK_VPQ_COLORSPACE_INDEX ColoSpace_T;
		extern RTK_VPQ_COLORSPACE_INDEX ColoSpace_Index;
		extern unsigned char g_srgbForceUpdate;

		VIP_system_info_structure_table = (_system_setting_info *)Scaler_GetShareMemVirAddr(SCALERIOC_VIP_system_info_structure);

		if (copy_from_user(&ColoSpace_T,  (int __user *)arg,sizeof(RTK_VPQ_COLORSPACE_INDEX))) {
			ret = -1;
		} else {

			ColorSpaceMode = ColoSpace_T.mode;
			if(pre_colorspaceisNative==1)
				g_srgbForceUpdate=1;

			//rtd_pr_vpq_info("HDR_flag=%d\n",VIP_system_info_structure_table ->HDR_flag);
			//rtd_pr_vpq_info("pre_colorspaceisNative=%d,g_srgbForceUpdate2=%d\n",pre_colorspaceisNative,g_srgbForceUpdate2);


			if(VIP_system_info_structure_table ->HDR_flag==0){
			if(ColoSpace_T.mode==0){//Color Space Mode=Auto
				drvif_color_D_3dLUT_Enable(0);
				fwif_color_set_sRGBMatrix();
			}
			else if(ColoSpace_T.mode==1){//Color Space Mode=Native
				drvif_color_D_3dLUT_Enable(0);
				fwif_color_set_color_mapping_enable(SLR_MAIN_DISPLAY, 0);//disable sRGB;//Scaler_Set_ColorSpace_InvGamma_sRGB_OFF();//Scaler_Set_ColorSpace_InvGamma_sRGB(0);
			}
			else if(ColoSpace_T.mode==2){//Color Space Mode=Custom
				Scaler_Set_ColorSpace_D_3dlutTBL(&ColoSpace_T);
				memcpy(&ColoSpace_Index,&ColoSpace_T,sizeof(RTK_VPQ_COLORSPACE_INDEX));
				drvif_color_D_3dLUT_Enable(1);
				fwif_color_set_sRGBMatrix();
			}
			}
			else{
				//rtd_pr_vpq_info("HDR no need to run ColorSpace flow\n");
				drvif_color_D_3dLUT_Enable(0);
				fwif_color_set_sRGBMatrix();
				ColorSpaceMode=0;
			}
			fwif_color_set_gamma_Magic();
			if(ColorSpaceMode==1)//Color Space Mode=Native
				pre_colorspaceisNative=1;
			else
				pre_colorspaceisNative=0;

			g_srgbForceUpdate = 0;

			ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_GET_LC_Blight_dat:
	{
		unsigned int *buf;
		unsigned int size;
		_RPC_system_setting_info *VIP_RPC_system_info_structure_table=NULL;


		VIP_RPC_system_info_structure_table = (_RPC_system_setting_info*)Scaler_GetShareMemVirAddr(SCALERIOC_VIP_RPC_system_info_structure);
		//if(Scaler_color_get_System_Info(VIP_RPC_system_info_structure_table) == true){
		if(VIP_RPC_system_info_structure_table != NULL){
			size = (unsigned int)((VIP_RPC_system_info_structure_table->Local_Contrast_Ctrl.Local_Contrast_Region_Num_Size.nBlk_Hnum+1)*(VIP_RPC_system_info_structure_table->Local_Contrast_Ctrl.Local_Contrast_Region_Num_Size.nBlk_Vnum+1));
			buf = (unsigned int *)dvr_malloc_specific(size * sizeof(unsigned int), GFP_DCU2_FIRST);
			Scaler_color_get_LC_Blight_dat(buf, size);
			if (copy_to_user((void __user *)arg, (void *)buf, sizeof(unsigned int)*size)) {
				rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_LC_Blight_dat fail\n");
				ret = -1;
			} else
				ret = 0;
			dvr_free((void *)buf);
		}else{
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_LC_Blight_dat fail\n");
			ret = -1;
		}
	}
	break;

	case VPQ_EXTERN_IOC_GET_LC_Blight_size:
	{
		_RPC_system_setting_info *VIP_RPC_system_info_structure_table=NULL;
		unsigned int size[2] = {0};

		VIP_RPC_system_info_structure_table = (_RPC_system_setting_info*)Scaler_GetShareMemVirAddr(SCALERIOC_VIP_RPC_system_info_structure);
		size[0] = (VIP_RPC_system_info_structure_table->Local_Contrast_Ctrl.Local_Contrast_Region_Num_Size.nBlk_Hnum+1);
		size[1] = (VIP_RPC_system_info_structure_table->Local_Contrast_Ctrl.Local_Contrast_Region_Num_Size.nBlk_Vnum+1);
		if (copy_to_user((void __user *)arg, (void *)size, sizeof(unsigned int)*2)) {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_LC_Blight_size fail\n");
			ret = -1;
		} else
			ret = 0;
	}
	break;

	case VPQ_EXTERN_IOC_INIT_DDOMAIN3DLUT_GAMMA_TABLE:
	{
		extern UINT8 DDomain3DLUT_Gamma_control[5][3];

		if (copy_to_user((void __user *)arg, (void *)&(DDomain3DLUT_Gamma_control[0][0]), sizeof(UINT8) * 15)) {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_INIT_DDOMAIN3DLUT_GAMMA_TABLE fail\n");
			ret = -1;
		} else {
			ret = 0;
		}

	}
	break;

	case VPQ_EXTERN_IOC_SET_DDOMAIN3DLUT_GAMMA:
	{
		UINT8 i_tmp = 0;

		if (copy_to_user((void __user *)arg, (void *)&i_tmp, sizeof(UINT8))) {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_DDOMAIN3DLUT_GAMMA fail\n");
			ret = -1;
		} else {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_DDOMAIN3DLUT_GAMMA get %d\n", i_tmp);
			Scaler_color_set_DDomain3DLUT_gamma_by_colorspace(i_tmp);
			ret = 0;
		}

	}
	break;

	case VPQ_EXTERN_IOC_SET_DOLBYBACKLIGHT:
	{
		KADP_VPQ_DOBYBAKLIGHT_INI glob_dolbybacklight_ini;

		if(copy_from_user(&glob_dolbybacklight_ini,  (int __user *)arg,sizeof(KADP_VPQ_DOBYBAKLIGHT_INI)))
		{
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_DOLBYBACKLIGHT fail\n");
			ret = -1;
		}
		else
		{
			#ifdef ADIM_PDIM_CTRL
			Scaler_color_vpq_set_dolbylight_init(glob_dolbybacklight_ini);
			#endif
			ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_GET_PANORAMA_TYPE:
	{
		TV030_RATIO_TYPE_E PamoramaType = (TV030_RATIO_TYPE_E)Scaler_DispCheckRatio(SLR_RATIO_PANORAMA);

		if(copy_to_user((void __user *)arg, (void *)&PamoramaType, sizeof(TV030_RATIO_TYPE_E)))
		{
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_PANORAMA_TYPE fail\n");
			ret = -1;
		}else
			ret = 0;
	}
	break;

	case VPQ_EXTERN_IOC_GET_VD_COLOR_byAP:
	{
		VD_COLOR_ST vd_color = {0};
		if (fwif_color_module_vdc_GetConBriSatHue_byAP(&vd_color) != 0)
			return -1;

		if(copy_to_user((void __user *)arg, &vd_color, sizeof(VD_COLOR_ST))) {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_GET_VD_COLOR_byAP fail\n");
			ret = -1;
		} else
			ret = 0;
	}
	break;

	case VPQ_EXTERN_IOC_SET_VD_COLOR_byAP:
	{
		VD_COLOR_ST vd_color = {0};

		if (copy_from_user(&vd_color,  (void __user *)arg,sizeof(VD_COLOR_ST))) {
			rtd_pr_vpq_err("kernel VPQ_EXTERN_IOC_SET_VD_COLOR_byAP fail\n");
			ret = -1;
		} else {
			ret = fwif_color_module_vdc_SetConBriSatHue_byAP(&vd_color);
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_RADCR_Para:
	{
		extern int scalerVIP_RADCR_set_table_from_user_space(unsigned long user_data_addr);
		if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
			rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_RADCR_Para hal_VPQ_DATA_EXTERN copy fail\n");
			ret = -1;
		} else {
			if (hal_VPQ_DATA_EXTERN.length != sizeof(RADCR_TBL_ST)) {
				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_RADCR_Para len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
				ret = -1;
			} else {
				ret = scalerVIP_RADCR_set_table_from_user_space(hal_VPQ_DATA_EXTERN.pData);
			}
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_RADCR_En:
	{
		extern void scalerVIP_RADCR_set_enable(short enable);
		unsigned int En;
		if (copy_from_user(&En,  (void __user *)arg, sizeof(unsigned int))) {
			ret = -1;
		} else {
			scalerVIP_RADCR_set_enable(En);
			ret = 0;
		}
	}
	break;

	case VPQ_EXTERN_IOC_SET_RADCR_Backlight_Mapping:
	{
		extern int scalerVIP_RADCR_set_bl_mapping_table_from_user_space(unsigned long user_data_addr);
		if (copy_from_user(&hal_VPQ_DATA_EXTERN, (int __user *)arg, sizeof(HAL_VPQ_DATA_EXTERN_T))) {
			rtd_pr_vpq_err("VPQ_EXTERN_IOC_SET_RADCR_Para hal_VPQ_DATA_EXTERN copy fail\n");
			ret = -1;
		} else {
			if (hal_VPQ_DATA_EXTERN.length != sizeof(RADCR_BL_MAP_ST)) {
				rtd_pr_vpq_info("kernel VPQ_EXTERN_IOC_SET_RADCR_Backlight_Mapping len error len=%d,\n", hal_VPQ_DATA_EXTERN.length);
				ret = -1;
			} else {
				ret = scalerVIP_RADCR_set_bl_mapping_table_from_user_space(hal_VPQ_DATA_EXTERN.pData);
			}
		}
	}
	break;

	default:
		rtd_pr_vpq_debug("kernel IO command %d not found!\n", cmd);
		rtd_pr_vpq_info("VPQ_EXTERN_IOC cmd=0x%x unknown\n", cmd);
		return -1;
	}
	return ret;/*Success*/
}

struct file_operations vpqex_fops = {
	.owner = THIS_MODULE,
	.open = vpqex_open,
	.release = vpqex_release,
	.read  = vpqex_read,
	.write = vpqex_write,
	.unlocked_ioctl = vpqex_ioctl,
#ifdef CONFIG_ARM64
#ifdef CONFIG_COMPAT
	.compat_ioctl = vpqex_ioctl,
#endif
#endif
};

#ifdef CONFIG_PM
static const struct dev_pm_ops vpqex_pm_ops =
{
        .suspend    = vpqex_suspend,
        .resume     = vpqex_resume,
#ifdef CONFIG_HIBERNATION
	.freeze 	= vpqex_suspend,
	.thaw		= vpqex_resume,
	.poweroff	= vpqex_suspend,
	.restore	= vpqex_resume,
#endif

};
#endif // CONFIG_PM

static struct class *vpqex_class = NULL;
static struct platform_device *vpqex_platform_devs = NULL;
static struct platform_driver vpqex_platform_driver = {
	.driver = {
		.name = VPQ_EXTERN_DEVICE_NAME,
		.bus = &platform_bus_type,
#ifdef CONFIG_PM
		.pm = &vpqex_pm_ops,
#endif
    },
} ;

static char *vpqex_devnode(struct device *dev, umode_t *mode)
{
	return NULL;
}

int vpqex_major   = 0;
int vpqex_minor   = 0 ;
int vpqex_nr_devs = 1;

int vpqex_module_init(void)
{

	int result;
	rtd_pr_vpq_info("vpqex_module_init %d\n", __LINE__);
	//dev_t devno = 0;//vbe device number
	result = alloc_chrdev_region(&vpqex_devno, vpqex_minor, vpqex_nr_devs, VPQ_EXTERN_DEVICE_NAME);
	vpqex_major = MAJOR(vpqex_devno);
	if (result != 0) {
		rtd_pr_vpq_err("Cannot allocate VPQEX device number\n");
		rtd_pr_vpq_info("vpqex_module_init %d\n", __LINE__);
		return result;
	}

	rtd_pr_vpq_info("vpqex_module_init %d\n", __LINE__);

	rtd_pr_vpq_info("VPQEX_DEVICE init module major number = %d\n", vpqex_major);
	//vpqex_devno = MKDEV(vpqex_major, vpqex_minor);

	vpqex_class = class_create(THIS_MODULE, VPQ_EXTERN_DEVICE_NAME);
	if (IS_ERR(vpqex_class)) {
		rtd_pr_vpq_err("scalevpqex: can not create class...\n");
		rtd_pr_vpq_info("vpqex_module_init %d\n", __LINE__);
		result = PTR_ERR(vpqex_class);
		goto fail_class_create;
	}

	vpqex_class->devnode = vpqex_devnode;

	vpqex_platform_devs = platform_device_register_simple(VPQ_EXTERN_DEVICE_NAME, -1, NULL, 0);
	if (platform_driver_register(&vpqex_platform_driver) != 0) {
		rtd_pr_vpq_err("scalevpqex: can not register platform driver...\n");
		rtd_pr_vpq_info("vpqex_module_init %d\n", __LINE__);
		result = -ENOMEM;
		goto fail_platform_driver_register;
	}

	cdev_init(&vpqex_cdev, &vpqex_fops);
	vpqex_cdev.owner = THIS_MODULE;
   	vpqex_cdev.ops = &vpqex_fops;
	result = cdev_add(&vpqex_cdev, vpqex_devno, 1);
	if (result < 0) {
		rtd_pr_vpq_err("scalevpqex: can not add character device...\n");
		rtd_pr_vpq_info("vpqex_module_init %d\n", __LINE__);
		goto fail_cdev_init;
	}

	device_create(vpqex_class, NULL, vpqex_devno, NULL, VPQ_EXTERN_DEVICE_NAME);

//	sema_init(&VPQ_EXTERN_Semaphore, 1);
	sema_init(&ICM_Semaphore,1);
	sema_init(&I_RGB2YUV_Semaphore,1);
	sema_init(&LC_Semaphore,1);


	vpqex_boot_init();

	rtd_pr_vpq_info("vpqex_module_init %d\n", __LINE__);
	return 0;/*Success*/

fail_cdev_init:
	platform_driver_unregister(&vpqex_platform_driver);
fail_platform_driver_register:
	platform_device_unregister(vpqex_platform_devs);
	vpqex_platform_devs = NULL;
	class_destroy(vpqex_class);
fail_class_create:
	vpqex_class = NULL;
	unregister_chrdev_region(vpqex_devno, 1);
	rtd_pr_vpq_info("vpqex_module_init %d\n", __LINE__);
	return result;

}



void __exit vpqex_module_exit(void)
{
	rtd_pr_vpq_info("vpqex_module_exit\n");

	if (vpqex_platform_devs == NULL)
		BUG();

	device_destroy(vpqex_class, vpqex_devno);
	cdev_del(&vpqex_cdev);

	platform_driver_unregister(&vpqex_platform_driver);
	platform_device_unregister(vpqex_platform_devs);
	vpqex_platform_devs = NULL;

	class_destroy(vpqex_class);
	vpqex_class = NULL;

	unregister_chrdev_region(vpqex_devno, 1);
}

#ifdef CONFIG_SUPPORT_SCALER_MODULE
// the module init/exit will be moved to scaler_module.c if scaler was built as a kernel module
#else
module_init(vpqex_module_init);
module_exit(vpqex_module_exit);
#endif
#endif
