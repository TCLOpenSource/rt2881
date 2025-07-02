#ifndef BUILD_QUICK_SHOW
#include <linux/semaphore.h>
#ifdef CONFIG_KDRIVER_USE_NEW_COMMON
	#include <scaler/scalerCommon.h>
#else
#include <scalercommon/scalerCommon.h>
#endif
#else
#include <no_os/semaphore.h>
#endif

#include <rbus/h3ddma_hsd_reg.h>

#include <rbus/h3ddma_0_rgb2yuv_dither_4xxto4xx_reg.h>
//#include <rbus/scaler/rbusPpOverlayReg.h>
#include <rbus/ppoverlay_reg.h>
#include <rtd_log/rtd_module_log.h>
#include <tvscalercontrol/i3ddma/i3ddma.h>
#include <tvscalercontrol/i3ddma/i3ddma_uzd.h>
#include <tvscalercontrol/i3ddma/i3ddma_reg_array.h>

#ifndef VPQ_SET_REG_FIELD
#define VPQ_SET_REG_FIELD(REG,FIELD,VALUE) do{ (REG) = ((REG) & ~((unsigned int)FIELD##_mask)) + (FIELD((unsigned int)VALUE)); } while(0);
#endif

unsigned short i3_tRGB2YUV_COEF_709_RGB_0_255[] =
{
    // CCIR 709 RGB
#if 1 // for 0~255
    0x0131,  // m11[10:2] U(9,8)
    0x025c,  // m12   [20:13] U(8,8)
    0x0074,  // m13 [31:23] U(9,8)
#else // for 16~235
    0x0163,  // m11
    0x02bf,   // m12
    0x0087,  // m13
#endif
    0x03D5,   // m21 >> 2 0x01D5 up bits at AT Name: cychen2 , Date: 2010/3/25
    0x0357,   // m22 >> 1
    0x0080,   // m23 >> 2
    0x0080,   // m31 >> 2
    0x0795,   // m32 >> 2 0x0195  up bits at AT Name: cychen2 , Date: 2010/3/25
    0x03D7,   // m33 >> 1
    0x0000,    // Yo_even
    0x0000,    // Yo_odd
    0x0100,    // Y_gain
    0x0001,  // sel_RGB
    0x0000,  // sel_Yin_offset
    0x0001,  // sel_UV_out_offset
    0x0000,  // sel_UV_off
    0x0000,  // Matrix_bypass
    0x0001,  // Enable_Y_gain
};
#if 0//no used
static unsigned short i3_tRGB2YUV_COEF_709_RGB_0_255_OSD_MIXER2[] =
{
    // CCIR 709 RGB

    0x0131,  //m11[10:2] U(9,8)
    0x0074,  // m12->m13
    0x025c,  // m13->m12
    0x03D5,   // m21
    0x0100,   // m22->m23
    0x01a8,   // m23->m22
    0x0080,   // m31
    0x07e8,   // m32->m33
    0x0328,   // m33->m32

    0x0000,    // Yo_even
    0x0000,    // Yo_odd
    0x0100,    // Y_gain
    0x0001,  // sel_RGB
    0x0000,  // sel_Yin_offset
    0x0001,  // sel_UV_out_offset
    0x0000,  // sel_UV_off
    0x0000,  // Matrix_bypass
    0x0001,  // Enable_Y_gain
};



static unsigned short i3_tRGB2YUV_COEF_709_RGB_0_255_SUB_OSD_MIXER1[] =
{
    // CCIR 709 RGB
    0x0074,  // m11->m13
    0x0130,  // m12->m11
    0x025c,  // m13->m12
    0x0080,   // m21->m23
    0x03a8,   // m22->m21
    0x01a8,   // m23->m22
    0x03e8,   // m31->m33
    0x0080,   // m32->m31
    0x0328,   // m33->m32

    0x0000,    // Yo_even
    0x0000,    // Yo_odd
    0x0100,    // Y_gain
    0x0001,  // sel_RGB
    0x0000,  // sel_Yin_offset
    0x0001,  // sel_UV_out_offset
    0x0000,  // sel_UV_off
    0x0000,  // Matrix_bypass
    0x0001,  // Enable_Y_gain
};
#endif

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
static  short tScale_Down_Coef_2tap[] = {
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    31,  95, 159, 223, 287, 351, 415, 479,
    543, 607, 671, 735, 799, 863, 927, 991,
};

static  short tScale_Down_Coef_Blur[] = {
    2,   3,   5,   7,  10,  15,  20,  28,  38,  49,  64,  81, 101, 124, 150, 178,
    209, 242, 277, 314, 351, 389, 426, 462, 496, 529, 556, 582, 602, 618, 629, 635,
};

static  short tScale_Down_Coef_Mid[] = {
    -2,  0,   1,   2,   5,   9,  15,  22,  32,  45,  60,  77,  98, 122, 149, 179,
    211, 245, 281, 318, 356, 394, 431, 468, 502, 533, 561, 586, 606, 620, 630, 636,
};

static  short tScale_Down_Coef_Sharp[] = {
    -2,   0,   1,   3,   6,  10,  15,  22,  32,  43,  58,  75,  95, 119, 145, 174,
    206, 240, 276, 314, 353, 391, 430, 467, 503, 536, 565, 590, 612, 628, 639, 646,
};

static  short tScale_Down_Coef_repeat[] = {
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024,
};

short *tScaleDown_COEF_TAB[] =
{
    tScale_Down_Coef_Blur,
	tScale_Down_Coef_Mid,
	tScale_Down_Coef_Sharp,
	tScale_Down_Coef_2tap,
	tScale_Down_Coef_repeat
};

