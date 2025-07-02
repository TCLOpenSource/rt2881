#include <rbus/vodma_reg.h>
#include <tvscalercontrol/io/ioregdrv.h>


unsigned char drvif_scaler_vodma_get_2p_gate_sel(void)
{
#ifndef CONFIG_ARCH_RTK2875Q
	vodma_vodma_clkgen_RBUS vodma_clkgen_reg;

    vodma_clkgen_reg.regValue = IoReg_Read32(VODMA_VODMA_CLKGEN_reg);

    return vodma_clkgen_reg.vodma_2p_gate_sel;
#else
    return 1;
#endif
}
