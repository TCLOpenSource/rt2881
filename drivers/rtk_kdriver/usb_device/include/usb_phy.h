#ifndef _USB_PHY_H_
#define _USB_PHY_H_

#include "arch.h"
#include "mach.h"
struct u2_phy_reg {
    UINT8 port1;
    UINT8 addr;
    UINT8 val;
};


struct u3_phy_reg {
    UINT8  page;
    UINT8  reg;
    UINT16 val;
};




//extern UINT16 dwc3_mdio_read(UINT8 phy_addr, UINT8 reg_addr);
extern void dwc3_mdio_write(UINT8 phy_addr, UINT8 reg_addr, UINT16 val);
extern inline void dwc3_wait_mdio_done(void);

extern UINT8 dwc3_usb2_load_phy(void);
extern UINT8 dwc3_usb3_load_phy(void);
//extern UINT8 dwc3_usb2_set_squelch_from_otp(void);


#endif
