#include <rbus/mdomain_disp_reg.h>
#include <tvscalercontrol/io/ioregdrv.h>


void drvif_scaler_mdisp_set_disp1_db(unsigned char bflag1, unsigned char bflag2)
{
    mdomain_disp_ddr_mainsubctrl_RBUS mdomain_disp_ddr_mainsubctrl_Reg;

    mdomain_disp_ddr_mainsubctrl_Reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
    mdomain_disp_ddr_mainsubctrl_Reg.disp1_double_enable = bflag1;
    mdomain_disp_ddr_mainsubctrl_Reg.disp1_double_apply = bflag2;
    IoReg_Write32(MDOMAIN_DISP_DDR_MainSubCtrl_reg, mdomain_disp_ddr_mainsubctrl_Reg.regValue);
}

void drvif_scaler_mdisp_set_disp1_double_enable(unsigned char bflag)
{
    mdomain_disp_ddr_mainsubctrl_RBUS mdomain_disp_ddr_mainsubctrl_Reg;

    mdomain_disp_ddr_mainsubctrl_Reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
    mdomain_disp_ddr_mainsubctrl_Reg.disp1_double_enable = bflag;
    IoReg_Write32(MDOMAIN_DISP_DDR_MainSubCtrl_reg, mdomain_disp_ddr_mainsubctrl_Reg.regValue);
}

unsigned char drvif_scaler_mdisp_get_disp1_double_enable(void)
{
    mdomain_disp_ddr_mainsubctrl_RBUS mdomain_disp_ddr_mainsubctrl_Reg;

    mdomain_disp_ddr_mainsubctrl_Reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
    return mdomain_disp_ddr_mainsubctrl_Reg.disp1_double_enable;
}

void drvif_scaler_mdisp_set_disp1_double_apply(unsigned char bflag)
{
    mdomain_disp_ddr_mainsubctrl_RBUS mdomain_disp_ddr_mainsubctrl_Reg;

    mdomain_disp_ddr_mainsubctrl_Reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
    mdomain_disp_ddr_mainsubctrl_Reg.disp1_double_apply = bflag;
    IoReg_Write32(MDOMAIN_DISP_DDR_MainSubCtrl_reg, mdomain_disp_ddr_mainsubctrl_Reg.regValue);
}

unsigned char drvif_scaler_mdisp_get_disp1_double_apply(void)
{
    mdomain_disp_ddr_mainsubctrl_RBUS mdomain_disp_ddr_mainsubctrl_Reg;

    mdomain_disp_ddr_mainsubctrl_Reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainSubCtrl_reg);
    return mdomain_disp_ddr_mainsubctrl_Reg.disp1_double_apply;
}

void drvif_scaler_mdisp_set_disp2_db(unsigned char bflag1, unsigned char bflag2)
{
    mdomain_disp_ddr_mainsubctrl_1_RBUS mdomain_disp_ddr_mainsubctrl_1_Reg;

    mdomain_disp_ddr_mainsubctrl_1_Reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainSubCtrl_1_reg);
    mdomain_disp_ddr_mainsubctrl_1_Reg.disp2_double_enable = bflag1;
    mdomain_disp_ddr_mainsubctrl_1_Reg.disp2_double_apply = bflag2;
    IoReg_Write32(MDOMAIN_DISP_DDR_MainSubCtrl_1_reg, mdomain_disp_ddr_mainsubctrl_1_Reg.regValue);
}

void drvif_scaler_mdisp_set_disp2_double_enable(unsigned char bflag)
{
    mdomain_disp_ddr_mainsubctrl_1_RBUS mdomain_disp_ddr_mainsubctrl_1_Reg;

    mdomain_disp_ddr_mainsubctrl_1_Reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainSubCtrl_1_reg);
    mdomain_disp_ddr_mainsubctrl_1_Reg.disp2_double_enable = bflag;
    IoReg_Write32(MDOMAIN_DISP_DDR_MainSubCtrl_1_reg, mdomain_disp_ddr_mainsubctrl_1_Reg.regValue);
}

void drvif_scaler_mdisp_set_disp2_double_apply(unsigned char bflag)
{
    mdomain_disp_ddr_mainsubctrl_1_RBUS mdomain_disp_ddr_mainsubctrl_1_Reg;

    mdomain_disp_ddr_mainsubctrl_1_Reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainSubCtrl_1_reg);
    mdomain_disp_ddr_mainsubctrl_1_Reg.disp2_double_apply = bflag;
    IoReg_Write32(MDOMAIN_DISP_DDR_MainSubCtrl_1_reg, mdomain_disp_ddr_mainsubctrl_1_Reg.regValue);
}

unsigned char drvif_scaler_mdisp_get_disp2_double_apply(void)
{
    mdomain_disp_ddr_mainsubctrl_1_RBUS mdomain_disp_ddr_mainsubctrl_1_Reg;

    mdomain_disp_ddr_mainsubctrl_1_Reg.regValue = IoReg_Read32(MDOMAIN_DISP_DDR_MainSubCtrl_1_reg);
    return mdomain_disp_ddr_mainsubctrl_1_Reg.disp2_double_apply;
}