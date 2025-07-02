/*============================================================================
* Copyright (c)      Realtek Semiconductor Corporation, 2024
* All rights reserved.
* ===========================================================================*/

//----------------------------------------------------------------------------
// ID Code      : txpre_memory.c
// Update Note  :
//----------------------------------------------------------------------------

#include <scaler/scalerCommon.h>

#include <rbus/mdom_hdmitx_cap_reg.h>
#include <rbus/mdom_hdmitx_disp_reg.h>

#include <linux/pageremap.h>
#include <mach/rtk_platform.h>

#include <rtd_log/rtd_module_log.h>

#include <tvscalercontrol/io/ioregdrv.h>
#include <tvscalercontrol/txpre/txpre_memory.h>

//****************************************************************************
// Implementation function define
//****************************************************************************
static unsigned int txpre_buffer_number = 1;
static unsigned int txpre_buffer_size;
static unsigned char txpre_compression_bits;
static unsigned char txpre_compression_mode;
static unsigned char txpre_compression_enable;

#define TXPRE_LINE_MODE 0
#define TXPRE_FRAME_MODE 1
#define TXPRE_BURST_LENGTH 0x10

#define TXPRE_MEM_444 0
#define TXPRE_MEM_422 1
#define TXPRE_MEM_8_BIT 8
#define TXPRE_MEM_10_BIT 10
#define TXPRE_MEM_12_BIT 12

#define COLOR_RGB     0
#define COLOR_YUV422  1
#define COLOR_YUV444  2
#define COLOR_YUV420  3
#define COLOR_YUV400  4
#define COLOR_UNKNOW  5
 
#define HDMITX_8BIT 0
#define HDMITX_10BIT 1
#define HDMITX_12BIT 2
#define HDMITX_16BIT 3

#define TXPRE_CAP_WID_MAX_5K 5120
#define TXPRE_CAP_LEN_MAX_5K 2880
#define TXPRE_CAP_WID_MAX 3840
#define TXPRE_CAP_LEN_MAX 2160

#define TXPRE_BUF_NUM_MAX 8
TXPRE_MEMORY_BUFFER_T TxpreMdomainBuffer[TXPRE_BUF_NUM_MAX] = {0};

unsigned int txpre_memory_get_buffer_start_address(unsigned char buf_idx)
{
    if (buf_idx >= TXPRE_BUF_NUM_MAX){
        buf_idx = TXPRE_BUF_NUM_MAX - 1;
    }
        
	return TxpreMdomainBuffer[buf_idx].phyaddr;
}

void txpre_memory_set_buffer_start_address(unsigned char buf_idx, unsigned int addr)
{
    if (buf_idx >= TXPRE_BUF_NUM_MAX)
    {
        buf_idx = TXPRE_BUF_NUM_MAX - 1;
    }
	TxpreMdomainBuffer[buf_idx].phyaddr = addr;
}

unsigned char txpre_memory_compression_get_enable(void)
{
	return txpre_compression_enable;
}

void txpre_memory_compression_set_enable(unsigned char enable)
{
	txpre_compression_enable = enable;
}

void txpre_memory_set_compression_bits(unsigned char comp_bits)
{
	txpre_compression_bits=comp_bits;
}

unsigned char txpre_memory_get_compression_bits(void)
{
	return txpre_compression_bits;
}

void txpre_memory_set_capture_mode(unsigned char compression_mode)
{
    txpre_compression_mode = compression_mode;
}

unsigned char txpre_memory_get_capture_mode(void)
{
    return txpre_compression_mode;
}

unsigned int txpre_memory_get_buffer_number(void)
{
    return txpre_buffer_number;
}

void txpre_memory_set_buffer_number(unsigned int buffer_number)
{
    txpre_buffer_number = buffer_number;
}

unsigned int txpre_memory_get_buffer_size(void)
{
    return txpre_buffer_size;
}

void txpre_memory_set_buffer_size(unsigned int buffer_size)
{
    txpre_buffer_size = buffer_size;
}

unsigned int txpre_memory_get_pixel_size(StructTxPreInfo * txpre_info){
    unsigned int PixelSize;

   if((txpre_info->Input_Color_Space == COLOR_YUV444) || (txpre_info->Input_Color_Space == COLOR_RGB)){
        if (txpre_info->Input_Color_Depth == HDMITX_12BIT){
            PixelSize = 36;
        } else if(txpre_info->Input_Color_Depth == HDMITX_10BIT) {		// 4:4:4 format     10bit
            PixelSize = 30;
        } else {			// 4:4:4 format     8bit
            PixelSize = 24;
        }
    }else{
        if (txpre_info->Input_Color_Depth == HDMITX_12BIT){
            PixelSize = 24;
        } else if(txpre_info->Input_Color_Depth == HDMITX_10BIT) {		// 4:2:2 format     10bit
            PixelSize = 20;
        } else {			// 4:2:2 format     8bit
            PixelSize = 16;
        }
    }
    rtd_pr_scaler_memory_emerg("[TXPRE] PixelSize=%d\n", PixelSize);
    return PixelSize;
}

unsigned int drvif_memory_get_data_align(unsigned int Value, unsigned int unit);
unsigned int txpre_memory_get_capture_line_size_compression(unsigned int capWid, unsigned char compression_bits)
{
	unsigned short cmp_line_sum_bit, sub_pixel_num;
	unsigned int cmp_line_sum_bit_pure_rgb, cmp_line_sum_bit_pure_a, cmp_line_sum_bit_32_dummy, cmp_line_sum_bit_128_dummy, cmp_line_sum_bit_real;
    unsigned int cmp_width_div32 = 0, frame_limit_bit = 0;

	rtd_pr_scaler_memory_emerg("[TXPRE] compression_bits=%x, capWid=%x\n", compression_bits, capWid);

	//width need to align 32 for new PQC @Crixus 20170615
	if((capWid % 32) != 0)
		capWid = capWid + (32 - (capWid % 32));

    cmp_width_div32 = capWid / 32;
    frame_limit_bit = compression_bits << 2;

    sub_pixel_num = 3;
    cmp_line_sum_bit_pure_rgb = (cmp_width_div32 * 32) * (frame_limit_bit>>2);
    cmp_line_sum_bit_pure_a = 0;
    cmp_line_sum_bit_32_dummy = (6+1) * 32 * sub_pixel_num;
    cmp_line_sum_bit_128_dummy = 0;
    cmp_line_sum_bit_real = cmp_line_sum_bit_pure_rgb + cmp_line_sum_bit_pure_a + cmp_line_sum_bit_32_dummy + cmp_line_sum_bit_128_dummy;
    cmp_line_sum_bit = ((cmp_line_sum_bit_real+128)>>8)*2;//ceil((cmp_line_sum_bit_real)/128)*2;

    //cmp_line_sum_bit = drvif_memory_get_data_align(cmp_line_sum_bit, 128);// (pixels of per line * bits / 64), unit: 64bits
	//cmp_line_sum_bit = (unsigned int)SHR(cmp_line_sum_bit, 7);

	rtd_pr_scaler_memory_info("[TXPRE] txpre_memory_get_capture_line_size_compression LineSize=%d\n", cmp_line_sum_bit);

	return cmp_line_sum_bit;
}