/*unsigned short tRGB2YUV_COEF_709_RGB_0_255[] =
{
	// CCIR 709 RGB
#if 1 // for 0~255
	0x0131,  // m11[10:2] U(9,8)
	0x025c,  // m12   [20:13] U(8,8)
	0x0074,  // m13 [31:23] U(9,8)
#else // for 16~235
	0x0163,  // m11
	0x02bf,   // m12
	0x0087,  // m13
#endif
	0x03D5,   // m21 >> 2 0x01D5 up bits at AT Name: cychen2 , Date: 2010/3/25
	0x0357,   // m22 >> 1
	0x0080,   // m23 >> 2
	0x0080,   // m31 >> 2
	0x0795,   // m32 >> 2 0x0195  up bits at AT Name: cychen2 , Date: 2010/3/25
	0x03D7,   // m33 >> 1
	0x0000,    // Yo_even
	0x0000,    // Yo_odd
	0x0100,    // Y_gain
	0x0001,  // sel_RGB
	0x0000,  // sel_Yin_offset
	0x0001,  // sel_UV_out_offset
	0x0000,  // sel_UV_off
	0x0000,  // Matrix_bypass
	0x0001,  // Enable_Y_gain
};*/


void I3DDMA_color_colorspacergb2yuvtransfer(DISPD_CAP_SRC idmaDispD_in_sel)
{
	unsigned short *table_index = 0;
	h3ddma_0_rgb2yuv_dither_4xxto4xx_hdmi3_0_ctrl_RBUS  i3ddma_rgb2yuv_ctrl_reg;
	h3ddma_0_rgb2yuv_dither_4xxto4xx_hdmi3_0_tab_m11_m12_RBUS tab2_M11_M12_REG;
	h3ddma_0_rgb2yuv_dither_4xxto4xx_hdmi3_0_tab_m13_m21_RBUS tab2_M13_M21_REG;
	h3ddma_0_rgb2yuv_dither_4xxto4xx_hdmi3_0_tab_m22_m23_RBUS tab2_M22_M23_REG;
	h3ddma_0_rgb2yuv_dither_4xxto4xx_hdmi3_0_tab_m31_m32_RBUS tab2_M31_M32_REG;
	h3ddma_0_rgb2yuv_dither_4xxto4xx_hdmi3_0_tab_m33_ygain_RBUS tab2_M33_YGain_REG;
	h3ddma_0_rgb2yuv_dither_4xxto4xx_hdmi3_0_tab_yo_RBUS tab2_Yo_REG;
	unsigned long flags;//for spin_lock_irqsave

	//display_timing_ctrl2_RBUS display_timing_ctrl2_reg;
	ppoverlay_display_timing_ctrl2_RBUS display_timing_ctrl2_reg;

	if((IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL_reg)&_BIT2)>>2)
	{
		down(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
		spin_lock_irqsave(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//lock double buffer spinlock
		IoReg_ClearBits(PPOVERLAY_Double_Buffer_CTRL_reg, _BIT2);
		// IDMA VGIP DispD input source select
		display_timing_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL2_reg);
		//display_timing_ctrl2_reg.d2i3ddma_src_sel = idmaDispD_in_sel;
		IoReg_Write32(PPOVERLAY_Display_Timing_CTRL2_reg, display_timing_ctrl2_reg.regValue);
		IoReg_SetBits(PPOVERLAY_Double_Buffer_CTRL_reg, _BIT2);
		spin_unlock_irqrestore(get_DBreg_spinlock(SLR_MAIN_DISPLAY), flags);//unlock double buffer spinlock
		up(get_forcebg_semaphore());/*semaphore must added for not effect mute function when setting b8028300*/
	}
	else
	{
		// IDMA VGIP DispD input source select
		display_timing_ctrl2_reg.regValue = IoReg_Read32(PPOVERLAY_Display_Timing_CTRL2_reg);
		//display_timing_ctrl2_reg.d2i3ddma_src_sel = idmaDispD_in_sel;
		IoReg_Write32(PPOVERLAY_Display_Timing_CTRL2_reg, display_timing_ctrl2_reg.regValue);
	}
	rtd_pr_i3ddma_debug("PPOVERLAY_Display_Timing_CTRL2_reg = %x\n", IoReg_Read32(PPOVERLAY_Display_Timing_CTRL2_reg));

	switch(idmaDispD_in_sel)
	{
	    case CAP_SRC_SUBTITLE:
	        //table_index = i3_tRGB2YUV_COEF_709_RGB_0_255_SUB_OSD_MIXER1;
	        table_index = i3_tRGB2YUV_COEF_709_RGB_0_255;
	        break;
	    case CAP_SRC_OSD:
	    case CAP_SRC_DITHER:
	        //table_index = i3_tRGB2YUV_COEF_709_RGB_0_255_OSD_MIXER2;
	        table_index = i3_tRGB2YUV_COEF_709_RGB_0_255;
	        break;
	    default:
	        table_index = i3_tRGB2YUV_COEF_709_RGB_0_255;
	        break;
	}

	//main  all tab-1
	tab2_M11_M12_REG.m11 = table_index [_RGB2YUV_m11];
	tab2_M11_M12_REG.m12 = table_index [_RGB2YUV_m12];
	//rtd_pr_i3ddma_debug("i3ddma_rgb2yuv_tab1_m1_reg.regValue = %x\n", i3ddma_rgb2yuv_tab1_m1_reg.regValue);
	IoReg_Write32(H3DDMA_0_RGB2YUV_DITHER_4XXTO4XX_HDMI3_0_TAB_M11_M12_reg, tab2_M11_M12_REG.regValue);

	tab2_M13_M21_REG.m13 = table_index [_RGB2YUV_m13];
	tab2_M13_M21_REG.m21 = table_index [_RGB2YUV_m21];
	IoReg_Write32(H3DDMA_0_RGB2YUV_DITHER_4XXTO4XX_HDMI3_0_TAB_M13_M21_reg, tab2_M13_M21_REG.regValue);

	tab2_M22_M23_REG.m22 = table_index [_RGB2YUV_m22];
	tab2_M22_M23_REG.m23 = table_index [_RGB2YUV_m23];
	//rtd_pr_i3ddma_debug("i3ddma_rgb2yuv_tab1_m2_reg.regValue = %x\n", i3ddma_rgb2yuv_tab1_m2_reg.regValue);
	IoReg_Write32(H3DDMA_0_RGB2YUV_DITHER_4XXTO4XX_HDMI3_0_TAB_M22_M23_reg, tab2_M22_M23_REG.regValue);

	tab2_M31_M32_REG.m31 = table_index [_RGB2YUV_m31];
	tab2_M31_M32_REG.m32 = table_index [_RGB2YUV_m32];
	IoReg_Write32(H3DDMA_0_RGB2YUV_DITHER_4XXTO4XX_HDMI3_0_TAB_M31_M32_reg, tab2_M31_M32_REG.regValue);

	tab2_M33_YGain_REG.m33 = table_index [_RGB2YUV_m33];
	tab2_M33_YGain_REG.y_gain= table_index [_RGB2YUV_Y_gain];
	//rtd_pr_i3ddma_debug("i3ddma_rgb2yuv_tab1_m3_reg.regValue = %x\n", i3ddma_rgb2yuv_tab1_m3_reg.regValue);
	IoReg_Write32(H3DDMA_0_RGB2YUV_DITHER_4XXTO4XX_HDMI3_0_TAB_M33_YGain_reg, tab2_M33_YGain_REG.regValue);

	tab2_Yo_REG.yo_odd= table_index [_RGB2YUV_Yo_odd];
	IoReg_Write32(H3DDMA_0_RGB2YUV_DITHER_4XXTO4XX_HDMI3_0_TAB_Yo_reg, tab2_Yo_REG.regValue);

	i3ddma_rgb2yuv_ctrl_reg.regValue = IoReg_Read32(H3DDMA_0_RGB2YUV_DITHER_4XXTO4XX_HDMI3_0_CTRL_reg);

	i3ddma_rgb2yuv_ctrl_reg.sel_rgb= table_index [_RGB2YUV_sel_RGB];
	i3ddma_rgb2yuv_ctrl_reg.set_r_in_offset= table_index [_RGB2YUV_set_Yin_offset];
	i3ddma_rgb2yuv_ctrl_reg.set_uv_out_offset= table_index [_RGB2YUV_set_UV_out_offset];
	i3ddma_rgb2yuv_ctrl_reg.sel_uv_off= table_index [_RGB2YUV_sel_UV_off];
	i3ddma_rgb2yuv_ctrl_reg.matrix_bypass= table_index [_RGB2YUV_Matrix_bypass];
	i3ddma_rgb2yuv_ctrl_reg.sel_y_gain= table_index [_RGB2YUV_Enable_Y_gain];

	//i3ddma_rgb2yuv_ctrl_reg.idmaDispD_in_sel = idmaDispD_in_sel;
	if(idmaDispD_in_sel == CAP_SRC_UZU)
		i3ddma_rgb2yuv_ctrl_reg.en_rgb2yuv = 0;
	else
		i3ddma_rgb2yuv_ctrl_reg.en_rgb2yuv = 1;

	rtd_pr_i3ddma_debug("[IVI] rgb2yuv=%d\n", i3ddma_rgb2yuv_ctrl_reg.en_rgb2yuv);

	IoReg_Write32(H3DDMA_0_RGB2YUV_DITHER_4XXTO4XX_HDMI3_0_CTRL_reg, i3ddma_rgb2yuv_ctrl_reg.regValue);

	return;
}

