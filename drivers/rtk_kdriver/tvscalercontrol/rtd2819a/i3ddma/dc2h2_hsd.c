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

#include <rbus/dc2h2_scaledown_reg.h>

#include <rbus/dc2h2_r2y_dither_4xxto4xx_reg.h>
//#include <rbus/scaler/rbusPpOverlayReg.h>
#include <rbus/ppoverlay_reg.h>
#include <rtd_log/rtd_module_log.h>
#include <tvscalercontrol/i3ddma/i3ddma.h>
#include <tvscalercontrol/i3ddma/i3ddma_uzd.h>

#ifndef VPQ_SET_REG_FIELD
#define VPQ_SET_REG_FIELD(REG,FIELD,VALUE) do{ (REG) = ((REG) & ~((unsigned int)FIELD##_mask)) + (FIELD((unsigned int)VALUE)); } while(0);
#endif

extern unsigned short i3_tRGB2YUV_COEF_709_RGB_0_255[];
#if 0//no used
extern unsigned short i3_tRGB2YUV_COEF_709_RGB_0_255_OSD_MIXER2[];

extern unsigned short i3_tRGB2YUV_COEF_709_RGB_0_255_SUB_OSD_MIXER1[];
#endif

extern short *tScaleDown_COEF_TAB[];

void DC2H2_color_colorspacergb2yuvtransfer(DISPD_CAP_SRC idmaDispD_in_sel)
{
	unsigned short *table_index = 0;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_rgb2yuv_ctrl_RBUS  i3ddma_rgb2yuv_ctrl_reg;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_rgb2yuv_tab_m11_m12_RBUS tab2_M11_M12_REG;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_rgb2yuv_tab_m13_m21_RBUS tab2_M13_M21_REG;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_rgb2yuv_tab_m22_m23_RBUS tab2_M22_M23_REG;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_rgb2yuv_tab_m31_m32_RBUS tab2_M31_M32_REG;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_rgb2yuv_tab_m33_ygain_RBUS tab2_M33_YGain_REG;
	dc2h2_r2y_dither_4xxto4xx_dc2h2_rgb2yuv_tab_yo_RBUS tab2_Yo_REG;
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
	IoReg_Write32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_TAB_M11_M12_reg, tab2_M11_M12_REG.regValue);

	tab2_M13_M21_REG.m13 = table_index [_RGB2YUV_m13];
	tab2_M13_M21_REG.m21 = table_index [_RGB2YUV_m21];
	IoReg_Write32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_TAB_M13_M21_reg, tab2_M13_M21_REG.regValue);

	tab2_M22_M23_REG.m22 = table_index [_RGB2YUV_m22];
	tab2_M22_M23_REG.m23 = table_index [_RGB2YUV_m23];
	//rtd_pr_i3ddma_debug("i3ddma_rgb2yuv_tab1_m2_reg.regValue = %x\n", i3ddma_rgb2yuv_tab1_m2_reg.regValue);
	IoReg_Write32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_TAB_M22_M23_reg, tab2_M22_M23_REG.regValue);

	tab2_M31_M32_REG.m31 = table_index [_RGB2YUV_m31];
	tab2_M31_M32_REG.m32 = table_index [_RGB2YUV_m32];
	IoReg_Write32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_TAB_M31_M32_reg, tab2_M31_M32_REG.regValue);

	tab2_M33_YGain_REG.m33 = table_index [_RGB2YUV_m33];
	tab2_M33_YGain_REG.y_gain= table_index [_RGB2YUV_Y_gain];
	//rtd_pr_i3ddma_debug("i3ddma_rgb2yuv_tab1_m3_reg.regValue = %x\n", i3ddma_rgb2yuv_tab1_m3_reg.regValue);
	IoReg_Write32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_TAB_M33_YGain_reg, tab2_M33_YGain_REG.regValue);

	tab2_Yo_REG.yo_odd= table_index [_RGB2YUV_Yo_odd];
	IoReg_Write32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_TAB_Yo_reg, tab2_Yo_REG.regValue);

	i3ddma_rgb2yuv_ctrl_reg.regValue = IoReg_Read32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_CTRL_reg);

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

	IoReg_Write32(DC2H2_R2Y_DITHER_4XXTO4XX_DC2H2_RGB2YUV_CTRL_reg, i3ddma_rgb2yuv_ctrl_reg.regValue);

	return;
}

