#ifndef _HOST_SCALER_I2C_ONLY_H_
#define _HOST_SCALER_I2C_ONLY_H_

#if defined(CONFIG_H5CX_SUPPORT)
char vbe_rlink_set_dlg_mode(unsigned int dlgMode);
char vbe_rlink_set_vrr_info(unsigned int vrr);
char vbe_rlink_set_freesync_info(unsigned int freesync);
#endif


#endif
