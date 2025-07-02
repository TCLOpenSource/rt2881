#include <rbus/dma_vgip_reg.h>
#include <rbus/i4_vgip_reg.h>
#include <rbus/i5_vgip_reg.h>
#include <rbus/i6_vgip_reg.h>
#include <rbus/h3ddma_reg.h>
#include <rbus/h4ddma_reg.h>
#include <rbus/h5ddma_reg.h>
#include <rbus/h6ddma_reg.h>
#include <rbus/h3ddma_hsd_reg.h>
#include <rbus/h4ddma_hsd_reg.h>
#include <rbus/h5ddma_hsd_reg.h>
#include <rbus/h6ddma_hsd_reg.h>
#include <rbus/h3ddma_1_rgb2yuv_dither_4xxto4xx_reg.h>
#include <rbus/h4ddma_rgb2yuv_dither_4xxto4xx_reg.h>
#include <rbus/h5ddma_rgb2yuv_dither_4xxto4xx_reg.h>
#include <rbus/h6ddma_rgb2yuv_dither_4xxto4xx_reg.h>

#include <scaler/scalerCommon.h>




//dma-vgip i3,i4,i5,i6 register array
//++++++++++++++++++++++++++++++++++start
const unsigned int vgip_ctrl_reg_addr[MAX_IDDMA_DEV] = 					{DMA_VGIP_DMA_VGIP_CTRL_reg,			I4_VGIP_I4_VGIP_CTRL_reg,				I5_VGIP_I5_VGIP_CTRL_reg,				I6_VGIP_I6_VGIP_CTRL_reg};
const unsigned int vgip_hsta_width_reg_addr[MAX_IDDMA_DEV] = 			{DMA_VGIP_DMA_VGIP_ACT_HSTA_Width_reg, 	I4_VGIP_I4_VGIP_ACT_HSTA_Width_reg, 	I5_VGIP_I5_VGIP_ACT_HSTA_Width_reg, 	I6_VGIP_I6_VGIP_ACT_HSTA_Width_reg};
const unsigned int vgip_vsta_length_reg_addr[MAX_IDDMA_DEV] = 			{DMA_VGIP_DMA_VGIP_ACT_VSTA_Length_reg, I4_VGIP_I4_VGIP_ACT_VSTA_Length_reg,	I5_VGIP_I5_VGIP_ACT_VSTA_Length_reg, 	I6_VGIP_I6_VGIP_ACT_VSTA_Length_reg};
const unsigned int vgip_misc_reg_addr[MAX_IDDMA_DEV] = 					{DMA_VGIP_DMA_VGIP_MISC_reg, 			I4_VGIP_I4_VGIP_MISC_reg, 				I5_VGIP_I5_VGIP_MISC_reg, 				I6_VGIP_I6_VGIP_MISC_reg};
const unsigned int vgip_htotal_reg_addr[MAX_IDDMA_DEV] = 				{DMA_VGIP_DMA_VGIP_HTOTAL_reg, 			I4_VGIP_I4_VGIP_HTOTAL_reg, 			I5_VGIP_I5_VGIP_HTOTAL_reg, 			I6_VGIP_I6_VGIP_HTOTAL_reg};
const unsigned int vgip_delay_reg_addr[MAX_IDDMA_DEV] = 				{DMA_VGIP_DMA_VGIP_DELAY_reg, 			I4_VGIP_I4_VGIP_DELAY_reg, 				I5_VGIP_I5_VGIP_DELAY_reg, 				I6_VGIP_I6_VGIP_DELAY_reg};
const unsigned int vgip_cts_fifo_ctl_reg_addr[MAX_IDDMA_DEV] =			{DMA_VGIP_DMA_VGIP_CTS_FIFO_CTL_reg,	I4_VGIP_I4_VGIP_CTS_FIFO_CTL_reg,		I5_VGIP_I5_VGIP_CTS_FIFO_CTL_reg, 		I6_VGIP_I6_VGIP_CTS_FIFO_CTL_reg};
const unsigned int vgip_regen_vs_ctrl1_reg_addr[MAX_IDDMA_DEV] = 		{DMA_VGIP_DMA_VGIP_REGEN_VS_CTRL1_reg,	I4_VGIP_I4_VGIP_REGEN_VS_CTRL1_reg,		I5_VGIP_I5_VGIP_REGEN_VS_CTRL1_reg,		I6_VGIP_I6_VGIP_REGEN_VS_CTRL1_reg};
const unsigned int vgip_regen_vs_ctrl2_reg_addr[MAX_IDDMA_DEV] =		{DMA_VGIP_DMA_VGIP_REGEN_VS_CTRL2_reg,	I4_VGIP_I4_VGIP_REGEN_VS_CTRL2_reg,		I5_VGIP_I5_VGIP_REGEN_VS_CTRL2_reg,		I6_VGIP_I6_VGIP_REGEN_VS_CTRL2_reg};
//----------------------------------end


