#include <linux/types.h>

#include <rbus/h3ddma_reg.h>
#include <rbus/vodma_reg.h>
#include <tvscalercontrol/io/ioregdrv.h>

#include <tvscalercontrol/i3ddma/i3ddma.h>
#include <tvscalercontrol/scalerdrv/scaler_i3ddmadrv.h>


void drvif_scaler_i3ddma_set_pqc_frame_limit_bit(unsigned int comp_ratio)
{
	h3ddma_h3ddma_pq_cmp_bit_RBUS h3ddma_h3ddma_pq_cmp_bit_reg;
	
	h3ddma_h3ddma_pq_cmp_bit_reg.regValue = IoReg_Read32(H3DDMA_H3DDMA_PQ_CMP_BIT_reg);
	h3ddma_h3ddma_pq_cmp_bit_reg.frame_limit_bit = comp_ratio;
	IoReg_Write32(H3DDMA_H3DDMA_PQ_CMP_BIT_reg, h3ddma_h3ddma_pq_cmp_bit_reg.regValue);
}

void drvif_scaler_i3ddma_set_pqc_line_limit_bit(unsigned int comp_ratio)
{
	h3ddma_h3ddma_pq_cmp_bit_RBUS h3ddma_h3ddma_pq_cmp_bit_reg;
	
	h3ddma_h3ddma_pq_cmp_bit_reg.regValue = IoReg_Read32(H3DDMA_H3DDMA_PQ_CMP_BIT_reg);
	h3ddma_h3ddma_pq_cmp_bit_reg.line_limit_bit = comp_ratio;
	IoReg_Write32(H3DDMA_H3DDMA_PQ_CMP_BIT_reg, h3ddma_h3ddma_pq_cmp_bit_reg.regValue);
}

void drvif_scaler_i3ddma_set_nn_inerrupt_en(unsigned char bOnOff)
{
	//h3ddma_interrupt_enable_RBUS h3ddma_interrupt_enable_reg;
	
	//h3ddma_interrupt_enable_reg.regValue = IoReg_Read32(H3DDMA_Interrupt_Enable_reg);
	//h3ddma_interrupt_enable_reg.cap1_last_wr_ie = bOnOff;
	//h3ddma_interrupt_enable_reg.cap1_last_wr_int_mux = bOnOff;
	//IoReg_Write32(H3DDMA_Interrupt_Enable_reg,h3ddma_interrupt_enable_reg.regValue);
}
#if 0 // need check
unsigned int calc_i3ddma_comp_line_sum_bit(unsigned int comp_wid, unsigned int comp_ratio, unsigned char cmp_alpha_en)
{
    unsigned int cmp_line_sum_bit = 0;
        /*
    width need to align 32
    line sum bit = (width * compression bit + 256) / 128
    for new PQC and PQDC @Crixus 20170615
    */
	comp_wid = CEIL_MODE(comp_wid, 32);
    cmp_line_sum_bit = (comp_wid * comp_ratio + 256);
    cmp_line_sum_bit = CEIL_MODE(cmp_line_sum_bit, 128) / 128;

    return cmp_line_sum_bit;
}
#endif
void set_dma_v1_blk_reg(VIDEO_RPC_VOUT_CONFIG_HDMI_3D *HDMI_INFO){
    vodma_vodma_v1_line_blk1_RBUS        V1_y_line_sel ;        //0xb800_5008
    vodma_vodma_v1_line_blk2_RBUS        V1_c_line_sel ;        //0xb800_500C
    //vodma_vodma_v1_blk1_RBUS		DMA_v1_hoffset;		//0xb800_5010
    //vodma_vodma_v1_blk2_RBUS		DMA_v1_voffset;		//0xb800_5014
    //vodma_vodma_v1_blk3_RBUS		DMA_v1_voffset2;	//0xb800_5018
    //vodma_vodma_v1_blk4_RBUS		DMA_v1_hoffset2;		//0xb800_5010
    //vodma_vodma_v1_blk5_RBUS		DMA_v1_hoffset3;		//0xb800_5010

    V1_y_line_sel.y_auto_line_step = 1; // 0: FW mode, 1: HW mode
    V1_y_line_sel.y_line_step = 0;
    V1_c_line_sel.c_auto_line_step = 1; // 0: FW mode, 1: HW mode
    V1_c_line_sel.c_line_step = 0;
    V1_y_line_sel.y_line_select = 0;
    V1_c_line_sel.c_line_select = 0;

    IoReg_Write32(VODMA_VODMA_V1_LINE_BLK1_reg, V1_y_line_sel.regValue);
    IoReg_Write32(VODMA_VODMA_V1_LINE_BLK2_reg, V1_c_line_sel.regValue);
    // IoReg_Write32(VODMA_VODMA_V1_BLK1_reg, DMA_v1_hoffset.regValue);
    // IoReg_Write32(VODMA_VODMA_V1_BLK2_reg, DMA_v1_voffset.regValue);
    // IoReg_Write32(VODMA_VODMA_V1_BLK3_reg, DMA_v1_voffset2.regValue);
    // IoReg_Write32(VODMA_VODMA_V1_BLK4_reg, DMA_v1_hoffset2.regValue);
    // IoReg_Write32(VODMA_VODMA_V1_BLK5_reg, DMA_v1_hoffset3.regValue);

}