void I3DDMA_ultrazoom_auto_four_tap(void)
{
	unsigned int z_RBUS_reg;
	z_RBUS_reg = IoReg_Read32( H3DDMA_HSD_I3DDMA0_UZD_CTRL1_reg );
	VPQ_SET_REG_FIELD( z_RBUS_reg , H3DDMA_HSD_I3DDMA0_UZD_CTRL1_auto_four_tap_en , 1 );
	IoReg_Write32( H3DDMA_HSD_I3DDMA0_UZD_CTRL1_reg , z_RBUS_reg );
}

void I3DDMA_ultrazoom_coef_db_apply(void)
{
	unsigned int z_RBUS_reg;
	z_RBUS_reg = IoReg_Read32( H3DDMA_HSD_I3DDMA0_UZD_COEF_DB_CTRL_reg );
	VPQ_SET_REG_FIELD( z_RBUS_reg , H3DDMA_HSD_I3DDMA0_UZD_COEF_DB_CTRL_coef_db_apply , 1 );
	IoReg_Write32( H3DDMA_HSD_I3DDMA0_UZD_COEF_DB_CTRL_reg , z_RBUS_reg );
}

void I3DDMA_ultrazoom_set_hor_ini_desire(
	h3ddma_hsd_i3ddma0_uzd_initial_value_RBUS* uzd_Initial_Value_REG ,
	h3ddma_hsd_i3ddma0_uzd_initial_int_value_RBUS* uzd_initial_int_value_REG ,
long long In_Width , long long HOR_FAC , int xdesire )
{
	int Vini = 0;
	if( !uzd_Initial_Value_REG || !uzd_initial_int_value_REG )
	{
		return;
	}
	if( HOR_FAC<=0 )
	{
		return;
	}
	Vini = (uzd_initial_int_value_REG->hor_ini_int<<8)|uzd_Initial_Value_REG->hor_ini;
	// Out_Height = floor( ( In_Width*1048576 -1 -(Vini<<12) ) / HOR_FAC ) + 1;
	while( Vini>=128 && ( ( ( In_Width*(long long)1048576 -(long long)1 -(long long)(Vini<<12) ) / HOR_FAC ) + 1)<xdesire )
	{
		Vini-=128;
	}
	uzd_initial_int_value_REG->hor_ini_int = Vini>>8;
	uzd_Initial_Value_REG->hor_ini = Vini&0xff;
}