//iddma i3,i4,i5,i6 register array
//++++++++++++++++++++++++++++++++++start
const unsigned int iddma_db_reg_addr[MAX_IDDMA_DEV] = 						{H3DDMA_H3DDMA_PQ_CMP_DOUBLE_BUFFER_CTRL_reg, 	H4DDMA_I4DMA_DOUBLE_BUFFER_CTRL_reg,				H5DDMA_I5DMA_DOUBLE_BUFFER_CTRL_reg,				H6DDMA_I6DMA_DOUBLE_BUFFER_CTRL_reg};
const unsigned int iddma_dma_enable_reg_addr[MAX_IDDMA_DEV]	=				{H3DDMA_I3DDMA_enable_reg,						H4DDMA_I4DMA_enable_reg,							H5DDMA_I5DMA_enable_reg,							H6DDMA_I6DMA_enable_reg};
const unsigned int iddma_cap1_ctrl_reg_addr[MAX_IDDMA_DEV] =				{H3DDMA_CAP1_Cap_CTL0_reg,						H4DDMA_I4DMA_CAP1_Cap_CTL0_reg,						H5DDMA_I5DMA_CAP1_Cap_CTL0_reg,						H6DDMA_I6DMA_CAP1_Cap_CTL0_reg};
const unsigned int iddma_cap2_ctrl_reg_addr[MAX_IDDMA_DEV] =				{H3DDMA_CAP2_Cap_CTL0_reg,						H4DDMA_I4DMA_CAP2_Cap_CTL0_reg,						H5DDMA_I5DMA_CAP2_Cap_CTL0_reg,						H6DDMA_I6DMA_CAP2_Cap_CTL0_reg};
const unsigned int iddma_cap2_1_ctrl_reg_addr[MAX_IDDMA_DEV] =				{H3DDMA_CAP2_1_Cap_CTL0_reg,					H4DDMA_I4DMA_CAP2_1_Cap_CTL0_reg,					H5DDMA_I5DMA_CAP2_1_Cap_CTL0_reg,					H6DDMA_I6DMA_CAP2_1_Cap_CTL0_reg};
const unsigned int iddma_cap1_line_burst_unm_reg_addr[MAX_IDDMA_DEV] =		{H3DDMA_CAP1_WR_DMA_num_bl_wrap_word_reg,		H4DDMA_I4DMA_CAP1_WR_DMA_num_bl_wrap_word_reg,		H5DDMA_I5DMA_CAP1_WR_DMA_num_bl_wrap_word_reg,		H6DDMA_I6DMA_CAP1_WR_DMA_num_bl_wrap_word_reg};
const unsigned int iddma_cap1_line_len_reg_addr[MAX_IDDMA_DEV] =			{H3DDMA_CAP1_WR_DMA_num_bl_wrap_ctl_reg,		H4DDMA_I4DMA_CAP1_WR_DMA_num_bl_wrap_ctl_reg,		H5DDMA_I5DMA_CAP1_WR_DMA_num_bl_wrap_ctl_reg,		H6DDMA_I6DMA_CAP1_WR_DMA_num_bl_wrap_ctl_reg};
const unsigned int iddma_cap1_line_step_reg_addr[MAX_IDDMA_DEV] =			{H3DDMA_CAP1_WR_DMA_num_bl_wrap_line_step_reg,	H4DDMA_I4DMA_CAP1_WR_DMA_num_bl_wrap_line_step_reg,	H5DDMA_I5DMA_CAP1_WR_DMA_num_bl_wrap_line_step_reg,	H6DDMA_I6DMA_CAP1_WR_DMA_num_bl_wrap_line_step_reg};
const unsigned int iddma_cap2_line_burst_unm_reg_addr[MAX_IDDMA_DEV] =		{H3DDMA_CAP2_WR_DMA_num_bl_wrap_word_reg,		H4DDMA_I4DMA_CAP2_WR_DMA_num_bl_wrap_word_reg,		H5DDMA_I5DMA_CAP2_WR_DMA_num_bl_wrap_word_reg,		H6DDMA_I6DMA_CAP2_WR_DMA_num_bl_wrap_word_reg};
const unsigned int iddma_cap2_1_line_burst_unm_reg_addr[MAX_IDDMA_DEV] =	{H3DDMA_CAP2_1_WR_DMA_num_bl_wrap_word_reg,		H4DDMA_I4DMA_CAP2_1_WR_DMA_num_bl_wrap_word_reg,	H5DDMA_I5DMA_CAP2_1_WR_DMA_num_bl_wrap_word_reg,	H6DDMA_I6DMA_CAP2_1_WR_DMA_num_bl_wrap_word_reg};
const unsigned int iddma_cap2_line_len_reg_addr[MAX_IDDMA_DEV] =			{H3DDMA_CAP2_WR_DMA_num_bl_wrap_ctl_reg,		H4DDMA_I4DMA_CAP2_WR_DMA_num_bl_wrap_ctl_reg,		H5DDMA_I5DMA_CAP2_WR_DMA_num_bl_wrap_ctl_reg,		H6DDMA_I6DMA_CAP2_WR_DMA_num_bl_wrap_ctl_reg};
const unsigned int iddma_cap2_1_line_len_reg_addr[MAX_IDDMA_DEV] =			{H3DDMA_CAP2_1_WR_DMA_num_bl_wrap_ctl_reg,		H4DDMA_I4DMA_CAP2_1_WR_DMA_num_bl_wrap_ctl_reg,		H5DDMA_I5DMA_CAP2_1_WR_DMA_num_bl_wrap_ctl_reg,		H6DDMA_I6DMA_CAP2_1_WR_DMA_num_bl_wrap_ctl_reg};
const unsigned int iddma_cap2_line_step_reg_addr[MAX_IDDMA_DEV] =			{H3DDMA_CAP2_WR_DMA_num_bl_wrap_line_step_reg,	H4DDMA_I4DMA_CAP2_WR_DMA_num_bl_wrap_line_step_reg,	H5DDMA_I5DMA_CAP2_WR_DMA_num_bl_wrap_line_step_reg,	H6DDMA_I6DMA_CAP2_WR_DMA_num_bl_wrap_line_step_reg};
const unsigned int iddma_cap2_1_line_step_reg_addr[MAX_IDDMA_DEV] =			{H3DDMA_CAP2_1_WR_DMA_num_bl_wrap_line_step_reg,H4DDMA_I4DMA_CAP2_1_WR_DMA_num_bl_wrap_line_step_reg,H5DDMA_I5DMA_CAP2_1_WR_DMA_num_bl_wrap_line_step_reg,H6DDMA_I6DMA_CAP2_1_WR_DMA_num_bl_wrap_line_step_reg};
const unsigned int iddma_cap2_pixel_encode_reg_addr[MAX_IDDMA_DEV] = 		{H3DDMA_CAP2_WR_DMA_pxltobus_reg,				H4DDMA_I4DMA_CAP2_WR_DMA_pxltobus_reg,				H5DDMA_I5DMA_CAP2_WR_DMA_pxltobus_reg,				H6DDMA_I6DMA_CAP2_WR_DMA_pxltobus_reg};
const unsigned int iddma_lr_separate_ctrl3_reg_addr[MAX_IDDMA_DEV] = 		{H3DDMA_LR_Separate_CTRL3_reg,					H4DDMA_I4DMA_LR_Separate_CTRL3_reg,					H5DDMA_I5DMA_LR_Separate_CTRL3_reg,					H6DDMA_I6DMA_LR_Separate_CTRL3_reg};
const unsigned int iddma_cap1_dma_swap_reg_addr[MAX_IDDMA_DEV] =			{H3DDMA_CAP1_CTI_DMA_WR_Ctrl_reg,				H4DDMA_I4DMA_CAP1_CTI_DMA_WR_Ctrl_reg,				H5DDMA_I5DMA_CAP1_CTI_DMA_WR_Ctrl_reg,				H6DDMA_I6DMA_CAP1_CTI_DMA_WR_Ctrl_reg};
const unsigned int iddma_cap2_dma_swap_reg_addr[MAX_IDDMA_DEV] = 			{H3DDMA_CAP2_CTI_DMA_WR_Ctrl_reg,				H4DDMA_I4DMA_CAP2_CTI_DMA_WR_Ctrl_reg,				H5DDMA_I5DMA_CAP2_CTI_DMA_WR_Ctrl_reg,				H6DDMA_I6DMA_CAP2_CTI_DMA_WR_Ctrl_reg};
const unsigned int iddma_cap2_1_dma_swap_reg_addr[MAX_IDDMA_DEV] = 			{H3DDMA_CAP2_1_CTI_DMA_WR_Ctrl_reg,				H4DDMA_I4DMA_CAP2_1_CTI_DMA_WR_Ctrl_reg,			H5DDMA_I5DMA_CAP2_1_CTI_DMA_WR_Ctrl_reg,			H6DDMA_I6DMA_CAP2_1_CTI_DMA_WR_Ctrl_reg};
const unsigned int iddma_interrupt_enable_reg_addr[MAX_IDDMA_DEV] =			{H3DDMA_Interrupt_Enable_reg,					H4DDMA_I4DMA_Interrupt_Enable_reg,					H5DDMA_I5DMA_Interrupt_Enable_reg,					H6DDMA_I6DMA_Interrupt_Enable_reg};
const unsigned int iddma_cap1_start_reg_addr[MAX_IDDMA_DEV] =				{H3DDMA_CAP1_WR_DMA_num_bl_wrap_addr_0_reg,		H4DDMA_I4DMA_CAP1_WR_DMA_num_bl_wrap_addr_0_reg,	H5DDMA_I5DMA_CAP1_WR_DMA_num_bl_wrap_addr_0_reg,	H6DDMA_I6DMA_CAP1_WR_DMA_num_bl_wrap_addr_0_reg};
const unsigned int iddma_cap2_start_reg_addr[MAX_IDDMA_DEV] =				{H3DDMA_CAP2_WR_DMA_num_bl_wrap_addr_0_reg,		H4DDMA_I4DMA_CAP2_WR_DMA_num_bl_wrap_addr_0_reg,	H5DDMA_I5DMA_CAP2_WR_DMA_num_bl_wrap_addr_0_reg,	H6DDMA_I6DMA_CAP2_WR_DMA_num_bl_wrap_addr_0_reg};
const unsigned int iddma_cap2_1_start_reg_addr[MAX_IDDMA_DEV] =				{H3DDMA_CAP2_1_WR_DMA_num_bl_wrap_addr_0_reg,	H4DDMA_I4DMA_CAP2_1_WR_DMA_num_bl_wrap_addr_0_reg,	H5DDMA_I5DMA_CAP2_1_WR_DMA_num_bl_wrap_addr_0_reg,	H6DDMA_I6DMA_CAP2_1_WR_DMA_num_bl_wrap_addr_0_reg};
const unsigned int iddma_cap1_pixel_encode_reg_addr[MAX_IDDMA_DEV] = 		{H3DDMA_CAP1_WR_DMA_pxltobus_reg,				H4DDMA_I4DMA_CAP1_WR_DMA_pxltobus_reg,				H5DDMA_I5DMA_CAP1_WR_DMA_pxltobus_reg,				H6DDMA_I6DMA_CAP1_WR_DMA_pxltobus_reg};
const unsigned int iddma_cap1_boundary_low_limit_reg_addr[MAX_IDDMA_DEV] =  {H3DDMA_CAP1_CTI_DMA_WR_Rule_check_low_reg,     H4DDMA_I4DMA_CAP1_CTI_DMA_WR_Rule_check_low_reg,    H5DDMA_I5DMA_CAP1_CTI_DMA_WR_Rule_check_low_reg,    H6DDMA_I6DMA_CAP1_CTI_DMA_WR_Rule_check_low_reg};
const unsigned int iddma_cap1_boundary_up_limit_reg_addr[MAX_IDDMA_DEV] =   {H3DDMA_CAP1_CTI_DMA_WR_Rule_check_up_reg,      H4DDMA_I4DMA_CAP1_CTI_DMA_WR_Rule_check_up_reg,     H5DDMA_I5DMA_CAP1_CTI_DMA_WR_Rule_check_up_reg,     H6DDMA_I6DMA_CAP1_CTI_DMA_WR_Rule_check_up_reg};
const unsigned int iddma_cap2_boundary_low_limit_reg_addr[MAX_IDDMA_DEV] =  {H3DDMA_CAP2_CTI_DMA_WR_Rule_check_low_reg,     H4DDMA_I4DMA_CAP2_CTI_DMA_WR_Rule_check_low_reg,    H5DDMA_I5DMA_CAP2_CTI_DMA_WR_Rule_check_low_reg,    H6DDMA_I6DMA_CAP2_CTI_DMA_WR_Rule_check_low_reg};
const unsigned int iddma_cap2_boundary_up_limit_reg_addr[MAX_IDDMA_DEV] =   {H3DDMA_CAP2_CTI_DMA_WR_Rule_check_up_reg,      H4DDMA_I4DMA_CAP2_CTI_DMA_WR_Rule_check_up_reg,     H5DDMA_I5DMA_CAP2_CTI_DMA_WR_Rule_check_up_reg,     H6DDMA_I6DMA_CAP2_CTI_DMA_WR_Rule_check_up_reg};
const unsigned int iddma_cap2_1_boundary_low_limit_reg_addr[MAX_IDDMA_DEV] ={H3DDMA_CAP2_1_CTI_DMA_WR_Rule_check_low_reg,   H4DDMA_I4DMA_CAP2_1_CTI_DMA_WR_Rule_check_low_reg,  H5DDMA_I5DMA_CAP2_1_CTI_DMA_WR_Rule_check_low_reg,  H6DDMA_I6DMA_CAP2_1_CTI_DMA_WR_Rule_check_low_reg};
const unsigned int iddma_cap2_1_boundary_up_limit_reg_addr[MAX_IDDMA_DEV] = {H3DDMA_CAP2_1_CTI_DMA_WR_Rule_check_up_reg,    H4DDMA_I4DMA_CAP2_1_CTI_DMA_WR_Rule_check_up_reg,   H5DDMA_I5DMA_CAP2_1_CTI_DMA_WR_Rule_check_up_reg,   H6DDMA_I6DMA_CAP2_1_CTI_DMA_WR_Rule_check_up_reg};
//----------------------------------end

