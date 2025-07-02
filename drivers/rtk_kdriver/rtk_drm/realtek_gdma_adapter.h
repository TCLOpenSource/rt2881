#if !defined(__RTK_GDMA_ADAPTER_H__)
#define __RTK_GDMA_ADAPTER_H__

#include <linux/device.h>
#include <linux/types.h>
#include "rtk_drm_fourcc.h"
#include <rtk_kdriver/gal/rtk_gdma_export.h>
#include <rtk_kdriver/gal/rtk_gdma_export_user.h>

#define GDMA_MAX_HEIGHT 2160
#define GDMA_MAX_WIDTH 3840

#if defined(CONFIG_ARCH_RTK2885P)
#define ARM_AFBC
#else
#define IMG_TFBC
#endif

bool rtk_drm_gdma_clocks_set(struct device *dev,
			 u32 clock_freq, u32 dev_num,
			 u32 hdisplay, u32 vdisplay);

void rtk_drm_gdma_set_updates_enabled(struct device *dev, bool enable);

void rtk_drm_gdma_set_syncgen_enabled(struct device *dev, bool enable);

void rtk_drm_gdma_set_powerdwn_enabled(struct device *dev, bool enable);

void rtk_drm_gdma_set_plane_enabled(struct device *dev, u32 plane, bool enable);

void rtk_drm_gdma_reset_planes(struct device *dev );

GDMA_DISPLAY_PLANE rtk_drm_get_gdma_plane_index(int drm_plane_index);

void rtk_drm_gdma_set_surface_atomic(struct device *dev,
			u32 plane, u32 address, u32 offset,
			int32_t crtcx, int32_t crtcy, u32 crtcw, u32 crtch,
			u32 srcx, u32 srcy, u32 srcw, u32 srch, u32 mode_w, u32 mode_h, 
			u32 screen_w, u32 screen_h, int32_t hot_x, int32_t hot_y, u32 stride,
			u32 format, u64 modifier, u32 alpha, bool blend);

void rtk_drm_gdma_set_surface(struct device *dev, 
			  u32 plane, u32 address, u32 offset,
			  u32 posx, u32 posy,
			  u32 width, u32 height, u32 screen_w, u32 screen_h, u32 stride,
			  u32 format, u64 modifier, u32 alpha, bool blend);

void rtk_drm_gdma_mode_set(struct device *dev,
		       u32 h_display, u32 v_display,
		       u32 hbps, u32 ht, u32 has,
		       u32 hlbs, u32 hfps, u32 hrbs,
		       u32 vbps, u32 vt, u32 vas,
		       u32 vtbs, u32 vfps, u32 vbbs,
		       bool nhsync, bool nvsync);

void rtk_drm_gdma_atomic_begin(void);

void rtk_drm_gdma_atomic_flush(int plane_num);
void rtk_drm_get_screen_size(u32 * hdisplay, u32 * vdisplay);

#if 0//IS_ENABLED(CONFIG_SUPPORT_SCALER)
extern unsigned int Get_DISPLAY_REFRESH_RATE(void);
#endif

#endif /* __RTK_GDMA_ADAPTER_H__*/