void txpre_memory_comp_setting(StructTxPreInfo * txpre_info, unsigned char enable_compression, unsigned int comp_wid, unsigned int comp_len, unsigned char comp_ratio)
{
    mdom_hdmitx_cap_dispm2_pq_cmp_RBUS mdom_hdmitx_cap_dispm2_pq_cmp_reg;
    mdom_hdmitx_cap_dispm2_pq_cmp_pair_RBUS mdom_hdmitx_cap_dispm2_pq_cmp_pair_reg;
    mdom_hdmitx_cap_dispm2_pq_cmp_bit_RBUS mdom_hdmitx_cap_dispm2_pq_cmp_bit_reg;
    mdom_hdmitx_cap_dispm2_pq_cmp_enable_RBUS mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg;
    mdom_hdmitx_cap_dispm2_pq_cmp_allocate_RBUS mdom_hdmitx_cap_dispm2_pq_cmp_allocate_reg;
    mdom_hdmitx_cap_dispm2_pq_cmp_poor_RBUS mdom_hdmitx_cap_dispm2_pq_cmp_poor_reg;
    mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add0_RBUS mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add0_reg;
    mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add1_RBUS mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add1_reg;
    mdom_hdmitx_cap_dispm2_pq_cmp_balance_RBUS mdom_hdmitx_cap_dispm2_pq_cmp_balance_reg;
    mdom_hdmitx_cap_dispm2_pq_cmp_smooth_RBUS mdom_hdmitx_cap_dispm2_pq_cmp_smooth_reg;
    mdom_hdmitx_cap_dispm2_pq_cmp_guarantee_RBUS mdom_hdmitx_cap_dispm2_pq_cmp_guarantee_reg;
    mdom_hdmitx_cap_dispm2_pq_cmp_bit_llb_RBUS mdom_hdmitx_cap_dispm2_pq_cmp_bit_llb_reg;
    mdom_hdmitx_cap_dispm2_pq_cmp_st_RBUS mdom_hdmitx_cap_dispm2_pq_cmp_st_reg;

    mdom_hdmitx_disp_disp2_pq_decmp_RBUS mdom_hdmitx_disp_disp2_pq_decmp_reg;
    mdom_hdmitx_disp_disp2_pq_decmp_pair_RBUS mdom_hdmitx_disp_disp2_pq_decmp_pair_reg;
    mdom_hdmitx_disp_disp2_pq_decmp_sat_en_RBUS mdom_hdmitx_disp_disp2_pq_decmp_sat_en_reg;

    unsigned short cmp_line_sum_bit;
    unsigned int cmp_width_div32 = 0, frame_limit_bit = 0;
    unsigned int llb_tolerance = 0, llb_x_blk_sta = 0, Llb_init = 0, Llb_step = 0;

    if(enable_compression == TRUE){

        if((comp_wid % 32) != 0){
            //drop bits
            //IoReg_Write32(MDOMAIN_DISP_DDR_MainAddrDropBits_reg, (32 - (comp_wid % 32)));
            comp_wid = comp_wid + (32 - (comp_wid % 32));
        }

        /*=======================PQC Setting======================================*/
        cmp_width_div32 = comp_wid / 32;
        frame_limit_bit = comp_ratio << 2;
        //capture compression setting
        mdom_hdmitx_cap_dispm2_pq_cmp_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_reg);
        mdom_hdmitx_cap_dispm2_pq_cmp_reg.cmp_height = comp_len;//set length
        mdom_hdmitx_cap_dispm2_pq_cmp_reg.cmp_width_div32 = comp_wid / 32;//set width
        IoReg_Write32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_reg, mdom_hdmitx_cap_dispm2_pq_cmp_reg.regValue);

        //compression bits setting
        mdom_hdmitx_cap_dispm2_pq_cmp_bit_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_BIT_reg);
        mdom_hdmitx_cap_dispm2_pq_cmp_bit_reg.frame_limit_bit = comp_ratio*4;
        mdom_hdmitx_cap_dispm2_pq_cmp_bit_reg.block_limit_bit = 0x3f;//PQC fine tune setting
        mdom_hdmitx_cap_dispm2_pq_cmp_bit_reg.first_line_more_bit = 0x20;
        IoReg_Write32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_BIT_reg, mdom_hdmitx_cap_dispm2_pq_cmp_bit_reg.regValue);

        //compression other setting
        mdom_hdmitx_cap_dispm2_pq_cmp_pair_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_PAIR_reg);
        mdom_hdmitx_cap_dispm2_pq_cmp_pair_reg.cmp_data_bit_width = 1; //10 bits

        mdom_hdmitx_cap_dispm2_pq_cmp_pair_reg.cmp_data_color = 1; // YUV
        if (txpre_info->Input_Color_Space == COLOR_YUV422)
        {
            mdom_hdmitx_cap_dispm2_pq_cmp_pair_reg.cmp_data_format = 1; // 422
        }
        else
        {
            mdom_hdmitx_cap_dispm2_pq_cmp_pair_reg.cmp_data_format = 0; // 422
        }
        mdom_hdmitx_cap_dispm2_pq_cmp_pair_reg.cmp_pair_para = 1;

        cmp_line_sum_bit = txpre_memory_get_capture_line_size_compression(comp_wid, comp_ratio);

        // Both line mode & frame mode need to set cmp_line_sum_bit;
        mdom_hdmitx_cap_dispm2_pq_cmp_pair_reg.cmp_line_sum_bit = cmp_line_sum_bit;

        if(txpre_memory_get_capture_mode() == TXPRE_LINE_MODE){
            //use line mode
            mdom_hdmitx_cap_dispm2_pq_cmp_pair_reg.cmp_line_mode = 1;
        }
        else{
            //use frame mode
            mdom_hdmitx_cap_dispm2_pq_cmp_pair_reg.cmp_line_mode = 0;
        }

        mdom_hdmitx_cap_dispm2_pq_cmp_pair_reg.two_line_prediction_en = 1;
        mdom_hdmitx_cap_dispm2_pq_cmp_pair_reg.cmp_qp_mode = 0;
        mdom_hdmitx_cap_dispm2_pq_cmp_pair_reg.cmp_jump4_en = 1;
        mdom_hdmitx_cap_dispm2_pq_cmp_pair_reg.cmp_jump6_en = 1;
        IoReg_Write32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_PAIR_reg, mdom_hdmitx_cap_dispm2_pq_cmp_pair_reg.regValue);

        //compression llb setting
        llb_tolerance = 6;
        llb_x_blk_sta = 0;		
        Llb_init = frame_limit_bit * 4 + llb_tolerance * 32;
        Llb_step = (Llb_init *16 - frame_limit_bit * 64) / cmp_width_div32;
        mdom_hdmitx_cap_dispm2_pq_cmp_bit_llb_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_BIT_LLB_reg);
        mdom_hdmitx_cap_dispm2_pq_cmp_bit_llb_reg.llb_x_blk_sta = llb_x_blk_sta;
        mdom_hdmitx_cap_dispm2_pq_cmp_bit_llb_reg.llb_init = Llb_init;
        mdom_hdmitx_cap_dispm2_pq_cmp_bit_llb_reg.llb_step = Llb_step;
        IoReg_Write32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_BIT_LLB_reg, mdom_hdmitx_cap_dispm2_pq_cmp_bit_llb_reg.regValue);

        //compression st setting
        mdom_hdmitx_cap_dispm2_pq_cmp_st_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_ST_reg);
        //mdom_hdmitx_cap_dispm2_pq_cmp_st_reg.cmp_readro_sel = //default;
        IoReg_Write32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_ST_reg, mdom_hdmitx_cap_dispm2_pq_cmp_st_reg.regValue);

        //compression PQC constrain @Crixus 20170626
        mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_ENABLE_reg);
        if(txpre_memory_get_capture_mode() == TXPRE_LINE_MODE){
            mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.first_line_more_en = 0;
        }
        else{
            mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.first_line_more_en = 1;
        }
        //RGB444 setting for PQC @Crixus 20170718
        if (txpre_info->Input_Color_Space == COLOR_YUV422){

            mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.g_ratio = 16;
            mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.do_422_mode = 1;//422 format
        }
        else{
            mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.g_ratio = 14;
            mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.do_422_mode = 0;//444 format
        }

        mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.first_predict_nc_en = 1;
        mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.blk0_add_en = 1;
        mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.blk0_add_half_en = 1;
        mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.add_last2blk_over_curve_bit = 1;
        mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.balance_en = 1;
        mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.variation_maxmin_en = 1;
        mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.dynamic_allocate_ratio_en = 1;
        mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.dynamic_sum_line_rst = 1;
        mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.not_rich_do_422_en = 1;
        mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.rb_lossy_do_422_en = 1;
        mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.not_enough_bit_do_422_en = 1;
        mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.poor_limit_min_qp_en = 1;
        mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.first_predict_nc_mode = 1;
        mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.overcurve_lossy_en = 1;
        //mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.master_qp_en = 1;//fix work around need to check by dic
        IoReg_Write32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_ENABLE_reg, mdom_hdmitx_cap_dispm2_pq_cmp_enable_reg.regValue);

        mdom_hdmitx_cap_dispm2_pq_cmp_allocate_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_ALLOCATE_reg);
        //RGB444 setting for PQC @Crixus 20170718
        if (txpre_info->Input_Color_Space == COLOR_YUV422){
            mdom_hdmitx_cap_dispm2_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 16;
            mdom_hdmitx_cap_dispm2_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 13;
        }
        else{
            mdom_hdmitx_cap_dispm2_pq_cmp_allocate_reg.dynamic_allocate_ratio_max = 16;
            mdom_hdmitx_cap_dispm2_pq_cmp_allocate_reg.dynamic_allocate_ratio_min = 13;
        }

        mdom_hdmitx_cap_dispm2_pq_cmp_allocate_reg.dynamic_allocate_less = 4;
        mdom_hdmitx_cap_dispm2_pq_cmp_allocate_reg.dynamic_allocate_more = 2;
        mdom_hdmitx_cap_dispm2_pq_cmp_allocate_reg.dynamic_allocate_line = 2;
        IoReg_Write32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_ALLOCATE_reg, mdom_hdmitx_cap_dispm2_pq_cmp_allocate_reg.regValue);

        mdom_hdmitx_cap_dispm2_pq_cmp_poor_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_POOR_reg);
        mdom_hdmitx_cap_dispm2_pq_cmp_poor_reg.poor_limit_th_qp4=12;
        mdom_hdmitx_cap_dispm2_pq_cmp_poor_reg.poor_limit_th_qp3=9;
        mdom_hdmitx_cap_dispm2_pq_cmp_poor_reg.poor_limit_th_qp2=4;
        mdom_hdmitx_cap_dispm2_pq_cmp_poor_reg.poor_limit_th_qp1=3;

        IoReg_Write32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_POOR_reg, mdom_hdmitx_cap_dispm2_pq_cmp_poor_reg.regValue);

        //Update PQC constrain setting for PQC @Crixus 20170725
        mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add0_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_BLK0_ADD0_reg);
        mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add1_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_BLK0_ADD1_reg);
        if(comp_ratio == 10){// minmal compression bit setting
            mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add0_reg.blk0_add_value32 = 1;
            mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add0_reg.blk0_add_value16 = 1;
            mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add1_reg.blk0_add_value8 = 1;
            mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add1_reg.blk0_add_value4 = 1;
            mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add1_reg.blk0_add_value2 = 1;
            mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add1_reg.blk0_add_value1 = 0;
            //mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add0_reg.dic_mode_tolerance = 0;
        }
        else{
            mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add0_reg.blk0_add_value32 = 16;
            mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add0_reg.blk0_add_value16 = 8;
            mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add0_reg.blk0_add_value0 = 0;
            //mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add0_reg.dic_mode_tolerance = 0;

            mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add1_reg.blk0_add_value8 = 4;
            mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add1_reg.blk0_add_value4 = 2;
            mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add1_reg.blk0_add_value2 = 1;
            mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add1_reg.blk0_add_value1 = 1;
        }
        IoReg_Write32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_BLK0_ADD0_reg, mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add0_reg.regValue);
        IoReg_Write32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_BLK0_ADD1_reg, mdom_hdmitx_cap_dispm2_pq_cmp_blk0_add1_reg.regValue);


        mdom_hdmitx_cap_dispm2_pq_cmp_balance_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_BALANCE_reg);
        mdom_hdmitx_cap_dispm2_pq_cmp_balance_reg.balance_rb_ov_th = 0;
        mdom_hdmitx_cap_dispm2_pq_cmp_balance_reg.balance_rb_ud_th = 3;
        mdom_hdmitx_cap_dispm2_pq_cmp_balance_reg.balance_rb_give = 3;
        mdom_hdmitx_cap_dispm2_pq_cmp_balance_reg.balance_g_ud_th = 3;
        mdom_hdmitx_cap_dispm2_pq_cmp_balance_reg.balance_g_ov_th = 0;
        mdom_hdmitx_cap_dispm2_pq_cmp_balance_reg.balance_g_give = 3;
        mdom_hdmitx_cap_dispm2_pq_cmp_balance_reg.variation_maxmin_th = 30;
        mdom_hdmitx_cap_dispm2_pq_cmp_balance_reg.variation_maxmin_th2 = 3;
        IoReg_Write32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_BALANCE_reg, mdom_hdmitx_cap_dispm2_pq_cmp_balance_reg.regValue);


        mdom_hdmitx_cap_dispm2_pq_cmp_smooth_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_SMOOTH_reg);
        mdom_hdmitx_cap_dispm2_pq_cmp_smooth_reg.variation_near_num_th = 4;
        mdom_hdmitx_cap_dispm2_pq_cmp_smooth_reg.variation_value_th = 3;
        mdom_hdmitx_cap_dispm2_pq_cmp_smooth_reg.variation_num_th = 3;
        mdom_hdmitx_cap_dispm2_pq_cmp_smooth_reg.not_enough_bit_do_422_qp = 5;
        mdom_hdmitx_cap_dispm2_pq_cmp_smooth_reg.not_rich_do_422_th = 4;
        mdom_hdmitx_cap_dispm2_pq_cmp_smooth_reg.rb_lossy_do_422_qp_level = 0;
        //mdom_hdmitx_cap_dispm2_pq_cmp_smooth_reg.dic_mode_entry_th = 15;//Mark2 removed

        IoReg_Write32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_SMOOTH_reg, mdom_hdmitx_cap_dispm2_pq_cmp_smooth_reg.regValue);

        mdom_hdmitx_cap_dispm2_pq_cmp_guarantee_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_Guarantee_reg);
        //mdom_hdmitx_cap_dispm2_pq_cmp_guarantee_reg.dic_mode_qp_th_g = 1;//Mark2 removed
        //mdom_hdmitx_cap_dispm2_pq_cmp_guarantee_reg.dic_mode_qp_th_rb = 1;//Mark2 removed
        IoReg_Write32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_Guarantee_reg, mdom_hdmitx_cap_dispm2_pq_cmp_guarantee_reg.regValue);


        /*=======================PQDC Setting======================================*/

        //display de-compression setting
        mdom_hdmitx_disp_disp2_pq_decmp_reg.regValue = IoReg_Read32(MDOM_HDMITX_DISP_DISP2_PQ_DECMP_reg);
        mdom_hdmitx_disp_disp2_pq_decmp_reg.decmp_height = comp_len;
        mdom_hdmitx_disp_disp2_pq_decmp_reg.decmp_width_div32 = comp_wid / 32;
        //IoReg_Write32(MDOM_HDMITX_DISP_DISP2_PQ_DECMP_reg, mdom_hdmitx_disp_disp2_pq_decmp_reg.regValue);

        //de-compression bits setting
        mdom_hdmitx_disp_disp2_pq_decmp_pair_reg.regValue = IoReg_Read32(MDOM_HDMITX_DISP_DISP2_PQ_DECMP_PAIR_reg);
        mdom_hdmitx_disp_disp2_pq_decmp_pair_reg.decmp_data_bit_width = 1;//10 bits

        mdom_hdmitx_disp_disp2_pq_decmp_pair_reg.decmp_data_color = 1; // YUV
        if (txpre_info->Input_Color_Space == COLOR_YUV422)
        {
            mdom_hdmitx_disp_disp2_pq_decmp_pair_reg.decmp_data_format = 1; // 422
        }
        else
        {
            mdom_hdmitx_disp_disp2_pq_decmp_pair_reg.decmp_data_format = 0; // 444
        }

        // Both line mode & frame mode need to set cmp_line_sum_bit;
        mdom_hdmitx_disp_disp2_pq_decmp_pair_reg.decmp_line_sum_bit = cmp_line_sum_bit;

        if(txpre_memory_get_capture_mode() == TXPRE_LINE_MODE){
            //default use line mode
            mdom_hdmitx_disp_disp2_pq_decmp_pair_reg.decmp_line_mode = 1;
            //mdom_hdmitx_disp_disp2_pq_decmp_pair_reg.decmp_line_sum_bit = TotalSize;
        }
        else{
            //use frame mode
            mdom_hdmitx_disp_disp2_pq_decmp_pair_reg.decmp_line_mode = 0;
            //mdom_hdmitx_disp_disp2_pq_decmp_pair_reg.decmp_line_sum_bit = 0;
        }

        mdom_hdmitx_disp_disp2_pq_decmp_pair_reg.decmp_two_line_prediction_en = 1;
        mdom_hdmitx_disp_disp2_pq_decmp_pair_reg.decmp_qp_mode = 0;
        //mdom_hdmitx_disp_disp2_pq_decmp_pair_reg.decmp_dic_mode_en = 1; //Mark2 removed
        mdom_hdmitx_disp_disp2_pq_decmp_pair_reg.decmp_jump4_en = 1;
        mdom_hdmitx_disp_disp2_pq_decmp_pair_reg.decmp_jump6_en = 1;

        IoReg_Write32(MDOM_HDMITX_DISP_DISP2_PQ_DECMP_PAIR_reg, mdom_hdmitx_disp_disp2_pq_decmp_pair_reg.regValue);

        //enable saturation for PQ
        mdom_hdmitx_disp_disp2_pq_decmp_sat_en_reg.regValue = IoReg_Read32(MDOM_HDMITX_DISP_DISP2_PQ_DECMP_SAT_EN_reg);
        mdom_hdmitx_disp_disp2_pq_decmp_sat_en_reg.saturation_en = 1;
        //mdom_hdmitx_disp_disp2_pq_decmp_sat_en_reg.decmp_ctrl_para_1 = //reserved;
        //mdom_hdmitx_disp_disp2_pq_decmp_sat_en_reg.decmp_ctrl_para_0 = //reserved;
        mdom_hdmitx_disp_disp2_pq_decmp_sat_en_reg.saturation_type = 0;
        IoReg_Write32(MDOM_HDMITX_DISP_DISP2_PQ_DECMP_SAT_EN_reg, mdom_hdmitx_disp_disp2_pq_decmp_sat_en_reg.regValue);

        //enable compression
        mdom_hdmitx_cap_dispm2_pq_cmp_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_reg);
        mdom_hdmitx_cap_dispm2_pq_cmp_reg.cmp_en = 1;
        IoReg_Write32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_reg, mdom_hdmitx_cap_dispm2_pq_cmp_reg.regValue);

        //enable de-compression
        //mdom_hdmitx_disp_disp2_pq_decmp_reg.regValue = IoReg_Read32(MDOM_HDMITX_DISP_DISP2_PQ_DECMP_reg);
        mdom_hdmitx_disp_disp2_pq_decmp_reg.decmp_en = 1;
        rtd_pr_scaler_memory_emerg("[TXPRE] mdom_hdmitx_disp_disp2_pq_decmp_reg.regValue=%x\n", mdom_hdmitx_disp_disp2_pq_decmp_reg.regValue);

        IoReg_Write32(MDOM_HDMITX_DISP_DISP2_PQ_DECMP_reg, mdom_hdmitx_disp_disp2_pq_decmp_reg.regValue);
    }
    else{
        //disable compression
        mdom_hdmitx_cap_dispm2_pq_cmp_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_reg);
        mdom_hdmitx_cap_dispm2_pq_cmp_reg.cmp_en = 0;
        IoReg_Write32(MDOM_HDMITX_CAP_DISPM2_PQ_CMP_reg, mdom_hdmitx_cap_dispm2_pq_cmp_reg.regValue);

        //disable de-compression
        mdom_hdmitx_disp_disp2_pq_decmp_reg.regValue = IoReg_Read32(MDOM_HDMITX_DISP_DISP2_PQ_DECMP_reg);
        mdom_hdmitx_disp_disp2_pq_decmp_reg.decmp_en = 0;
        IoReg_Write32(MDOM_HDMITX_DISP_DISP2_PQ_DECMP_reg, mdom_hdmitx_disp_disp2_pq_decmp_reg.regValue);
    }
}