void I3DDMA_ultrazoom_set_ver_ini_desire(
	h3ddma_hsd_i3ddma0_uzd_initial_value_RBUS* uzd_Initial_Value_REG ,
	h3ddma_hsd_i3ddma0_uzd_initial_int_value_RBUS* uzd_initial_int_value_REG ,
long long In_Height , long long VER_FAC , int xdesire )
{
	int Vini = 0;
	if( !uzd_Initial_Value_REG || !uzd_initial_int_value_REG )
	{
		return;
	}
	if( VER_FAC<=0 )
	{
		return;
	}
	Vini = (uzd_initial_int_value_REG->ver_ini_int<<8)|uzd_Initial_Value_REG->ver_ini;
	// Out_Height = floor( ( In_Height*1048576 -1 -(Vini<<12) ) / VER_FAC ) + 1;
	while( Vini>=128 && ( ( ( In_Height*(long long)1048576 -(long long)1 -(long long)(Vini<<12) ) / VER_FAC ) + 1)<xdesire )
	{
		Vini-=128;
	}
	uzd_initial_int_value_REG->ver_ini_int = Vini>>8;
	uzd_Initial_Value_REG->ver_ini = Vini&0xff;
}


#define TMPMUL	(16)
/*============================================================================*/
/**
 * CScalerSetScaleDown
 * This function is used to set scaling-down registers, including main and sub display.
 *
 * @param <info> { display-info struecture }
 * @return { none }
 * @note
 * It can be linear or non-linear either, based on the non-linear flag condition:
 */
