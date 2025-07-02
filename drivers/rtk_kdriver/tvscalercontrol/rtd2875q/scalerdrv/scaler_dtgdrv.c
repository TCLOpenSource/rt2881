#include <rbus/ppoverlay_reg.h>
#include <tvscalercontrol/io/ioregdrv.h>

void drvif_scaler_dtg_set_dsubreg_db(unsigned char bflag1, unsigned char bflag2)
{
    ppoverlay_double_buffer_ctrl4_RBUS ppoverlay_double_buffer_ctrl4_Reg;

    ppoverlay_double_buffer_ctrl4_Reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL4_reg);
    ppoverlay_double_buffer_ctrl4_Reg.dsubreg_dbuf_en = bflag1;
    ppoverlay_double_buffer_ctrl4_Reg.dsubreg_dbuf_set = bflag2;
    IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL4_reg, ppoverlay_double_buffer_ctrl4_Reg.regValue);
}

void drvif_scaler_dtg_set_dsubreg_dbuf_en(unsigned char bflag)
{
    ppoverlay_double_buffer_ctrl4_RBUS ppoverlay_double_buffer_ctrl4_Reg;

    ppoverlay_double_buffer_ctrl4_Reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL4_reg);
    ppoverlay_double_buffer_ctrl4_Reg.dsubreg_dbuf_en = bflag;
    IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL4_reg, ppoverlay_double_buffer_ctrl4_Reg.regValue);
}

void drvif_scaler_dtg_set_dsubreg_dbuf_set(unsigned char bflag)
{
    ppoverlay_double_buffer_ctrl4_RBUS ppoverlay_double_buffer_ctrl4_Reg;

    ppoverlay_double_buffer_ctrl4_Reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL4_reg);
    ppoverlay_double_buffer_ctrl4_Reg.dsubreg_dbuf_set = bflag;
    IoReg_Write32(PPOVERLAY_Double_Buffer_CTRL4_reg, ppoverlay_double_buffer_ctrl4_Reg.regValue);
}

unsigned char drvif_scaler_dtg_get_dsubreg_dbuf_set(void)
{
    ppoverlay_double_buffer_ctrl4_RBUS ppoverlay_double_buffer_ctrl4_Reg;

    ppoverlay_double_buffer_ctrl4_Reg.regValue = IoReg_Read32(PPOVERLAY_Double_Buffer_CTRL4_reg);
    
    return ppoverlay_double_buffer_ctrl4_Reg.dsubreg_dbuf_set;
}
unsigned char drvif_scaler_support_panel_type_by_opt(unsigned long long dclk,unsigned long long iclk)
{
	return 0;
}