void txpre_memory_frc_decide_buffer_size(StructTxPreInfo * txpre_info)
{
    unsigned int buffer_size;
    unsigned int buffer_wid;
    unsigned int buffer_len;

    if (get_platform_model() == PLATFORM_MODEL_5K){
        buffer_wid = TXPRE_CAP_WID_MAX_5K;
        buffer_len = TXPRE_CAP_LEN_MAX_5K;
    } else {
        buffer_wid = TXPRE_CAP_WID_MAX;
        buffer_len = TXPRE_CAP_LEN_MAX;
    }
    
    if(txpre_memory_compression_get_enable() == TRUE){
        buffer_size = txpre_memory_get_capture_line_size_compression(buffer_wid,  txpre_memory_get_compression_bits()) * 128 / 8 * buffer_len + TXPRE_BURST_LENGTH;
    } else {
        buffer_size = buffer_wid * 24 / 8 * buffer_len + TXPRE_BURST_LENGTH;
    }
    rtd_pr_hdmitx_emerg("[txpre-domain] buffer size = %d, %s\n", buffer_size, __FUNCTION__);
    txpre_memory_set_buffer_size(buffer_size);
}

void txpre_memory_frc_free_buffer(StructTxPreInfo * txpre_info)
{
    unsigned char buffer_num = txpre_memory_get_buffer_number();
    unsigned int i;
    for (i = 0; i < buffer_num; i++){
        if (TxpreMdomainBuffer[i].cache != 0){
            dvr_free(TxpreMdomainBuffer[i].cache);
        }
        memset(&(TxpreMdomainBuffer[i]), 0, sizeof(TXPRE_MEMORY_BUFFER_T));
    }
}