void I3DDMA_ultrazoom_set_scale_down(SIZE* in_Size, SIZE* out_Size, unsigned char panorama)
{
    unsigned char SDRatio;
    unsigned char SDFilter=0;
    unsigned int tmp_data;
    short *coef_pt;
    unsigned char i;
    unsigned int nFactor;
    unsigned char Hini, Vini;

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    h3ddma_hsd_i3ddma0_uzd_ctrl0_RBUS				i3ddma_uzd_ctrl0_reg;
    h3ddma_hsd_i3ddma0_uzd_scale_hor_factor_RBUS	i3ddma_hsd_scale_hor_factor_reg;
    h3ddma_hsd_i3ddma0_uzd_scale_ver_factor_RBUS   	i3ddma_vsd_Scale_Ver_Factor_reg;
    h3ddma_hsd_i3ddma0_uzd_initial_value_RBUS		i3ddma_uzd_initial_value_reg;
    h3ddma_hsd_i3ddma0_uzd_initial_int_value_RBUS		i3ddma_uzd_initial_int_value_reg;
    h3ddma_hsd_i3ddma0_uzd_ibuff_ctrl_RBUS		i3ddma_uzd_ibuff_ctrl_reg;
	
    i3ddma_uzd_ctrl0_reg.regValue		= IoReg_Read32(H3DDMA_HSD_I3DDMA0_UZD_CTRL0_reg);
    i3ddma_hsd_scale_hor_factor_reg.regValue	= IoReg_Read32(H3DDMA_HSD_I3DDMA0_UZD_SCALE_HOR_FACTOR_reg);
    i3ddma_vsd_Scale_Ver_Factor_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA0_UZD_SCALE_VER_FACTOR_reg);
   // i3ddma_hsd_hor_delta1_reg.regValue	= IoReg_Read32(H3DDMA_HSD_I3DDMA0_UZD_HSD_Hor_Delta1_reg);
    i3ddma_uzd_initial_value_reg.regValue	= IoReg_Read32(H3DDMA_HSD_I3DDMA0_UZD_INITIAL_VALUE_reg);
    i3ddma_uzd_initial_int_value_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA0_UZD_INITIAL_INT_VALUE_reg);
    i3ddma_uzd_ibuff_ctrl_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA0_UZD_IBUFF_CTRL_reg);
	
    //o============ H scale-down=============o
    if (in_Size->nWidth > out_Size->nWidth) {
        // o-------calculate scaledown ratio to select one of different bandwith filters.--------o
        if ( out_Size->nWidth == 0 ) {
            rtd_pr_i3ddma_debug("output width = 0 !!!\n");
            SDRatio = 0;
        } else {
            SDRatio = (in_Size->nWidth*TMPMUL) / out_Size->nWidth;
        }

        //rtd_pr_i3ddma_debug("CSW SDRatio number=%d\n",SDRatio);

        if(SDRatio <= ((TMPMUL*3)/2))	//<1.5 sharp, wider bw
            SDFilter = 2;
        else if(SDRatio <= (TMPMUL*2) && SDRatio > ((TMPMUL*3)/2) )	// Mid
            SDFilter = 1;
        else		// blurest, narrow bw
            SDFilter = 0;

        //o---------------- fill coefficients into access port--------------------o
        coef_pt = tScaleDown_COEF_TAB[SDFilter];

        i3ddma_uzd_ctrl0_reg.h_y_table_sel = 0;	// TAB1
        i3ddma_uzd_ctrl0_reg.h_c_table_sel = 0;	// TAB1

        for (i=0; i<16; i++)
        {
            tmp_data = ((unsigned int)(*coef_pt++)<<16);
            tmp_data += (unsigned int)(*coef_pt++);
            IoReg_Write32(H3DDMA_HSD_I3DDMA0_UZD_FIR_COEF_TAB1_C0_reg+ i*4, tmp_data);
        }
    }

	//o============ V scale-down=============o
	if (in_Size->nLength > out_Size->nLength) {
		// o-------calculate scaledown ratio to select one of different bandwith filters.--------o

		 //jeffrey 961231
		if ( out_Size->nLength == 0 ) {
			SDRatio = 0;
		} else {
			SDRatio = (in_Size->nLength*TMPMUL) / out_Size->nLength;
		}

		//rtd_pr_i3ddma_debug("CSW SDRatio number=%d\n",SDRatio);

		#if 1
		SDFilter = 4;// when i3ddma pqc, i3ddma vsd can only repeat
		#else
		if(SDRatio <= ((TMPMUL*3)/2))	//<1.5 sharp, wider bw
			SDFilter = 2;
		else if(SDRatio <= (TMPMUL*2) && SDRatio > ((TMPMUL*3)/2) )	// Mid
			SDFilter = 1;
		else		// blurest, narrow bw
			SDFilter = 0;
		#endif

		//o---------------- fill coefficients into access port--------------------o
		coef_pt = tScaleDown_COEF_TAB[SDFilter];

		i3ddma_uzd_ctrl0_reg.v_y_table_sel = 1;	// TAB2
		i3ddma_uzd_ctrl0_reg.v_c_table_sel = 1;	// TAB2

		for (i=0; i<16; i++)
		{
			tmp_data = ((unsigned int)(*coef_pt++)<<16);
			tmp_data += (unsigned int)(*coef_pt++);
			IoReg_Write32(H3DDMA_HSD_I3DDMA0_UZD_FIR_COEF_TAB2_C0_reg+ i*4, tmp_data);
		}

	}
	I3DDMA_ultrazoom_coef_db_apply();

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	if (in_Size->nWidth > out_Size->nWidth) {    // H scale-down
		Hini = 0x80;// actual phase is 0x180;
		i3ddma_uzd_initial_value_reg.hor_ini = Hini;
		i3ddma_uzd_initial_int_value_reg.hor_ini_int = 1;
		{
			//frank@0108 add for code exception
			if ( out_Size->nWidth == 0 ) {
				rtd_pr_i3ddma_debug("output width = 0 !!!\n");
				nFactor = 0;
			} else if(in_Size->nWidth>4095){
				//nFactor = (unsigned int)((((in_Size->nWidth-1)<<19) / (out_Size->nWidth-1))<<1);
				nFactor = (unsigned int)((((in_Size->nWidth)<<19) / (out_Size->nWidth))<<1);
			} else if(in_Size->nWidth>2047){
				//nFactor = (unsigned int)(((in_Size->nWidth-1)<<20) / (out_Size->nWidth-1));
				nFactor = (unsigned int)(((in_Size->nWidth)<<20) / (out_Size->nWidth));
			} else {
				nFactor = (unsigned int)((in_Size->nWidth<<21)) / (out_Size->nWidth);
				nFactor = SHR(nFactor + 1, 1); //rounding
			}
		}

		i3ddma_hsd_scale_hor_factor_reg.hor_fac = nFactor;
		I3DDMA_ultrazoom_set_hor_ini_desire(
			&i3ddma_uzd_initial_value_reg ,
			&i3ddma_uzd_initial_int_value_reg ,
		in_Size->nWidth , nFactor , out_Size->nWidth );
	}else {
		i3ddma_hsd_scale_hor_factor_reg.hor_fac = 0x100000;
	}

	i3ddma_uzd_ibuff_ctrl_reg.ibuf_h_size = out_Size->nWidth;

	if (in_Size->nLength > out_Size->nLength) {    // V scale-down
//			ich2_uzd_Ctrl0_REG.buffer_mode = 0x02;
		Vini = 0x80;// actual 0x180;
		i3ddma_uzd_initial_value_reg.ver_ini = Vini;
		i3ddma_uzd_initial_int_value_reg.ver_ini_int = 1;
		//nFactor = (unsigned int)((ptInSize->nLength << 20) - (Vini<<12)) / (ptOutSize->nLength - 1);
		//frank@0108 add for code exception
		if ( out_Size->nLength == 0 ) {
			rtd_pr_i3ddma_debug("output length = 0 !!!\n");
			nFactor = 0;
		} else {
			//nFactor = (unsigned int)((in_Size->nLength << 21)) / (out_Size->nLength);
			//modify the calculation for V-scale down @Crixus 20160804
			nFactor = (unsigned int)((in_Size->nLength * 1024 * 1024)) / (out_Size->nLength);
		}
		//nFactor = SHR(nFactor+1, 1); //rounding
		i3ddma_vsd_Scale_Ver_Factor_reg.ver_fac = nFactor;
		I3DDMA_ultrazoom_set_ver_ini_desire(
				&i3ddma_uzd_initial_value_reg ,
				&i3ddma_uzd_initial_int_value_reg ,
			in_Size->nLength , nFactor , out_Size->nLength );
	}else{
//			ich2_uzd_Ctrl0_REG.buffer_mode = 0;
		i3ddma_vsd_Scale_Ver_Factor_reg.ver_fac = 0x100000;
	}

