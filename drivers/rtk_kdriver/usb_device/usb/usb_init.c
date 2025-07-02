#include "arch.h"
#include "mach.h"
#include "usb_mac.h"
#include "rtk_io.h"
#include "rtk_usb_device_dbg.h"

//PRAGMA_CODESEG(BANK0);
//PRAGMA_CONSTSEG(BANK0);

int usb_init(void)
{
    pm_printk(LOGGER_ERROR, "%s start\n", __func__);
    rtk_usb_driver_init();
    return 0;
}

