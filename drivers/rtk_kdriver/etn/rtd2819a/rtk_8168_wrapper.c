
static void rtl8168_phy_int_disable(struct rtl8168_private *tp)
{
		u32 data = 0;
		data = r8168_mac_ocp_read(tp, 0xE404);
		data = (data & ~BIT(11)) | BIT(12);
		r8168_mac_ocp_write(tp, 0xE404, data);

		data = r8168_mac_ocp_read(tp, 0xFC1E);
		data &= ~BIT(1);
		r8168_mac_ocp_write(tp, 0xFC1E, data);
}

static void rtl8168_phy_int_enable(struct rtl8168_private *tp)
{
		u32 data = 0;
		data = r8168_mac_ocp_read(tp, 0xE404);
		data |= BIT(12)|BIT(11);
		r8168_mac_ocp_write(tp, 0xE404, data);

		data = r8168_mac_ocp_read(tp, 0xFC1E);
		data |= BIT(1);
		r8168_mac_ocp_write(tp, 0xFC1E, data);
}

static void rtl8168_phy_eee_enable(struct rtl8168_private *tp, bool enable)
{
	if(enable) {
		/* EEE on */
		rtl_writephy(tp, 0x1f, 0x0a43);
		rtl_writephy(tp, 0x19, 0x0073);
		
	}
}

#ifdef ENABLE_EEEP
/*
 * Mac EEEPlus patch from CN
 */
static void rtl8168_mac_eeep_patch_disable(struct rtl8168_private *tp)
{
}

static void rtl8168_phy_eeep_enable(struct rtl8168_private *tp, bool enable)
{
	if(enable) {
		rtl_writephy(tp, 0x1f, 0x0a43);
		rtl_writephy(tp, 0x18, 0x2119);
		rtl_writephy(tp, 0x19, 0x0867);
		rtl_writephy(tp, 0x1f, 0x0a44);	
		rtl_writephy(tp, 0x11, 0x04BC);	
		rtl_writephy(tp, 0x11, 0x0CBC);	

		rtl_writephy(tp, 0x1f, 0x0000);
					
		rtl_writephy(tp, 0x00, 0x8000);
		mdelay(2);
		rtl_writephy(tp, 0x00, 0x0100);
	}
}

static void rtl8168_mac_eeep_enable(struct rtl8168_private *tp)
{
	rtl8168_phy_eeep_enable(tp, true);
}

static void rtl8168_mac_eeep_patch(struct rtl8168_private *tp)
{

}
#endif


static void rtl8168_enable_mac_phy_connect(struct rtl8168_private *tp)
{
	if(tp->output_mode ==	OUTPUT_RGMII_TO_PHY
		|| tp->output_mode == OUTPUT_RGMII_TO_MAC)
		r8168_mac_ocp_write(tp, 0xEA34, 0x04b5);
	else
		r8168_mac_ocp_write(tp, 0xEA34, 0x2);
}

static void rtl8168_pre_phy_init(struct rtl8168_private *tp)
{	
}

static void rtl8168_set_tx_tdfnr(struct rtl8168_private *tp)
{
}