unsigned char txpre_memory_frc_alloc_buffer(StructTxPreInfo * txpre_info)
{   
    unsigned long vir_addr, vir_addr_noncache;
    unsigned long phyaddr_addr;
    unsigned char buffer_num = txpre_memory_get_buffer_number();
    unsigned int buffer_size = txpre_memory_get_buffer_size();
    unsigned int i;

    if (TxpreMdomainBuffer[0].cache != 0)
    {
        txpre_memory_frc_free_buffer(txpre_info);
    }

    vir_addr = (unsigned long)dvr_malloc_uncached_specific(buffer_size * buffer_num, GFP_DCU2_FIRST, (void **)&vir_addr_noncache);
    phyaddr_addr = (unsigned long)dvr_to_phys((void*)vir_addr);
    if(vir_addr == 0){
        rtd_pr_hdmitx_emerg("[txpre_memory][%s %d] alloc idma dymanic memory failed from cma\n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    TxpreMdomainBuffer[0].cache = (void *) vir_addr;
    TxpreMdomainBuffer[0].phyaddr = phyaddr_addr;
    TxpreMdomainBuffer[0].getsize = buffer_size * buffer_num;
    TxpreMdomainBuffer[0].size = buffer_size;
    TxpreMdomainBuffer[0].uncache = NULL;
    rtd_pr_hdmitx_emerg("[txpre_memory] TxpreMdomainBuffer[0] = %lx\n", TxpreMdomainBuffer[0].phyaddr);
    for (i = 1; i < buffer_num; i++){
        if(i == 0){
            TxpreMdomainBuffer[0].cache = (void *) vir_addr;
            TxpreMdomainBuffer[0].getsize = buffer_size * buffer_num;
        } else {
            TxpreMdomainBuffer[i].cache = NULL;
            TxpreMdomainBuffer[i].getsize = 0;
        }
        TxpreMdomainBuffer[i].phyaddr = phyaddr_addr + buffer_size * i;
        TxpreMdomainBuffer[i].size = buffer_size;
        TxpreMdomainBuffer[i].uncache = NULL;

        rtd_pr_hdmitx_emerg("[txpre_memory] TxpreMdomainBuffer[%d] = %lx\n", i, TxpreMdomainBuffer[i].phyaddr);
    }

    return TRUE;
}

void txpre_memory_set_capture_ds_path(StructTxPreInfo * txpre_info)
{
    unsigned int CapWidth = txpre_info->Cap_Wid;
	unsigned int CapLength = txpre_info->Cap_Len;

    mdom_hdmitx_cap_cap2_ds_ctrl_0_RBUS mdom_hdmitx_cap_cap2_ds_ctrl_0_reg;
    mdom_hdmitx_cap_cap2_pxl_wrap_ctrl_0_RBUS mdom_hdmitx_cap_cap2_pxl_wrap_ctrl_0_reg;
    mdom_hdmitx_cap_frc_cap2_pack_ctrl_RBUS mdom_hdmitx_cap_frc_cap2_pack_ctrl_reg;

    // ds ctrl
    mdom_hdmitx_cap_cap2_ds_ctrl_0_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_cap2_DS_Ctrl_0_reg);
    mdom_hdmitx_cap_cap2_ds_ctrl_0_reg.ds_hor_ds_mode = 0;
    mdom_hdmitx_cap_cap2_ds_ctrl_0_reg.ds_ver_ds_mode = 0;
    mdom_hdmitx_cap_cap2_ds_ctrl_0_reg.ds_image_h_size = CapWidth;
    mdom_hdmitx_cap_cap2_ds_ctrl_0_reg.ds_ver_en = 0;
    if(txpre_memory_compression_get_enable() == TRUE){
        mdom_hdmitx_cap_cap2_ds_ctrl_0_reg.ds_hor_en = 0;
    }
    else {
        mdom_hdmitx_cap_cap2_ds_ctrl_0_reg.ds_hor_en = (txpre_info->Input_Color_Space == COLOR_YUV422);
    }

    IoReg_Write32(MDOM_HDMITX_CAP_cap2_DS_Ctrl_0_reg, mdom_hdmitx_cap_cap2_ds_ctrl_0_reg.regValue);

    rtd_pr_hdmitx_emerg("[TXPRE] ds_ver_en = %d, ds_hor_en = %d\n", mdom_hdmitx_cap_cap2_ds_ctrl_0_reg.ds_ver_en, mdom_hdmitx_cap_cap2_ds_ctrl_0_reg.ds_hor_en);

    // pxl wrap
    mdom_hdmitx_cap_cap2_pxl_wrap_ctrl_0_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_cap2_Pxl_Wrap_Ctrl_0_reg);
    mdom_hdmitx_cap_cap2_pxl_wrap_ctrl_0_reg.disp2_fs_en = 0;
    mdom_hdmitx_cap_cap2_pxl_wrap_ctrl_0_reg.pack_dummy_format = (txpre_memory_get_capture_mode() == TXPRE_FRAME_MODE);
    mdom_hdmitx_cap_cap2_pxl_wrap_ctrl_0_reg.pack_data_format = txpre_info->Input_Color_Depth;
    mdom_hdmitx_cap_cap2_pxl_wrap_ctrl_0_reg.pack_wrap_mode = (txpre_info->Input_Color_Space == COLOR_YUV422);
    IoReg_Write32(MDOM_HDMITX_CAP_cap2_Pxl_Wrap_Ctrl_0_reg, mdom_hdmitx_cap_cap2_pxl_wrap_ctrl_0_reg.regValue);

    rtd_pr_hdmitx_emerg("[TXPRE] pack_wrap_mode = %d, pack_dummy_format = %d, pack_data_format = %d\n", 
                mdom_hdmitx_cap_cap2_pxl_wrap_ctrl_0_reg.pack_wrap_mode,  mdom_hdmitx_cap_cap2_pxl_wrap_ctrl_0_reg.pack_dummy_format, mdom_hdmitx_cap_cap2_pxl_wrap_ctrl_0_reg.pack_dummy_format);

    // pack ctrl
    mdom_hdmitx_cap_frc_cap2_pack_ctrl_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_frc_cap2_pack_ctrl_reg);
    mdom_hdmitx_cap_frc_cap2_pack_ctrl_reg.cap2_frame_acc_mode = (txpre_memory_get_capture_mode() == TXPRE_FRAME_MODE);
    mdom_hdmitx_cap_frc_cap2_pack_ctrl_reg.cap2_vact = CapLength;
    mdom_hdmitx_cap_frc_cap2_pack_ctrl_reg.cap2_hact = CapWidth;
    IoReg_Write32(MDOM_HDMITX_CAP_frc_cap2_pack_ctrl_reg, mdom_hdmitx_cap_frc_cap2_pack_ctrl_reg.regValue);

}