void DC2H2_ultrazoom_auto_four_tap(void)
{
	unsigned int z_RBUS_reg;
	z_RBUS_reg = IoReg_Read32( DC2H2_SCALEDOWN_DC2H2_UZD_CTRL1_reg );
	VPQ_SET_REG_FIELD( z_RBUS_reg , DC2H2_SCALEDOWN_DC2H2_UZD_CTRL1_auto_four_tap_en , 1 );
	IoReg_Write32( DC2H2_SCALEDOWN_DC2H2_UZD_CTRL1_reg , z_RBUS_reg );
}

void DC2H2_ultrazoom_coef_db_apply(void)
{
	unsigned int z_RBUS_reg;
	z_RBUS_reg = IoReg_Read32( DC2H2_SCALEDOWN_DC2H2_UZD_COEF_DB_CTRL_reg );
	VPQ_SET_REG_FIELD( z_RBUS_reg , DC2H2_SCALEDOWN_DC2H2_UZD_COEF_DB_CTRL_coef_db_apply , 1 );
	IoReg_Write32( DC2H2_SCALEDOWN_DC2H2_UZD_COEF_DB_CTRL_reg , z_RBUS_reg );
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
void DC2H2_ultrazoom_set_scale_down(SIZE* in_Size, SIZE* out_Size, unsigned char panorama)
{
    unsigned char SDRatio;
    unsigned char SDFilter=0;
    unsigned int tmp_data;
    short *coef_pt;
    unsigned char i;
    unsigned int nFactor;
    unsigned char Hini, Vini;

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    dc2h2_scaledown_dc2h2_uzd_ctrl0_RBUS				i3ddma_uzd_ctrl0_reg;
    dc2h2_scaledown_dc2h2_uzd_scale_hor_factor_RBUS	i3ddma_hsd_scale_hor_factor_reg;
    dc2h2_scaledown_dc2h2_uzd_scale_ver_factor_RBUS   	i3ddma_vsd_Scale_Ver_Factor_reg;
    dc2h2_scaledown_dc2h2_uzd_initial_value_RBUS		i3ddma_uzd_initial_value_reg;
    dc2h2_scaledown_dc2h2_uzd_initial_int_value_RBUS		i3ddma_uzd_initial_int_value_reg;
    dc2h2_scaledown_dc2h2_uzd_ibuff_ctrl_RBUS		i3ddma_uzd_ibuff_ctrl_reg;
	
    i3ddma_uzd_ctrl0_reg.regValue		= IoReg_Read32(DC2H2_SCALEDOWN_DC2H2_UZD_CTRL0_reg);
    i3ddma_hsd_scale_hor_factor_reg.regValue	= IoReg_Read32(DC2H2_SCALEDOWN_DC2H2_UZD_SCALE_HOR_FACTOR_reg);
    i3ddma_vsd_Scale_Ver_Factor_reg.regValue = IoReg_Read32(DC2H2_SCALEDOWN_DC2H2_UZD_SCALE_VER_FACTOR_reg);
   // i3ddma_hsd_hor_delta1_reg.regValue	= IoReg_Read32(DC2H2_SCALEDOWN_DC2H2_UZD_HSD_Hor_Delta1_reg);
    i3ddma_uzd_initial_value_reg.regValue	= IoReg_Read32(DC2H2_SCALEDOWN_DC2H2_UZD_INITIAL_VALUE_reg);
    i3ddma_uzd_initial_int_value_reg.regValue = IoReg_Read32(DC2H2_SCALEDOWN_DC2H2_UZD_INITIAL_INT_VALUE_reg);
    i3ddma_uzd_ibuff_ctrl_reg.regValue = IoReg_Read32(DC2H2_SCALEDOWN_DC2H2_UZD_IBUFF_CTRL_reg);
	
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
            IoReg_Write32(DC2H2_SCALEDOWN_DC2H2_UZD_FIR_COEF_TAB1_C0_reg+ i*4, tmp_data);
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
			IoReg_Write32(DC2H2_SCALEDOWN_DC2H2_UZD_FIR_COEF_TAB2_C0_reg+ i*4, tmp_data);
		}

	}
	DC2H2_ultrazoom_coef_db_apply();

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
	}else{
//			ich2_uzd_Ctrl0_REG.buffer_mode = 0;
		i3ddma_vsd_Scale_Ver_Factor_reg.ver_fac = 0x100000;
	}