//	ich2_uzd_Ctrl0_REG.output_fmt = 1;
//	CLR_422_Fr_SD();
	I3DDMA_ultrazoom_auto_four_tap();

	IoReg_Write32(H3DDMA_HSD_I3DDMA0_UZD_CTRL0_reg, i3ddma_uzd_ctrl0_reg.regValue);
	IoReg_Write32(H3DDMA_HSD_I3DDMA0_UZD_SCALE_HOR_FACTOR_reg, i3ddma_hsd_scale_hor_factor_reg.regValue);
	IoReg_Write32(H3DDMA_HSD_I3DDMA0_UZD_SCALE_VER_FACTOR_reg, i3ddma_vsd_Scale_Ver_Factor_reg.regValue);
	IoReg_Write32(H3DDMA_HSD_I3DDMA0_UZD_INITIAL_VALUE_reg, i3ddma_uzd_initial_value_reg.regValue);
	IoReg_Write32(H3DDMA_HSD_I3DDMA0_UZD_INITIAL_INT_VALUE_reg, i3ddma_uzd_initial_int_value_reg.regValue);
	IoReg_Write32(H3DDMA_HSD_I3DDMA0_UZD_IBUFF_CTRL_reg, i3ddma_uzd_ibuff_ctrl_reg.regValue);

	return;
}


#ifdef CONFIG_ENABLE_HDMI_NN
void IDDMA_ultrazoom_auto_four_tap(unsigned char dev)
{
	unsigned int z_RBUS_reg;
	z_RBUS_reg = IoReg_Read32( iddma_uzd_ctrl1_reg_addr[dev] );
	VPQ_SET_REG_FIELD( z_RBUS_reg , H3DDMA_HSD_I3DDMA1_UZD_CTRL1_auto_four_tap_en , 1 );
	IoReg_Write32( iddma_uzd_ctrl1_reg_addr[dev] , z_RBUS_reg );
}