void txpre_memory_set_wdma(StructTxPreInfo * txpre_info)
{
    mdom_hdmitx_cap_frc_cap2_num_bl_wrap_line_step_RBUS mdom_hdmitx_cap_frc_cap2_num_bl_wrap_line_step_reg;
    mdom_hdmitx_cap_frc_cap2_num_bl_wrap_ctl_RBUS mdom_hdmitx_cap_frc_cap2_num_bl_wrap_ctl_reg;
    mdom_hdmitx_cap_frc_cap2_num_bl_wrap_word_RBUS mdom_hdmitx_cap_frc_cap2_num_bl_wrap_word_reg;
    mdom_hdmitx_cap_frc_cap2_wr_ctrl_RBUS mdom_hdmitx_cap_frc_cap2_wr_ctrl_reg;

	unsigned int TotalSize;
	unsigned int LineStep;
	unsigned int CapWidth = txpre_info->Cap_Wid;
	unsigned int CapLength = txpre_info->Cap_Len;


    if(txpre_memory_compression_get_enable() == TRUE)
	{
        TotalSize = txpre_memory_get_capture_line_size_compression(CapWidth,  txpre_memory_get_compression_bits());
	} else
    {
        TotalSize = drvif_memory_get_data_align(txpre_memory_get_pixel_size(txpre_info) * CapWidth, 128) / 128;
    }

    if (txpre_memory_get_capture_mode() == TXPRE_FRAME_MODE)
    {
        TotalSize = TotalSize * CapLength;
    }

	// cap0_line_step
	if (txpre_memory_get_capture_mode() == TXPRE_LINE_MODE)
	{
		LineStep = TotalSize;
	} else {
		LineStep = 0;
	}

    mdom_hdmitx_cap_frc_cap2_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_FRC_CAP2_num_bl_wrap_line_step_reg);
    mdom_hdmitx_cap_frc_cap2_num_bl_wrap_line_step_reg.cap2_line_step = LineStep;
    IoReg_Write32(MDOM_HDMITX_CAP_FRC_CAP2_num_bl_wrap_line_step_reg, mdom_hdmitx_cap_frc_cap2_num_bl_wrap_line_step_reg.regValue);
    
    rtd_pr_hdmitx_emerg("[TXPRE] WDMA TotalSize = %d, LineStep = %d\n", TotalSize,  LineStep);
	
    // cap0_burst_len & cap0_line_num
    mdom_hdmitx_cap_frc_cap2_num_bl_wrap_ctl_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_FRC_CAP2_num_bl_wrap_ctl_reg);
    mdom_hdmitx_cap_frc_cap2_num_bl_wrap_ctl_reg.cap2_line_num = CapLength;
    mdom_hdmitx_cap_frc_cap2_num_bl_wrap_ctl_reg.cap2_burst_len = TXPRE_BURST_LENGTH;
    IoReg_Write32(MDOM_HDMITX_CAP_FRC_CAP2_num_bl_wrap_ctl_reg, mdom_hdmitx_cap_frc_cap2_num_bl_wrap_ctl_reg.regValue);

	// line_burst_num
    mdom_hdmitx_cap_frc_cap2_num_bl_wrap_word_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_FRC_CAP2_num_bl_wrap_word_reg);
    mdom_hdmitx_cap_frc_cap2_num_bl_wrap_word_reg.cap2_addr_toggle_mode = 0;
    mdom_hdmitx_cap_frc_cap2_num_bl_wrap_word_reg.cap2_line_burst_num = TotalSize;
    IoReg_Write32(MDOM_HDMITX_CAP_FRC_CAP2_num_bl_wrap_word_reg, mdom_hdmitx_cap_frc_cap2_num_bl_wrap_word_reg.regValue);

    // byte_swap
    mdom_hdmitx_cap_frc_cap2_wr_ctrl_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_FRC_CAP2_WR_Ctrl_reg);
    mdom_hdmitx_cap_frc_cap2_wr_ctrl_reg.cap2_dma_8byte_swap = 0;
    mdom_hdmitx_cap_frc_cap2_wr_ctrl_reg.cap2_dma_4byte_swap = 0;
    mdom_hdmitx_cap_frc_cap2_wr_ctrl_reg.cap2_dma_2byte_swap = 0;
    mdom_hdmitx_cap_frc_cap2_wr_ctrl_reg.cap2_dma_1byte_swap = 0;
    IoReg_Write32(MDOM_HDMITX_CAP_FRC_CAP2_WR_Ctrl_reg, mdom_hdmitx_cap_frc_cap2_wr_ctrl_reg.regValue);
    
    // outstanding
    //mcap_outstanding_ctrl( _ENABLE);

}

void txpre_memory_frc_set_capture_block_control(StructTxPreInfo * txpre_info)
{
    mdom_hdmitx_cap_cap2_buf_control_RBUS mdom_hdmitx_cap_cap2_buf_control_reg;
    mdom_hdmitx_cap_cap2_buf_control_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_Cap2_buf_control_reg);
    mdom_hdmitx_cap_cap2_buf_control_reg.cap2_buf_num = (txpre_memory_get_buffer_number() == 0) ? 0 : (txpre_memory_get_buffer_number() - 1);
    mdom_hdmitx_cap_cap2_buf_control_reg.cap2_buf_distance = 0;
    mdom_hdmitx_cap_cap2_buf_control_reg.cap2_buf_force_en = 0;
    mdom_hdmitx_cap_cap2_buf_control_reg.cap2_buf_force_addr = 0;
    mdom_hdmitx_cap_cap2_buf_control_reg.cap2_wait_disp_chg_en = 1;
    IoReg_Write32(MDOM_HDMITX_CAP_Cap2_buf_control_reg, mdom_hdmitx_cap_cap2_buf_control_reg.regValue);
}

