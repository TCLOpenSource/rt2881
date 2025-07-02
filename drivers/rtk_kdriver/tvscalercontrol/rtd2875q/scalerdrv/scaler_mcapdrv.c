#include <rbus/mdomain_cap_reg.h>
#include <tvscalercontrol/io/ioregdrv.h>


void drvif_scaler_mcap_set_cap1_db(unsigned char bflag1, unsigned char bflag2)
{
    mdomain_cap_cap_reg_doublbuffer_RBUS mdomain_cap_cap_reg_doublbuffer_reg;

    mdomain_cap_cap_reg_doublbuffer_reg.regValue = IoReg_Read32(MDOMAIN_CAP_cap_reg_doublbuffer_reg);
    mdomain_cap_cap_reg_doublbuffer_reg.cap1_db_en = bflag1;
    mdomain_cap_cap_reg_doublbuffer_reg.cap1_db_apply = bflag2;
    IoReg_Write32(MDOMAIN_CAP_cap_reg_doublbuffer_reg,mdomain_cap_cap_reg_doublbuffer_reg.regValue);
}

void drvif_scaler_mcap_set_cap1_db_en(unsigned char bflag)
{
    mdomain_cap_cap_reg_doublbuffer_RBUS mdomain_cap_cap_reg_doublbuffer_reg;

    mdomain_cap_cap_reg_doublbuffer_reg.regValue = IoReg_Read32(MDOMAIN_CAP_cap_reg_doublbuffer_reg);
    mdomain_cap_cap_reg_doublbuffer_reg.cap1_db_en = bflag;
    IoReg_Write32(MDOMAIN_CAP_cap_reg_doublbuffer_reg,mdomain_cap_cap_reg_doublbuffer_reg.regValue);
}

unsigned char drvif_scaler_mcap_get_cap1_db_en(void)
{
    mdomain_cap_cap_reg_doublbuffer_RBUS mdomain_cap_cap_reg_doublbuffer_reg;

    mdomain_cap_cap_reg_doublbuffer_reg.regValue = IoReg_Read32(MDOMAIN_CAP_cap_reg_doublbuffer_reg);
    return mdomain_cap_cap_reg_doublbuffer_reg.cap1_db_en;
}

void drvif_scaler_mcap_set_cap1_db_apply(unsigned char bflag)
{
    mdomain_cap_cap_reg_doublbuffer_RBUS mdomain_cap_cap_reg_doublbuffer_reg;

    mdomain_cap_cap_reg_doublbuffer_reg.regValue =IoReg_Read32(MDOMAIN_CAP_cap_reg_doublbuffer_reg);
    mdomain_cap_cap_reg_doublbuffer_reg.cap1_db_apply = bflag;
    IoReg_Write32(MDOMAIN_CAP_cap_reg_doublbuffer_reg,mdomain_cap_cap_reg_doublbuffer_reg.regValue);
}

unsigned char drvif_scaler_mcap_get_cap1_db_apply(void)
{
    mdomain_cap_cap_reg_doublbuffer_RBUS mdomain_cap_cap_reg_doublbuffer_reg;

    mdomain_cap_cap_reg_doublbuffer_reg.regValue =IoReg_Read32(MDOMAIN_CAP_cap_reg_doublbuffer_reg);
    return mdomain_cap_cap_reg_doublbuffer_reg.cap1_db_apply;
}

void drvif_scaler_mcap_set_cap2_db(unsigned char bflag1, unsigned char bflag2)
{
    mdomain_cap_cap_reg_doublbuffer_1_RBUS mdomain_cap_cap_reg_doublbuffer_1_reg;

    mdomain_cap_cap_reg_doublbuffer_1_reg.regValue = IoReg_Read32(MDOMAIN_CAP_cap_reg_doublbuffer_1_reg);
    mdomain_cap_cap_reg_doublbuffer_1_reg.cap2_db_en = bflag1;
    mdomain_cap_cap_reg_doublbuffer_1_reg.cap2_db_apply = bflag2;
    IoReg_Write32(MDOMAIN_CAP_cap_reg_doublbuffer_1_reg,mdomain_cap_cap_reg_doublbuffer_1_reg.regValue);
}

void drvif_scaler_mcap_set_cap2_db_en(unsigned char bflag)
{
    mdomain_cap_cap_reg_doublbuffer_1_RBUS mdomain_cap_cap_reg_doublbuffer_1_reg;

    mdomain_cap_cap_reg_doublbuffer_1_reg.regValue = IoReg_Read32(MDOMAIN_CAP_cap_reg_doublbuffer_1_reg);
    mdomain_cap_cap_reg_doublbuffer_1_reg.cap2_db_en = bflag;
    IoReg_Write32(MDOMAIN_CAP_cap_reg_doublbuffer_1_reg,mdomain_cap_cap_reg_doublbuffer_1_reg.regValue);
}

unsigned char drvif_scaler_mcap_get_cap2_db_en(void)
{
    mdomain_cap_cap_reg_doublbuffer_1_RBUS mdomain_cap_cap_reg_doublbuffer_1_reg;

    mdomain_cap_cap_reg_doublbuffer_1_reg.regValue = IoReg_Read32(MDOMAIN_CAP_cap_reg_doublbuffer_1_reg);
    return mdomain_cap_cap_reg_doublbuffer_1_reg.cap2_db_en;
}

void drvif_scaler_mcap_set_cap2_db_apply(unsigned char bflag)
{
    mdomain_cap_cap_reg_doublbuffer_1_RBUS mdomain_cap_cap_reg_doublbuffer_1_reg;

    mdomain_cap_cap_reg_doublbuffer_1_reg.regValue = IoReg_Read32(MDOMAIN_CAP_cap_reg_doublbuffer_1_reg);
    mdomain_cap_cap_reg_doublbuffer_1_reg.cap2_db_apply = bflag;
    IoReg_Write32(MDOMAIN_CAP_cap_reg_doublbuffer_1_reg, mdomain_cap_cap_reg_doublbuffer_1_reg.regValue);
}


unsigned char drvif_scaler_mcap_get_cap2_db_apply(void)
{
    mdomain_cap_cap_reg_doublbuffer_1_RBUS mdomain_cap_cap_reg_doublbuffer_1_reg;

    mdomain_cap_cap_reg_doublbuffer_1_reg.regValue = IoReg_Read32(MDOMAIN_CAP_cap_reg_doublbuffer_1_reg);
    return mdomain_cap_cap_reg_doublbuffer_1_reg.cap2_db_apply;
}

void drvif_scaler_mcap_set_in2_pqc_clken(unsigned char bflag)
{
    mdomain_cap_smooth_tog_ctrl_1_RBUS mdomain_cap_smooth_tog_ctrl_1_reg;

    mdomain_cap_smooth_tog_ctrl_1_reg.regValue = IoReg_Read32(MDOMAIN_CAP_Smooth_tog_ctrl_1_reg);
    mdomain_cap_smooth_tog_ctrl_1_reg.in2_pqc_clken = bflag;
    IoReg_Write32(MDOMAIN_CAP_Smooth_tog_ctrl_1_reg, mdomain_cap_smooth_tog_ctrl_1_reg.regValue);
}