void IDDMA_ultrazoom_coef_db_apply(unsigned char dev)
{
	unsigned int z_RBUS_reg;
	z_RBUS_reg = IoReg_Read32( iddma_uzd_coef_db_ctrl_reg_addr[dev] );
	VPQ_SET_REG_FIELD( z_RBUS_reg , H3DDMA_HSD_I3DDMA1_UZD_COEF_DB_CTRL_coef_db_apply , 1 );
	IoReg_Write32( iddma_uzd_coef_db_ctrl_reg_addr[dev] , z_RBUS_reg );
}
void I3DDMA_ultrazoom_set_scale_down_for_cap(unsigned char dev, SIZE* in_Size, SIZE* out_Size, unsigned char panorama)
{
    unsigned char SDRatio;
    unsigned char SDFilter=0;
    unsigned int tmp_data;
    short *coef_pt;
    unsigned char i;
    unsigned int nFactor;
    unsigned char Hini, Vini;

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    h3ddma_hsd_i3ddma1_uzd_ctrl0_RBUS			       i3ddma_uzd_ctrl0_reg;
    h3ddma_hsd_i3ddma1_uzd_scale_hor_factor_RBUS	i3ddma_hsd_scale_hor_factor_reg;
    h3ddma_hsd_i3ddma1_uzd_scale_ver_factor_RBUS   	i3ddma_vsd_Scale_Ver_Factor_reg;
    h3ddma_hsd_i3ddma1_uzd_initial_value_RBUS		i3ddma_uzd_initial_value_reg;
	h3ddma_hsd_i3ddma0_uzd_initial_int_value_RBUS		i3ddma_uzd_initial_int_value_reg;
    h3ddma_hsd_i3ddma0_uzd_ibuff_ctrl_RBUS		i3ddma_uzd_ibuff_ctrl_reg;

    i3ddma_uzd_ctrl0_reg.regValue		= IoReg_Read32(iddma_uzd_ctrl_reg_addr[dev]);
    i3ddma_hsd_scale_hor_factor_reg.regValue	= IoReg_Read32(iddma_uzd_hor_factor_reg_addr[dev]);
    i3ddma_vsd_Scale_Ver_Factor_reg.regValue = IoReg_Read32(iddma_uzd_ver_factor_reg_addr[dev]);
    i3ddma_uzd_initial_value_reg.regValue	= IoReg_Read32(iddma_uzd_init_value_reg_addr[dev]);
	i3ddma_uzd_initial_int_value_reg.regValue = IoReg_Read32(iddma_uzd_init_int_reg_addr[dev]);
    i3ddma_uzd_ibuff_ctrl_reg.regValue = IoReg_Read32(iddma_uzd_ibuff_ctrl_reg_addr[dev]);

	rtd_pr_i3ddma_emerg("I3DDMA_ultrazoom_set_scale_down\n");
    //o============ H scale-down=============o
    if (in_Size->nWidth > out_Size->nWidth) {
        // o-------calculate scaledown ratio to select one of different bandwith filters.--------o
        if ( out_Size->nWidth == 0 ) {
            rtd_pr_i3ddma_debug("output width = 0 !!!\n");
            SDRatio = 0;
        } else {
            SDRatio = (in_Size->nWidth*TMPMUL) / out_Size->nWidth;
        }

        //rtd_pr_i3ddma_debug("CSW SDRatio number=%d\n",SDRatio);

        if(SDRatio <= ((TMPMUL*3)/2))	//<1.5 sharp, wider bw
            SDFilter = 2;
        else if(SDRatio <= (TMPMUL*2) && SDRatio > ((TMPMUL*3)/2) )	// Mid
            SDFilter = 1;
        else		// blurest, narrow bw
            SDFilter = 0;

        //o---------------- fill coefficients into access port--------------------o
        coef_pt = tScaleDown_COEF_TAB[SDFilter];

        i3ddma_uzd_ctrl0_reg.h_y_table_sel = 0;	// TAB1
        i3ddma_uzd_ctrl0_reg.h_c_table_sel = 0;	// TAB1

        for (i=0; i<16; i++)
        {
            tmp_data = ((unsigned int)(*coef_pt++)<<16);
            tmp_data += (unsigned int)(*coef_pt++);
            IoReg_Write32(iddma_uzd_fir_coef_tab1_c0_reg_addr[dev] + i*4, tmp_data);
        }
    }

	//o============ V scale-down=============o
	if (in_Size->nLength > out_Size->nLength) {
		// o-------calculate scaledown ratio to select one of different bandwith filters.--------o

		 //jeffrey 961231
		if ( out_Size->nLength == 0 ) {
			SDRatio = 0;
		} else {
			SDRatio = (in_Size->nLength*TMPMUL) / out_Size->nLength;
		}

		//rtd_pr_i3ddma_debug("CSW SDRatio number=%d\n",SDRatio);

		#if 1
		SDFilter = 4;// when i3ddma pqc, i3ddma vsd can only repeat
		#else
		if(SDRatio <= ((TMPMUL*3)/2))	//<1.5 sharp, wider bw
			SDFilter = 2;
		else if(SDRatio <= (TMPMUL*2) && SDRatio > ((TMPMUL*3)/2) )	// Mid
			SDFilter = 1;
		else		// blurest, narrow bw
			SDFilter = 0;
		#endif

		//o---------------- fill coefficients into access port--------------------o
		coef_pt = tScaleDown_COEF_TAB[SDFilter];

		i3ddma_uzd_ctrl0_reg.v_y_table_sel = 1;	// TAB2
		i3ddma_uzd_ctrl0_reg.v_c_table_sel = 1;	// TAB2

		for (i=0; i<16; i++)
		{
			tmp_data = ((unsigned int)(*coef_pt++)<<16);
			tmp_data += (unsigned int)(*coef_pt++);
			IoReg_Write32(iddma_uzd_fir_coef_tab2_c0_reg_addr[dev] + i*4, tmp_data);
		}

	}
	IDDMA_ultrazoom_coef_db_apply(dev);


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	if (in_Size->nWidth > out_Size->nWidth) {    // H scale-down
		Hini = 0x80;// actual phase is 0x180;
		i3ddma_uzd_initial_value_reg.hor_ini = Hini;
		i3ddma_uzd_initial_int_value_reg.hor_ini_int = 1;
		{
			//frank@0108 add for code exception
			if ( out_Size->nWidth == 0 ) {
				rtd_pr_i3ddma_debug("output width = 0 !!!\n");
				nFactor = 0;
			} else if(in_Size->nWidth>4095){
				//nFactor = (unsigned int)((((in_Size->nWidth-1)<<19) / (out_Size->nWidth-1))<<1);
				nFactor = (unsigned int)((((in_Size->nWidth)<<19) / (out_Size->nWidth))<<1);
			} else if(in_Size->nWidth>2047){
				//nFactor = (unsigned int)(((in_Size->nWidth-1)<<20) / (out_Size->nWidth-1));
				nFactor = (unsigned int)(((in_Size->nWidth)<<20) / (out_Size->nWidth));
			} else {
				nFactor = (unsigned int)((in_Size->nWidth<<21)) / (out_Size->nWidth);
				nFactor = SHR(nFactor + 1, 1); //rounding
			}
		}

		i3ddma_hsd_scale_hor_factor_reg.hor_fac = nFactor;
		I3DDMA_ultrazoom_set_hor_ini_desire(
			(h3ddma_hsd_i3ddma0_uzd_initial_value_RBUS*)&i3ddma_uzd_initial_value_reg ,
			(h3ddma_hsd_i3ddma0_uzd_initial_int_value_RBUS*)&i3ddma_uzd_initial_int_value_reg ,
		in_Size->nWidth , nFactor , out_Size->nWidth );
	}else {
		i3ddma_hsd_scale_hor_factor_reg.hor_fac = 0x100000;
	}

	i3ddma_uzd_ibuff_ctrl_reg.ibuf_h_size = out_Size->nWidth;
	i3ddma_uzd_ibuff_ctrl_reg.ibuf_v_size = out_Size->nLength;

	if (in_Size->nLength > out_Size->nLength) {    // V scale-down
//			ich2_uzd_Ctrl0_REG.buffer_mode = 0x02;
		Vini = 0x80;// actual 0x180;
		//i3ddma_uzd_initial_value_reg.ver_ini = Vini;
		//i3ddma_uzd_initial_value_reg.ver_ini_l = 0x80;
		i3ddma_uzd_initial_value_reg.ver_ini = 0x80;
		i3ddma_uzd_initial_int_value_reg.ver_ini_int = 1;
		//i3ddma_uzd_initial_int_value_reg.ver_ini_int_l = 1;

		//nFactor = (unsigned int)((ptInSize->nLength << 20) - (Vini<<12)) / (ptOutSize->nLength - 1);
		//frank@0108 add for code exception
		if ( out_Size->nLength == 0 ) {
			rtd_pr_i3ddma_debug("output length = 0 !!!\n");
			nFactor = 0;
		} else {
			//nFactor = (unsigned int)((in_Size->nLength << 21)) / (out_Size->nLength);
			//modify the calculation for V-scale down @Crixus 20160804
			nFactor = (unsigned int)((in_Size->nLength * 1024 * 1024)) / (out_Size->nLength);
		}
		//nFactor = SHR(nFactor+1, 1); //rounding
		i3ddma_vsd_Scale_Ver_Factor_reg.ver_fac = nFactor;
		I3DDMA_ultrazoom_set_ver_ini_desire(
				(h3ddma_hsd_i3ddma0_uzd_initial_value_RBUS*)&i3ddma_uzd_initial_value_reg ,
				(h3ddma_hsd_i3ddma0_uzd_initial_int_value_RBUS*)&i3ddma_uzd_initial_int_value_reg ,
			in_Size->nLength , nFactor , out_Size->nLength );
	}else{
//			ich2_uzd_Ctrl0_REG.buffer_mode = 0;
		i3ddma_vsd_Scale_Ver_Factor_reg.ver_fac = 0x100000;
	}
	IDDMA_ultrazoom_auto_four_tap(dev);

//	ich2_uzd_Ctrl0_REG.output_fmt = 1;
//	CLR_422_Fr_SD();

	IoReg_Write32(iddma_uzd_ctrl_reg_addr[dev], i3ddma_uzd_ctrl0_reg.regValue);
	IoReg_Write32(iddma_uzd_hor_factor_reg_addr[dev], i3ddma_hsd_scale_hor_factor_reg.regValue);
	IoReg_Write32(iddma_uzd_ver_factor_reg_addr[dev], i3ddma_vsd_Scale_Ver_Factor_reg.regValue);
	IoReg_Write32(iddma_uzd_init_value_reg_addr[dev], i3ddma_uzd_initial_value_reg.regValue);
	IoReg_Write32(iddma_uzd_init_int_reg_addr[dev], i3ddma_uzd_initial_int_value_reg.regValue);
	IoReg_Write32(iddma_uzd_ibuff_ctrl_reg_addr[dev], i3ddma_uzd_ibuff_ctrl_reg.regValue);

	return;
}
#endif