void txpre_memory_update_capture_buffer(void)
{
    mdom_hdmitx_cap_cap2_boundaryaddr_ctrl0_RBUS mdom_hdmitx_cap_cap2_boundaryaddr_ctrl0_reg;
    unsigned int buffer_size = txpre_memory_get_buffer_size();
    unsigned char cap_num = txpre_memory_get_buffer_number();

    if(cap_num > 0){

        IoReg_Write32(MDOM_HDMITX_CAP_cap2_first_buf_addr_reg, txpre_memory_get_buffer_start_address(0) & 0xfffffff0);

        IoReg_Write32(MDOM_HDMITX_CAP_Cap2_downlimit_0_reg, txpre_memory_get_buffer_start_address(0) & 0xfffffff0);
        IoReg_Write32(MDOM_HDMITX_CAP_Cap2_uplimit_0_reg, (txpre_memory_get_buffer_start_address(0) + buffer_size - TXPRE_BURST_LENGTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 1){

        IoReg_Write32(MDOM_HDMITX_CAP_cap2_second_buf_addr_reg, txpre_memory_get_buffer_start_address(1) & 0xfffffff0);

        IoReg_Write32(MDOM_HDMITX_CAP_Cap2_downlimit_1_reg, txpre_memory_get_buffer_start_address(1) & 0xfffffff0);
        IoReg_Write32(MDOM_HDMITX_CAP_Cap2_uplimit_1_reg, (txpre_memory_get_buffer_start_address(1) + buffer_size - TXPRE_BURST_LENGTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 2){

        IoReg_Write32(MDOM_HDMITX_CAP_cap2_third_buf_addr_reg, txpre_memory_get_buffer_start_address(2) & 0xfffffff0);

        IoReg_Write32(MDOM_HDMITX_CAP_Cap2_downlimit_2_reg, txpre_memory_get_buffer_start_address(2) & 0xfffffff0);
        IoReg_Write32(MDOM_HDMITX_CAP_Cap2_uplimit_2_reg, (txpre_memory_get_buffer_start_address(2) + buffer_size - TXPRE_BURST_LENGTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 3){

        IoReg_Write32(MDOM_HDMITX_CAP_cap2_forth_buf_addr_reg, txpre_memory_get_buffer_start_address(3) & 0xfffffff0);

        IoReg_Write32(MDOM_HDMITX_CAP_Cap2_downlimit_3_reg, txpre_memory_get_buffer_start_address(3) & 0xfffffff0);
        IoReg_Write32(MDOM_HDMITX_CAP_Cap2_uplimit_3_reg, (txpre_memory_get_buffer_start_address(3) + buffer_size - TXPRE_BURST_LENGTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 4){

        IoReg_Write32(MDOM_HDMITX_CAP_cap2_fifth_buf_addr_reg, txpre_memory_get_buffer_start_address(4) & 0xfffffff0);

        IoReg_Write32(MDOM_HDMITX_CAP_Cap2_downlimit_4_reg, txpre_memory_get_buffer_start_address(4) & 0xfffffff0);
        IoReg_Write32(MDOM_HDMITX_CAP_Cap2_uplimit_4_reg, (txpre_memory_get_buffer_start_address(4) + buffer_size - TXPRE_BURST_LENGTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 5){

        IoReg_Write32(MDOM_HDMITX_CAP_cap2_sixth_buf_addr_reg, txpre_memory_get_buffer_start_address(5) & 0xfffffff0);

        IoReg_Write32(MDOM_HDMITX_CAP_Cap2_downlimit_5_reg, txpre_memory_get_buffer_start_address(5) & 0xfffffff0);
        IoReg_Write32(MDOM_HDMITX_CAP_Cap2_uplimit_5_reg, (txpre_memory_get_buffer_start_address(5) + buffer_size - TXPRE_BURST_LENGTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 6){

        IoReg_Write32(MDOM_HDMITX_CAP_cap2_seventh_buf_addr_reg, txpre_memory_get_buffer_start_address(6) & 0xfffffff0);

        IoReg_Write32(MDOM_HDMITX_CAP_Cap2_downlimit_6_reg, txpre_memory_get_buffer_start_address(6) & 0xfffffff0);
        IoReg_Write32(MDOM_HDMITX_CAP_Cap2_uplimit_6_reg, (txpre_memory_get_buffer_start_address(6) + buffer_size - TXPRE_BURST_LENGTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 7){

        IoReg_Write32(MDOM_HDMITX_CAP_cap2_eightth_buf_addr_reg, txpre_memory_get_buffer_start_address(7) & 0xfffffff0);

        IoReg_Write32(MDOM_HDMITX_CAP_Cap2_downlimit_7_reg, txpre_memory_get_buffer_start_address(7) & 0xfffffff0);
        IoReg_Write32(MDOM_HDMITX_CAP_Cap2_uplimit_7_reg, (txpre_memory_get_buffer_start_address(7) + buffer_size - TXPRE_BURST_LENGTH) & 0xfffffff0);//need cut burstLen

    }

    mdom_hdmitx_cap_cap2_boundaryaddr_ctrl0_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_Cap2_BoundaryAddr_CTRL0_reg);
    mdom_hdmitx_cap_cap2_boundaryaddr_ctrl0_reg.cap2_blk7_sel = 7;
    mdom_hdmitx_cap_cap2_boundaryaddr_ctrl0_reg.cap2_blk6_sel = 6;
    mdom_hdmitx_cap_cap2_boundaryaddr_ctrl0_reg.cap2_blk5_sel = 5;
    mdom_hdmitx_cap_cap2_boundaryaddr_ctrl0_reg.cap2_blk4_sel = 4;
    mdom_hdmitx_cap_cap2_boundaryaddr_ctrl0_reg.cap2_blk3_sel = 3;
    mdom_hdmitx_cap_cap2_boundaryaddr_ctrl0_reg.cap2_blk2_sel = 2;
    mdom_hdmitx_cap_cap2_boundaryaddr_ctrl0_reg.cap2_blk1_sel = 1;
    mdom_hdmitx_cap_cap2_boundaryaddr_ctrl0_reg.cap2_blk0_sel = 0;
    IoReg_Write32(MDOM_HDMITX_CAP_Cap2_BoundaryAddr_CTRL0_reg, mdom_hdmitx_cap_cap2_boundaryaddr_ctrl0_reg.regValue);
}

void txpre_memory_set_display_us_path(StructTxPreInfo * txpre_info)
{
    unsigned int DispWidth = txpre_info->Cap_Wid;
	unsigned int DispLength = txpre_info->Cap_Len;

    mdom_hdmitx_disp_frc_us0_ctrl_RBUS mdom_hdmitx_disp_frc_us0_ctrl_reg;
    mdom_hdmitx_disp_frc_unpack0_ctrl_RBUS mdom_hdmitx_disp_frc_unpack0_ctrl_reg;
    mdom_hdmitx_disp_frc_disp2_size_ctrl0_RBUS mdom_hdmitx_disp_frc_disp2_size_ctrl0_reg;

        // us ctrl
    mdom_hdmitx_disp_frc_us0_ctrl_reg.regValue = IoReg_Read32(MDOM_HDMITX_DISP_FRC_US0_Ctrl_reg);
    mdom_hdmitx_disp_frc_us0_ctrl_reg.disp2_hor_us_mode = 0;
    mdom_hdmitx_disp_frc_us0_ctrl_reg.disp2_ver_us_mode = 0;
    mdom_hdmitx_disp_frc_us0_ctrl_reg.disp2_ver_en = 0;
    if(txpre_memory_compression_get_enable() == TRUE){
        mdom_hdmitx_disp_frc_us0_ctrl_reg.disp2_hor_en = 0;
    } else {
        mdom_hdmitx_disp_frc_us0_ctrl_reg.disp2_hor_en = (txpre_info->Input_Color_Space == COLOR_YUV422);
    }
    IoReg_Write32(MDOM_HDMITX_DISP_FRC_US0_Ctrl_reg, mdom_hdmitx_disp_frc_us0_ctrl_reg.regValue);
    
    rtd_pr_hdmitx_emerg("[TXPRE] US disp2_ver_en = %d, disp2_hor_en = %d\n", mdom_hdmitx_disp_frc_us0_ctrl_reg.disp2_ver_en, mdom_hdmitx_disp_frc_us0_ctrl_reg.disp2_hor_en);
        
    // unpack_ctrl
    mdom_hdmitx_disp_frc_unpack0_ctrl_reg.regValue = IoReg_Read32(MDOM_HDMITX_DISP_FRC_unpack0_ctrl_reg);
    mdom_hdmitx_disp_frc_unpack0_ctrl_reg.disp2_dummy_format = (txpre_memory_get_capture_mode() == TXPRE_FRAME_MODE);
    mdom_hdmitx_disp_frc_unpack0_ctrl_reg.disp2_data_format = txpre_info->Input_Color_Depth;
    mdom_hdmitx_disp_frc_unpack0_ctrl_reg.disp2_wrap_mode = (txpre_info->Input_Color_Space == COLOR_YUV422);
    IoReg_Write32(MDOM_HDMITX_DISP_FRC_unpack0_ctrl_reg, mdom_hdmitx_disp_frc_unpack0_ctrl_reg.regValue);
    
    rtd_pr_hdmitx_emerg("[TXPRE] US disp2_wrap_mode = %d, disp2_dummy_format = %d, disp2_data_format = %d\n",
                mdom_hdmitx_disp_frc_unpack0_ctrl_reg.disp2_wrap_mode, mdom_hdmitx_disp_frc_unpack0_ctrl_reg.disp2_dummy_format, mdom_hdmitx_disp_frc_unpack0_ctrl_reg.disp2_data_format);

    // size ctrl
    mdom_hdmitx_disp_frc_disp2_size_ctrl0_reg.regValue = IoReg_Read32(MDOM_HDMITX_DISP_FRC_disp2_size_ctrl0_reg);
    mdom_hdmitx_disp_frc_disp2_size_ctrl0_reg.disp2_vact = DispLength;
    mdom_hdmitx_disp_frc_disp2_size_ctrl0_reg.disp2_hact = DispWidth;
    IoReg_Write32(MDOM_HDMITX_DISP_FRC_disp2_size_ctrl0_reg, mdom_hdmitx_disp_frc_disp2_size_ctrl0_reg.regValue);

}

void txpre_memory_set_rdma(StructTxPreInfo * txpre_info)
{
    mdom_hdmitx_disp_frc_disp2_num_bl_wrap_line_step_RBUS mdom_hdmitx_disp_frc_disp2_num_bl_wrap_line_step_reg;
    mdom_hdmitx_disp_frc_disp2_num_bl_wrap_ctl_RBUS mdom_hdmitx_disp_frc_disp2_num_bl_wrap_ctl_reg;
    mdom_hdmitx_disp_frc_disp2_num_bl_wrap_word_RBUS mdom_hdmitx_disp_frc_disp2_num_bl_wrap_word_reg;
    mdom_hdmitx_disp_frc_disp2_rd_ctrl_RBUS mdom_hdmitx_disp_frc_disp2_rd_ctrl_reg;
    
    unsigned int TotalSize;
	unsigned int LineStep;
	unsigned int DispWidth = txpre_info->Cap_Wid;
	unsigned int DispLength = txpre_info->Cap_Len;

    if(txpre_memory_compression_get_enable() == TRUE)
	{
        TotalSize = txpre_memory_get_capture_line_size_compression(DispWidth,  txpre_memory_get_compression_bits());
	} else
    {
        TotalSize = drvif_memory_get_data_align(txpre_memory_get_pixel_size(txpre_info) * DispWidth, 128) / 128;
    }

    if (txpre_memory_get_capture_mode() == TXPRE_FRAME_MODE)
    {
        TotalSize = TotalSize * DispLength;
    }

    // line_step
	if (txpre_memory_get_capture_mode() == TXPRE_LINE_MODE)
	{
        LineStep = TotalSize;
	} else {
		LineStep = 0;
	}
    mdom_hdmitx_disp_frc_disp2_num_bl_wrap_line_step_reg.regValue = IoReg_Read32(MDOM_HDMITX_DISP_FRC_DISP2_num_bl_wrap_line_step_reg);
    mdom_hdmitx_disp_frc_disp2_num_bl_wrap_line_step_reg.disp2_line_step = LineStep;
    IoReg_Write32(MDOM_HDMITX_DISP_FRC_DISP2_num_bl_wrap_line_step_reg, mdom_hdmitx_disp_frc_disp2_num_bl_wrap_line_step_reg.regValue);
 
    rtd_pr_hdmitx_emerg("[TXPRE] RDMA TotalSize = %d, LineStep = %d\n", TotalSize,  LineStep);

    // line_num & burst_len
    mdom_hdmitx_disp_frc_disp2_num_bl_wrap_ctl_reg.regValue = IoReg_Read32(MDOM_HDMITX_DISP_FRC_DISP2_num_bl_wrap_ctl_reg);
    mdom_hdmitx_disp_frc_disp2_num_bl_wrap_ctl_reg.disp2_line_num = DispLength;
    mdom_hdmitx_disp_frc_disp2_num_bl_wrap_ctl_reg.disp2_burst_len = TXPRE_BURST_LENGTH;
    IoReg_Write32(MDOM_HDMITX_DISP_FRC_DISP2_num_bl_wrap_ctl_reg, mdom_hdmitx_disp_frc_disp2_num_bl_wrap_ctl_reg.regValue);

    // line_burst_num
    mdom_hdmitx_disp_frc_disp2_num_bl_wrap_word_reg.regValue = IoReg_Read32(MDOM_HDMITX_DISP_FRC_DISP2_num_bl_wrap_word_reg);
    mdom_hdmitx_disp_frc_disp2_num_bl_wrap_word_reg.disp2_addr_toggle_mode = 0;
    mdom_hdmitx_disp_frc_disp2_num_bl_wrap_word_reg.disp2_line_burst_num = TotalSize;
    IoReg_Write32(MDOM_HDMITX_DISP_FRC_DISP2_num_bl_wrap_word_reg, mdom_hdmitx_disp_frc_disp2_num_bl_wrap_word_reg.regValue);

    // byte_swap
    mdom_hdmitx_disp_frc_disp2_rd_ctrl_reg.regValue = IoReg_Read32(MDOM_HDMITX_DISP_FRC_DISP2_RD_Ctrl_reg);
    mdom_hdmitx_disp_frc_disp2_rd_ctrl_reg.disp2_dma_8byte_swap = 0;
    mdom_hdmitx_disp_frc_disp2_rd_ctrl_reg.disp2_dma_4byte_swap = 0;
    mdom_hdmitx_disp_frc_disp2_rd_ctrl_reg.disp2_dma_2byte_swap = 0;
    mdom_hdmitx_disp_frc_disp2_rd_ctrl_reg.disp2_dma_1byte_swap = 0;
    IoReg_Write32(MDOM_HDMITX_DISP_FRC_DISP2_RD_Ctrl_reg, mdom_hdmitx_disp_frc_disp2_rd_ctrl_reg.regValue);

    // outstanding
    //mdisp_outstanding_ctrl( _ENABLE);
}

void txpre_memory_frc_set_display_block_control(StructTxPreInfo * txpre_info)
{
    mdom_hdmitx_disp_frc_disp2_buffer_select_control_RBUS mdom_hdmitx_disp_frc_disp2_buffer_select_control_reg;
    mdom_hdmitx_disp_frc_disp2_buffer_select_control_reg.regValue = IoReg_Read32(MDOM_HDMITX_DISP_FRC_disp2_buffer_select_control_reg);
    mdom_hdmitx_disp_frc_disp2_buffer_select_control_reg.disp2_buf_num = (txpre_memory_get_buffer_number() == 0) ? 0 : (txpre_memory_get_buffer_number() - 1);
    mdom_hdmitx_disp_frc_disp2_buffer_select_control_reg.disp2_buf_distance = 0;
    mdom_hdmitx_disp_frc_disp2_buffer_select_control_reg.disp2_buf_force_en = 0;
    mdom_hdmitx_disp_frc_disp2_buffer_select_control_reg.disp2_buf_force_addr = 0;
    mdom_hdmitx_disp_frc_disp2_buffer_select_control_reg.disp2_wait_cap_chg_en = 1;
    IoReg_Write32(MDOM_HDMITX_DISP_FRC_disp2_buffer_select_control_reg, mdom_hdmitx_disp_frc_disp2_buffer_select_control_reg.regValue);
}

void txpre_memory_update_display_buffer(void)
{
    mdom_hdmitx_disp_disp2_boundaryaddr_ctrl0_RBUS mdom_hdmitx_disp_disp2_boundaryaddr_ctrl0_reg;
    unsigned int buffer_size = txpre_memory_get_buffer_size();
    unsigned char cap_num = txpre_memory_get_buffer_number();

    if(cap_num > 0){

        IoReg_Write32(MDOM_HDMITX_DISP_disp2_first_buf_addr_reg, txpre_memory_get_buffer_start_address(0) & 0xfffffff0);

        IoReg_Write32(MDOM_HDMITX_DISP_Disp2_downlimit_0_reg, txpre_memory_get_buffer_start_address(0) & 0xfffffff0);
        IoReg_Write32(MDOM_HDMITX_DISP_Disp2_uplimit_0_reg, (txpre_memory_get_buffer_start_address(0) + buffer_size - TXPRE_BURST_LENGTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 1){

        IoReg_Write32(MDOM_HDMITX_DISP_disp2_second_buf_addr_reg, txpre_memory_get_buffer_start_address(1) & 0xfffffff0);

        IoReg_Write32(MDOM_HDMITX_DISP_Disp2_downlimit_1_reg, txpre_memory_get_buffer_start_address(1) & 0xfffffff0);
        IoReg_Write32(MDOM_HDMITX_DISP_Disp2_uplimit_1_reg, (txpre_memory_get_buffer_start_address(1) + buffer_size - TXPRE_BURST_LENGTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 2){

        IoReg_Write32(MDOM_HDMITX_DISP_disp2_third_buf_addr_reg, txpre_memory_get_buffer_start_address(2) & 0xfffffff0);

        IoReg_Write32(MDOM_HDMITX_DISP_Disp2_downlimit_2_reg, txpre_memory_get_buffer_start_address(2) & 0xfffffff0);
        IoReg_Write32(MDOM_HDMITX_DISP_Disp2_uplimit_2_reg, (txpre_memory_get_buffer_start_address(2) + buffer_size - TXPRE_BURST_LENGTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 3){

        IoReg_Write32(MDOM_HDMITX_DISP_disp2_forth_buf_addr_reg, txpre_memory_get_buffer_start_address(3) & 0xfffffff0);

        IoReg_Write32(MDOM_HDMITX_DISP_Disp2_downlimit_3_reg, txpre_memory_get_buffer_start_address(3) & 0xfffffff0);
        IoReg_Write32(MDOM_HDMITX_DISP_Disp2_uplimit_3_reg, (txpre_memory_get_buffer_start_address(3) + buffer_size - TXPRE_BURST_LENGTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 4){

        IoReg_Write32(MDOM_HDMITX_DISP_disp2_fifth_buf_addr_reg, txpre_memory_get_buffer_start_address(4) & 0xfffffff0);

        IoReg_Write32(MDOM_HDMITX_DISP_Disp2_downlimit_4_reg, txpre_memory_get_buffer_start_address(4) & 0xfffffff0);
        IoReg_Write32(MDOM_HDMITX_DISP_Disp2_uplimit_4_reg, (txpre_memory_get_buffer_start_address(4) + buffer_size - TXPRE_BURST_LENGTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 5){

        IoReg_Write32(MDOM_HDMITX_DISP_disp2_sixth_buf_addr_reg, txpre_memory_get_buffer_start_address(5) & 0xfffffff0);

        IoReg_Write32(MDOM_HDMITX_DISP_Disp2_downlimit_5_reg, txpre_memory_get_buffer_start_address(5) & 0xfffffff0);
        IoReg_Write32(MDOM_HDMITX_DISP_Disp2_uplimit_5_reg, (txpre_memory_get_buffer_start_address(5) + buffer_size - TXPRE_BURST_LENGTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 6){

        IoReg_Write32(MDOM_HDMITX_DISP_disp2_seventh_buf_addr_reg, txpre_memory_get_buffer_start_address(6) & 0xfffffff0);

        IoReg_Write32(MDOM_HDMITX_DISP_Disp2_downlimit_6_reg, txpre_memory_get_buffer_start_address(6) & 0xfffffff0);
        IoReg_Write32(MDOM_HDMITX_DISP_Disp2_uplimit_6_reg, (txpre_memory_get_buffer_start_address(6) + buffer_size - TXPRE_BURST_LENGTH) & 0xfffffff0);//need cut burstLen

    }

    if(cap_num > 7){

        IoReg_Write32(MDOM_HDMITX_DISP_disp2_eightth_buf_addr_reg, txpre_memory_get_buffer_start_address(7) & 0xfffffff0);

        IoReg_Write32(MDOM_HDMITX_DISP_Disp2_downlimit_7_reg, txpre_memory_get_buffer_start_address(7) & 0xfffffff0);
        IoReg_Write32(MDOM_HDMITX_DISP_Disp2_uplimit_7_reg, (txpre_memory_get_buffer_start_address(7) + buffer_size - TXPRE_BURST_LENGTH) & 0xfffffff0);//need cut burstLen

    }

    mdom_hdmitx_disp_disp2_boundaryaddr_ctrl0_reg.regValue = IoReg_Read32(MDOM_HDMITX_DISP_Disp2_BoundaryAddr_CTRL0_reg);
    mdom_hdmitx_disp_disp2_boundaryaddr_ctrl0_reg.disp2_blk7_sel = 7;
    mdom_hdmitx_disp_disp2_boundaryaddr_ctrl0_reg.disp2_blk6_sel = 6;
    mdom_hdmitx_disp_disp2_boundaryaddr_ctrl0_reg.disp2_blk5_sel = 5;
    mdom_hdmitx_disp_disp2_boundaryaddr_ctrl0_reg.disp2_blk4_sel = 4;
    mdom_hdmitx_disp_disp2_boundaryaddr_ctrl0_reg.disp2_blk3_sel = 3;
    mdom_hdmitx_disp_disp2_boundaryaddr_ctrl0_reg.disp2_blk2_sel = 2;
    mdom_hdmitx_disp_disp2_boundaryaddr_ctrl0_reg.disp2_blk1_sel = 1;
    mdom_hdmitx_disp_disp2_boundaryaddr_ctrl0_reg.disp2_blk0_sel = 0;
    IoReg_Write32(MDOM_HDMITX_DISP_Disp2_BoundaryAddr_CTRL0_reg, mdom_hdmitx_disp_disp2_boundaryaddr_ctrl0_reg.regValue);


}

void txpre_memory_set_capture_enable(unsigned char enable)
{
    mdom_hdmitx_cap_frc_cap2_wr_ctrl_RBUS mdom_hdmitx_cap_frc_cap2_wr_ctrl_reg;

    rtd_pr_hdmitx_emerg("[txpre-domain] txpre_memory_set_capture_enable = %d\n", enable);
    mdom_hdmitx_cap_frc_cap2_wr_ctrl_reg.regValue = IoReg_Read32(MDOM_HDMITX_CAP_FRC_CAP2_WR_Ctrl_reg);
    mdom_hdmitx_cap_frc_cap2_wr_ctrl_reg.cap2_dma_enable = enable;
    IoReg_Write32(MDOM_HDMITX_CAP_FRC_CAP2_WR_Ctrl_reg, mdom_hdmitx_cap_frc_cap2_wr_ctrl_reg.regValue);
}

void txpre_memory_set_display_enable(unsigned char enable)
{
    mdom_hdmitx_disp_frc_disp2_rd_ctrl_RBUS mdom_hdmitx_disp_frc_disp2_rd_ctrl_reg;

    rtd_pr_hdmitx_emerg("[txpre-mdomain] txpre_memory_set_display_enable = %d\n", enable);
    mdom_hdmitx_disp_frc_disp2_rd_ctrl_reg.regValue = IoReg_Read32(MDOM_HDMITX_DISP_FRC_DISP2_RD_Ctrl_reg);
    mdom_hdmitx_disp_frc_disp2_rd_ctrl_reg.disp2_dma_enable = enable;
    IoReg_Write32(MDOM_HDMITX_DISP_FRC_DISP2_RD_Ctrl_reg, mdom_hdmitx_disp_frc_disp2_rd_ctrl_reg.regValue);
}

void txpre_memory_frc_set_capture(StructTxPreInfo * txpre_info)
{
    txpre_memory_frc_decide_buffer_size(txpre_info);
    if (txpre_memory_frc_alloc_buffer(txpre_info) == FALSE)
    {
        rtd_pr_hdmitx_err("[txpre-mdomain] txpre_memory_frc_alloc_buffer fail! stop setting capture!");
        return ;
    }
    txpre_memory_update_capture_buffer();

    txpre_memory_set_capture_ds_path(txpre_info);
    txpre_memory_set_wdma(txpre_info);
    txpre_memory_frc_set_capture_block_control(txpre_info);
}

void txpre_memory_frc_set_display(StructTxPreInfo * txpre_info)
{
    txpre_memory_update_display_buffer();

    txpre_memory_set_display_us_path(txpre_info);
    txpre_memory_set_rdma(txpre_info);
    txpre_memory_frc_set_display_block_control(txpre_info);
}

void txpre_memory_fs_set_capture(StructTxPreInfo * txpre_info)
{
    txpre_memory_set_capture_ds_path(txpre_info);
    txpre_memory_set_wdma(txpre_info);
}

void txpre_memory_fs_set_display(StructTxPreInfo * txpre_info)
{
    txpre_memory_set_display_us_path(txpre_info);
    txpre_memory_set_rdma(txpre_info);
}

void txpre_memory_frc_handler(StructTxPreInfo * txpre_info)
{
    txpre_memory_set_capture_enable(FALSE);
    txpre_memory_set_display_enable(FALSE);

    if (txpre_info->isDsc){
        txpre_memory_compression_set_enable(FALSE);
    } else {
        txpre_memory_compression_set_enable(TRUE);
    }
    txpre_memory_set_capture_mode(TXPRE_LINE_MODE);
    txpre_memory_set_compression_bits(16);
    txpre_memory_set_buffer_number(3);

    txpre_memory_frc_set_capture(txpre_info);
    txpre_memory_frc_set_display(txpre_info);

    txpre_memory_comp_setting(txpre_info, txpre_memory_compression_get_enable(), txpre_info->Cap_Wid, txpre_info->Cap_Len, txpre_memory_get_compression_bits());

    txpre_memory_set_capture_enable(TRUE);
    txpre_memory_set_display_enable(TRUE);
}

void txpre_memory_fs_handler(StructTxPreInfo * txpre_info)
{
    txpre_memory_set_capture_enable(FALSE);
    txpre_memory_set_display_enable(FALSE);

    if (txpre_info->isDsc){
        txpre_memory_compression_set_enable(FALSE);
    } else {
        txpre_memory_compression_set_enable(TRUE);
    }
    
    txpre_memory_set_capture_mode(TXPRE_LINE_MODE);
    txpre_memory_set_compression_bits(16);
    txpre_memory_set_buffer_number(3);
    txpre_memory_frc_set_capture(txpre_info);
    txpre_memory_frc_set_display(txpre_info);
    txpre_memory_comp_setting(txpre_info, txpre_memory_compression_get_enable(), txpre_info->Cap_Wid, txpre_info->Cap_Len, txpre_memory_get_compression_bits());
    IoReg_SetBits(MDOM_HDMITX_CAP_cap2_Pxl_Wrap_Ctrl_0_reg, MDOM_HDMITX_CAP_cap2_Pxl_Wrap_Ctrl_0_disp2_fs_en_mask);
}