//dither i3,i4,i5,i6 register array
//++++++++++++++++++++++++++++++++++start
const unsigned int i3ddma_dither_ctrl1_reg_addr[MAX_IDDMA_DEV] = 			{H3DDMA_1_RGB2YUV_DITHER_4XXTO4XX_HDMI3_1_DITHER_CTRL1_reg,					H4DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI4_DITHER_CTRL1_reg,			H5DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI5_DITHER_CTRL1_reg,			H6DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI6_DITHER_CTRL1_reg};
const unsigned int i3ddma_dither_seq_r_00_15_reg_addr[MAX_IDDMA_DEV] =		{H3DDMA_1_RGB2YUV_DITHER_4XXTO4XX_HDMI3_1_DITHER_SEQ_R_00_15_reg,			H4DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI4_DITHER_SEQ_R_00_15_reg,	H5DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI5_DITHER_SEQ_R_00_15_reg,	H6DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI6_DITHER_SEQ_R_00_15_reg};
const unsigned int i3ddma_dither_seq_g_00_07_reg_addr[MAX_IDDMA_DEV] = 		{H3DDMA_1_RGB2YUV_DITHER_4XXTO4XX_HDMI3_1_DITHER_SEQ_G_00_07_reg,			H4DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI4_DITHER_SEQ_G_00_07_reg,	H5DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI5_DITHER_SEQ_G_00_07_reg,	H6DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI6_DITHER_SEQ_G_00_07_reg};
const unsigned int i3ddma_dither_seq_b_00_15_reg_addr[MAX_IDDMA_DEV] = 		{H3DDMA_1_RGB2YUV_DITHER_4XXTO4XX_HDMI3_1_DITHER_SEQ_B_00_15_reg,			H4DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI4_DITHER_SEQ_B_00_15_reg,	H5DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI5_DITHER_SEQ_B_00_15_reg,	H6DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI6_DITHER_SEQ_B_00_15_reg};
const unsigned int i3ddma_dither_seq_r_16_31_reg_addr[MAX_IDDMA_DEV] = 		{H3DDMA_1_RGB2YUV_DITHER_4XXTO4XX_HDMI3_1_DITHER_SEQ_R_16_31_reg,			H4DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI4_DITHER_SEQ_R_16_31_reg,	H5DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI5_DITHER_SEQ_R_16_31_reg,	H6DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI6_DITHER_SEQ_R_16_31_reg};
const unsigned int i3ddma_dither_seq_g_16_31_reg_addr[MAX_IDDMA_DEV] = 		{H3DDMA_1_RGB2YUV_DITHER_4XXTO4XX_HDMI3_1_DITHER_SEQ_G_16_31_reg,			H4DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI4_DITHER_SEQ_G_16_31_reg,	H5DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI5_DITHER_SEQ_G_16_31_reg,	H6DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI6_DITHER_SEQ_G_16_31_reg};
const unsigned int i3ddma_dither_seq_b_16_31_reg_addr[MAX_IDDMA_DEV] =		{H3DDMA_1_RGB2YUV_DITHER_4XXTO4XX_HDMI3_1_DITHER_SEQ_B_16_31_reg,			H4DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI4_DITHER_SEQ_B_16_31_reg,	H5DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI5_DITHER_SEQ_B_16_31_reg,	H6DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI6_DITHER_SEQ_B_16_31_reg};
const unsigned int i3ddma_dither_tb_r_00_reg_addr[MAX_IDDMA_DEV] =			{H3DDMA_1_RGB2YUV_DITHER_4XXTO4XX_HDMI3_1_DITHER_TB_R_00_reg,				H4DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI4_DITHER_TB_R_00_reg,		H5DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI5_DITHER_TB_R_00_reg,		H6DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI6_DITHER_TB_R_00_reg};
const unsigned int i3ddma_dither_tb_g_00_reg_addr[MAX_IDDMA_DEV] = 			{H3DDMA_1_RGB2YUV_DITHER_4XXTO4XX_HDMI3_1_DITHER_TB_G_00_reg,				H4DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI4_DITHER_TB_G_00_reg,		H5DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI5_DITHER_TB_G_00_reg,		H6DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI6_DITHER_TB_G_00_reg};
const unsigned int i3ddma_dither_tb_b_00_reg_addr[MAX_IDDMA_DEV] =			{H3DDMA_1_RGB2YUV_DITHER_4XXTO4XX_HDMI3_1_DITHER_TB_B_00_reg,				H4DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI4_DITHER_TB_B_00_reg,		H5DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI5_DITHER_TB_B_00_reg,		H6DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI6_DITHER_TB_B_00_reg};
const unsigned int i3ddma_dither_tb_r_02_reg_addr[MAX_IDDMA_DEV] = 			{H3DDMA_1_RGB2YUV_DITHER_4XXTO4XX_HDMI3_1_DITHER_TB_R_02_reg,				H4DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI4_DITHER_TB_R_02_reg,		H5DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI5_DITHER_TB_R_02_reg,		H6DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI6_DITHER_TB_R_02_reg};
const unsigned int i3ddma_dither_tb_g_02_reg_addr[MAX_IDDMA_DEV] = 			{H3DDMA_1_RGB2YUV_DITHER_4XXTO4XX_HDMI3_1_DITHER_TB_G_02_reg,				H4DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI4_DITHER_TB_G_02_reg,		H5DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI5_DITHER_TB_G_02_reg,		H6DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI6_DITHER_TB_G_02_reg};
const unsigned int i3ddma_dither_tb_b_02_reg_addr[MAX_IDDMA_DEV] = 			{H3DDMA_1_RGB2YUV_DITHER_4XXTO4XX_HDMI3_1_DITHER_TB_B_02_reg,				H4DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI4_DITHER_TB_B_02_reg,		H5DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI5_DITHER_TB_B_02_reg,		H6DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI6_DITHER_TB_B_02_reg};
const unsigned int i3ddma_dither_tmp_tb_reg_addr[MAX_IDDMA_DEV] =			{H3DDMA_1_RGB2YUV_DITHER_4XXTO4XX_HDMI3_1_DITHER_Temp_TB_reg,				H4DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI4_DITHER_Temp_TB_reg,		H5DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI5_DITHER_Temp_TB_reg,		H6DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI6_DITHER_Temp_TB_reg};
const unsigned int i3ddma_rgb2yuv_ctrl_reg_addr[MAX_IDDMA_DEV] =			{H3DDMA_1_RGB2YUV_DITHER_4XXTO4XX_HDMI3_1_RGB2YUV_CTRL_reg,					H4DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI4_RGB2YUV_CTRL_reg,			H5DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI5_RGB2YUV_CTRL_reg,			H6DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI6_RGB2YUV_CTRL_reg};
const unsigned int i3ddma_422to444_ctrl_reg_addr[MAX_IDDMA_DEV] =			{H3DDMA_1_RGB2YUV_DITHER_4XXTO4XX_HDMI3_1_422to444_CTRL_reg,				H4DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI4_422to444_CTRL_reg,			H5DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI5_422to444_CTRL_reg,			H6DDMA_RGB2YUV_DITHER_4XXTO4XX_HDMI6_422to444_CTRL_reg};
//----------------------------------end