//	ich2_uzd_Ctrl0_REG.output_fmt = 1;
//	CLR_422_Fr_SD();
	DC2H2_ultrazoom_auto_four_tap();

	IoReg_Write32(DC2H2_SCALEDOWN_DC2H2_UZD_CTRL0_reg, i3ddma_uzd_ctrl0_reg.regValue);
	IoReg_Write32(DC2H2_SCALEDOWN_DC2H2_UZD_SCALE_HOR_FACTOR_reg, i3ddma_hsd_scale_hor_factor_reg.regValue);
	IoReg_Write32(DC2H2_SCALEDOWN_DC2H2_UZD_SCALE_VER_FACTOR_reg, i3ddma_vsd_Scale_Ver_Factor_reg.regValue);
	IoReg_Write32(DC2H2_SCALEDOWN_DC2H2_UZD_INITIAL_VALUE_reg, i3ddma_uzd_initial_value_reg.regValue);
	IoReg_Write32(DC2H2_SCALEDOWN_DC2H2_UZD_INITIAL_INT_VALUE_reg, i3ddma_uzd_initial_int_value_reg.regValue);
	IoReg_Write32(DC2H2_SCALEDOWN_DC2H2_UZD_IBUFF_CTRL_reg, i3ddma_uzd_ibuff_ctrl_reg.regValue);

	return;
}

void DC2H2_ultrazoom_config_scaling_down(SIZE* in_Size, SIZE* out_Size, unsigned char panorama)
{
#ifdef CONFIG_ENABLE_HDMI3_0_NN
	rtd_pr_i3ddma_emerg("Don't do DC2H2_ultrazoom_config_scaling_down\n");
#else
	// Scale down setup for Channel1
	dc2h2_scaledown_dc2h2_uzd_ctrl0_RBUS i3ddma_uzd_ctrl0_reg;
	
	i3ddma_uzd_ctrl0_reg.regValue = IoReg_Read32(DC2H2_SCALEDOWN_DC2H2_UZD_CTRL0_reg);
	i3ddma_uzd_ctrl0_reg.h_zoom_en = (in_Size->nWidth > out_Size->nWidth);
	
	i3ddma_uzd_ctrl0_reg.v_zoom_en = (in_Size->nLength > out_Size->nLength);
	
	i3ddma_uzd_ctrl0_reg.sort_fmt = (Scaler_DispGetStatus(SLR_SUB_DISPLAY,SLR_DISP_422CAP) ? 0 : 1);
	i3ddma_uzd_ctrl0_reg.video_comp_en = (Scaler_DispGetStatus(SLR_SUB_DISPLAY,SLR_DISP_COMP) ? 1 : 0 );
	i3ddma_uzd_ctrl0_reg.truncationctrl = (Scaler_DispGetStatus(SLR_SUB_DISPLAY, SLR_DISP_10BIT)? 0: 1);
	IoReg_Write32(DC2H2_SCALEDOWN_DC2H2_UZD_CTRL0_reg, i3ddma_uzd_ctrl0_reg.regValue);

	DC2H2_ultrazoom_set_scale_down(in_Size, out_Size, panorama);
#endif
	return;
}
