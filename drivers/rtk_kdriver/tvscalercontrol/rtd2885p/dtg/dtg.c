#include <linux/kernel.h>
#include <tvscalercontrol/dtg/dtg.h>
#include <rbus/M8P_ppoverlay_reg.h>
#include <tvscalercontrol/io/ioregdrv.h>

int m8p_dtg_set_double_buffer_enable(DTG_DOUBLE_BUFFER_INDEX index, unsigned char *enable)
{
    M8P_ppoverlay_double_buffer_ctrl4_RBUS M8P_ppoverlay_double_buffer_ctrl4_reg;

    M8P_ppoverlay_double_buffer_ctrl4_reg.regValue = IoReg_Read32(M8P_PPOVERLAY_Double_Buffer_CTRL4_reg);

    if(enable == NULL)
    {
        return -1;
    }

    if(index == DOUBLE_BUFFER_D3)
    {
        M8P_ppoverlay_double_buffer_ctrl4_reg.dsubreg_dbuf_en_new = *enable;
    }

    IoReg_Write32(M8P_PPOVERLAY_Double_Buffer_CTRL4_reg, M8P_ppoverlay_double_buffer_ctrl4_reg.regValue);

    return 0;
}

int m8p_dtg_get_double_buffer_enable(DTG_DOUBLE_BUFFER_INDEX index, unsigned char *enable)
{
    M8P_ppoverlay_double_buffer_ctrl4_RBUS M8P_ppoverlay_double_buffer_ctrl4_reg;

    M8P_ppoverlay_double_buffer_ctrl4_reg.regValue = IoReg_Read32(M8P_PPOVERLAY_Double_Buffer_CTRL4_reg);

    if(enable == NULL)
    {
        return -1;
    }

    if(index == DOUBLE_BUFFER_D3)
    {
        *enable = M8P_ppoverlay_double_buffer_ctrl4_reg.dsubreg_dbuf_en_new;
    }

    return 0;
}

int m8p_dtg_set_double_buffer_apply(DTG_DOUBLE_BUFFER_INDEX index, unsigned char *apply)
{
    M8P_ppoverlay_double_buffer_ctrl4_RBUS M8P_ppoverlay_double_buffer_ctrl4_reg;

    M8P_ppoverlay_double_buffer_ctrl4_reg.regValue = IoReg_Read32(M8P_PPOVERLAY_Double_Buffer_CTRL4_reg);

    if(apply == NULL)
    {
        return -1;
    }

    if(index == DOUBLE_BUFFER_D3)
    {
        M8P_ppoverlay_double_buffer_ctrl4_reg.dsubreg_dbuf_set_new = *apply;
    }

    IoReg_Write32(M8P_PPOVERLAY_Double_Buffer_CTRL4_reg, M8P_ppoverlay_double_buffer_ctrl4_reg.regValue);

    return 0;
}

int m8p_dtg_get_double_buffer_apply(DTG_DOUBLE_BUFFER_INDEX index, unsigned char *apply)
{
    M8P_ppoverlay_double_buffer_ctrl4_RBUS M8P_ppoverlay_double_buffer_ctrl4_reg;

    M8P_ppoverlay_double_buffer_ctrl4_reg.regValue = IoReg_Read32(M8P_PPOVERLAY_Double_Buffer_CTRL4_reg);

    if(apply == NULL)
    {
        return -1;
    }

    if(index == DOUBLE_BUFFER_D3)
    {
        *apply = M8P_ppoverlay_double_buffer_ctrl4_reg.dsubreg_dbuf_set_new;
    }

    return 0;
}