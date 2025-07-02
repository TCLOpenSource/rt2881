#include <rbus/sys_reg_reg.h>
#include <rbus/hdr_all_top_reg.h>
#include <rtk_kdriver/io.h>
#include <scaler/scalerCommon.h>
#include <tvscalercontrol/io/ioregdrv.h>

void hdr_crt_ctrl(bool enable)
{
    uint32_t write_data = enable ? _BIT0 : 0;

    IoReg_Write32(SYS_REG_SYS_CLKEN0_reg, SYS_REG_SYS_CLKEN0_clken_hdr1_mask | write_data);
    IoReg_Write32(SYS_REG_SYS_CLKEN1_reg, BIT(SYS_REG_SYS_CLKEN1_clken_hdr1_comp_shift) 
                | BIT(SYS_REG_SYS_CLKEN1_clken_hdr1_dm_shift)
                | write_data);
}

void hdr_all_top_top_ctl_set_vs_inv(unsigned int bflag)
{
    hdr_all_top_top_ctl_RBUS hdr_all_top_top_ctl_reg;

    hdr_all_top_top_ctl_reg.regValue = IoReg_Read32(HDR_ALL_TOP_TOP_CTL_reg);
    hdr_all_top_top_ctl_reg.vs_inv = bflag;
    IoReg_Write32(HDR_ALL_TOP_TOP_CTL_reg, hdr_all_top_top_ctl_reg.regValue); //HDMI dolby mode or HDR10 mode
}

void hdr_all_top_top_ctl_set_hdr1_2p(unsigned int bflag)
{

}