void I3DDMA_ultrazoom_config_scaling_down(SIZE* in_Size, SIZE* out_Size, unsigned char panorama)
{
	// Scale down setup for Channel1
	h3ddma_hsd_i3ddma0_uzd_ctrl0_RBUS i3ddma_uzd_ctrl0_reg;
	
	i3ddma_uzd_ctrl0_reg.regValue = IoReg_Read32(H3DDMA_HSD_I3DDMA0_UZD_CTRL0_reg);
	i3ddma_uzd_ctrl0_reg.h_zoom_en = (in_Size->nWidth > out_Size->nWidth);
	
	i3ddma_uzd_ctrl0_reg.v_zoom_en = (in_Size->nLength > out_Size->nLength);
	
	i3ddma_uzd_ctrl0_reg.sort_fmt = (Scaler_DispGetStatus(SLR_SUB_DISPLAY,SLR_DISP_422CAP) ? 0 : 1);
	i3ddma_uzd_ctrl0_reg.video_comp_en = (Scaler_DispGetStatus(SLR_SUB_DISPLAY,SLR_DISP_COMP) ? 1 : 0 );
	i3ddma_uzd_ctrl0_reg.truncationctrl = (Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_10BIT)? 0: 1);
	IoReg_Write32(H3DDMA_HSD_I3DDMA0_UZD_CTRL0_reg, i3ddma_uzd_ctrl0_reg.regValue);

	I3DDMA_ultrazoom_set_scale_down(in_Size, out_Size, panorama);

	return;
}


#ifdef CONFIG_ENABLE_HDMI_NN
extern I3DDMA_NN_SUPPORT_CAP_MODE_E g_enSupCapMode;
void I3DDMA_ultrazoom_config_scaling_down_for_cap(unsigned char dev, SIZE* in_Size, SIZE* out_Size, unsigned char panorama)
{
	// Scale down setup for Channel1
	h3ddma_hsd_i3ddma1_uzd_ctrl0_RBUS i3ddma_uzd_ctrl0_reg;

	h3ddma_hsd_i3ddma1_uzd_v_db_ctrl_RBUS h3ddma_hsd_i3ddma_v_db_ctrl_reg;
	h3ddma_hsd_i3ddma1_uzd_h_db_ctrl_RBUS h3ddma_hsd_i3ddma_h_db_ctrl_reg;

	//disable vsd double buffer
	h3ddma_hsd_i3ddma_v_db_ctrl_reg.regValue = IoReg_Read32(iddma_uzd_v_db_reg_addr[dev]);
	h3ddma_hsd_i3ddma_v_db_ctrl_reg.v_db_en = 0;
	IoReg_Write32(iddma_uzd_v_db_reg_addr[dev], h3ddma_hsd_i3ddma_v_db_ctrl_reg.regValue);

	//disable vsd double buffer
	h3ddma_hsd_i3ddma_h_db_ctrl_reg.regValue = IoReg_Read32(iddma_uzd_h_db_reg_addr[dev]);
	h3ddma_hsd_i3ddma_h_db_ctrl_reg.h_db_en = 0;
	IoReg_Write32(iddma_uzd_h_db_reg_addr[dev], h3ddma_hsd_i3ddma_h_db_ctrl_reg.regValue);

	i3ddma_uzd_ctrl0_reg.regValue = IoReg_Read32(iddma_uzd_ctrl_reg_addr[dev]);
	//i3ddma_hsd_ctrl0_reg.h_zoom_en = (in_Size->nWidth > out_Size->nWidth);
	// Test, force uzd
	i3ddma_uzd_ctrl0_reg.h_zoom_en = (in_Size->nWidth > out_Size->nWidth);
	i3ddma_uzd_ctrl0_reg.v_zoom_en = (in_Size->nLength > out_Size->nLength);

	i3ddma_uzd_ctrl0_reg.sort_fmt = 0;
	i3ddma_uzd_ctrl0_reg.video_comp_en = 0;
	i3ddma_uzd_ctrl0_reg.truncationctrl = 0; // fixme 6982
	i3ddma_uzd_ctrl0_reg.cutout_en = (((g_enSupCapMode == I3DDMA_CROP_CAP_ONLY) && (IDDMA_Get_DMAWorkMode(dev) == IDDMA_NN_MODE)) ? 1 : 0);
	IoReg_Write32(iddma_uzd_ctrl_reg_addr[dev], i3ddma_uzd_ctrl0_reg.regValue);

	I3DDMA_ultrazoom_set_scale_down_for_cap(dev, in_Size, out_Size, panorama);
}
#endif