//uzd i3,i4,i5,i6 register array
//++++++++++++++++++++++++++++++++++start
const unsigned int iddma_uzd_h_db_reg_addr[MAX_IDDMA_DEV] =					{H3DDMA_HSD_I3DDMA1_UZD_H_DB_CTRL_reg,				H4DDMA_HSD_I4DDMA_UZD_H_DB_CTRL_reg,			H5DDMA_HSD_I5DDMA_UZD_H_DB_CTRL_reg,			H6DDMA_HSD_I6DDMA_UZD_H_DB_CTRL_reg};
const unsigned int iddma_uzd_v_db_reg_addr[MAX_IDDMA_DEV] =					{H3DDMA_HSD_I3DDMA1_UZD_V_DB_CTRL_reg,				H4DDMA_HSD_I4DDMA_UZD_V_DB_CTRL_reg,			H5DDMA_HSD_I5DDMA_UZD_V_DB_CTRL_reg,			H6DDMA_HSD_I6DDMA_UZD_V_DB_CTRL_reg};
const unsigned int iddma_uzd_ctrl_reg_addr[MAX_IDDMA_DEV] = 				{H3DDMA_HSD_I3DDMA1_UZD_CTRL0_reg,					H4DDMA_HSD_I4DDMA_UZD_CTRL0_reg,				H5DDMA_HSD_I5DDMA_UZD_CTRL0_reg,				H6DDMA_HSD_I6DDMA_UZD_CTRL0_reg};
const unsigned int iddma_uzd_ctrl1_reg_addr[MAX_IDDMA_DEV] =                {H3DDMA_HSD_I3DDMA1_UZD_CTRL1_reg,                  H4DDMA_HSD_I4DDMA_UZD_CTRL1_reg,                H5DDMA_HSD_I5DDMA_UZD_CTRL1_reg,                H6DDMA_HSD_I6DDMA_UZD_CTRL1_reg};
const unsigned int iddma_uzd_hor_factor_reg_addr[MAX_IDDMA_DEV] =			{H3DDMA_HSD_I3DDMA1_UZD_SCALE_HOR_FACTOR_reg,		H4DDMA_HSD_I4DDMA_UZD_SCALE_HOR_FACTOR_reg,		H5DDMA_HSD_I5DDMA_UZD_SCALE_HOR_FACTOR_reg,		H6DDMA_HSD_I6DDMA_UZD_SCALE_HOR_FACTOR_reg};
const unsigned int iddma_uzd_ver_factor_reg_addr[MAX_IDDMA_DEV] =			{H3DDMA_HSD_I3DDMA1_UZD_SCALE_VER_FACTOR_reg,		H4DDMA_HSD_I4DDMA_UZD_SCALE_VER_FACTOR_reg,		H5DDMA_HSD_I5DDMA_UZD_SCALE_VER_FACTOR_reg,		H6DDMA_HSD_I6DDMA_UZD_SCALE_VER_FACTOR_reg};
const unsigned int iddma_uzd_init_value_reg_addr[MAX_IDDMA_DEV] = 			{H3DDMA_HSD_I3DDMA1_UZD_INITIAL_VALUE_reg,			H4DDMA_HSD_I4DDMA_UZD_INITIAL_VALUE_reg,		H5DDMA_HSD_I5DDMA_UZD_INITIAL_VALUE_reg,		H6DDMA_HSD_I6DDMA_UZD_INITIAL_VALUE_reg};
const unsigned int iddma_uzd_init_int_reg_addr[MAX_IDDMA_DEV] =				{H3DDMA_HSD_I3DDMA1_UZD_INITIAL_INT_VALUE_reg,		H4DDMA_HSD_I4DDMA_UZD_INITIAL_INT_VALUE_reg,	H5DDMA_HSD_I5DDMA_UZD_INITIAL_INT_VALUE_reg,	H6DDMA_HSD_I6DDMA_UZD_INITIAL_INT_VALUE_reg};
const unsigned int iddma_uzd_ibuff_ctrl_reg_addr[MAX_IDDMA_DEV] =			{H3DDMA_HSD_I3DDMA1_UZD_IBUFF_CTRL_reg,				H4DDMA_HSD_I4DDMA_UZD_IBUFF_CTRL_reg,			H5DDMA_HSD_I5DDMA_UZD_IBUFF_CTRL_reg,			H6DDMA_HSD_I6DDMA_UZD_IBUFF_CTRL_reg};
const unsigned int iddma_uzd_fir_coef_tab1_c0_reg_addr[MAX_IDDMA_DEV] = 	{H3DDMA_HSD_I3DDMA1_UZD_FIR_COEF_TAB1_C0_reg,		H4DDMA_HSD_I4DDMA_UZD_FIR_COEF_TAB1_C0_reg,		H5DDMA_HSD_I5DDMA_UZD_FIR_COEF_TAB1_C0_reg,		H6DDMA_HSD_I6DDMA_UZD_FIR_COEF_TAB1_C0_reg};
const unsigned int iddma_uzd_fir_coef_tab1_c14_reg_addr[MAX_IDDMA_DEV] =	{H3DDMA_HSD_I3DDMA1_UZD_FIR_COEF_TAB1_C14_reg,		H4DDMA_HSD_I4DDMA_UZD_FIR_COEF_TAB1_C14_reg,	H5DDMA_HSD_I5DDMA_UZD_FIR_COEF_TAB1_C14_reg,	H6DDMA_HSD_I6DDMA_UZD_FIR_COEF_TAB1_C14_reg};
const unsigned int iddma_uzd_fir_coef_tab1_c16_reg_addr[MAX_IDDMA_DEV] =    {H3DDMA_HSD_I3DDMA1_UZD_FIR_COEF_TAB1_C16_reg,		H4DDMA_HSD_I4DDMA_UZD_FIR_COEF_TAB1_C16_reg,	H5DDMA_HSD_I5DDMA_UZD_FIR_COEF_TAB1_C16_reg,	H6DDMA_HSD_I6DDMA_UZD_FIR_COEF_TAB1_C16_reg};
const unsigned int iddma_uzd_fir_coef_tab1_c30_reg_addr[MAX_IDDMA_DEV] =	{H3DDMA_HSD_I3DDMA1_UZD_FIR_COEF_TAB1_C30_reg,		H4DDMA_HSD_I4DDMA_UZD_FIR_COEF_TAB1_C30_reg,	H5DDMA_HSD_I5DDMA_UZD_FIR_COEF_TAB1_C30_reg,	H6DDMA_HSD_I6DDMA_UZD_FIR_COEF_TAB1_C30_reg};
const unsigned int iddma_uzd_fir_coef_tab2_c0_reg_addr[MAX_IDDMA_DEV] =		{H3DDMA_HSD_I3DDMA1_UZD_FIR_COEF_TAB2_C0_reg,		H4DDMA_HSD_I4DDMA_UZD_FIR_COEF_TAB2_C0_reg,		H5DDMA_HSD_I5DDMA_UZD_FIR_COEF_TAB2_C0_reg,		H6DDMA_HSD_I6DDMA_UZD_FIR_COEF_TAB2_C0_reg};
const unsigned int iddma_uzd_coef_db_ctrl_reg_addr[MAX_IDDMA_DEV] =         {H3DDMA_HSD_I3DDMA1_UZD_COEF_DB_CTRL_reg,           H4DDMA_HSD_I4DDMA_UZD_COEF_DB_CTRL_reg,         H5DDMA_HSD_I5DDMA_UZD_COEF_DB_CTRL_reg,         H6DDMA_HSD_I6DDMA_UZD_COEF_DB_CTRL_reg};
//----------------------------------end


