#ifndef __QUICK_SHOW_H__
#define __QUICK_SHOW_H__
#include <base_types.h>
#ifndef BUILD_QUICK_SHOW
#include <linux/errno.h>
#else
#include <errno.h>
#endif

#include <quick_show/quick_show_ipc.h>
#include <scaler/scalerCommon.h>

#if IS_ENABLED(CONFIG_RTK_KDRV_QUICKSHOW)
bool is_QS_active(void);
bool is_QS_hdmi_enable(void);
bool is_QS_dp_enable(void);
bool is_QS_panel_enable(void);
bool is_QS_scaler_enable(void);
bool is_QS_i2c_enable(void);
bool is_QS_pq_enable(void);
bool is_QS_acpu_enable(void);
bool is_QS_vcpu_enable(void);
bool is_QS_amp_enable(void);
bool is_QS_hp_enable(void);
bool is_QS_typec_enable(void);
int get_QS_portnum(void);
bool is_QS_game_mode(void);
PC_MODE_INFO get_QS_pc_mode(void);
LOW_LATENCY_MODE_INFO get_QS_low_latency_mode(void);
bool is_QS_freesync_enable(void);
bool is_QS_gdma_enable(void);
bool is_QS_eco_mode(void);
void set_QS_inactive(void);
int get_QS_mute(void);
int get_QS_volume(void);
int get_QS_audio_balance(void);

extern bool in_quick_show;

#else

static inline bool is_QS_active(void) { return false; }
static inline bool is_QS_hdmi_enable(void) { return false; }
static inline bool is_QS_dp_enable(void) { return false; }
static inline bool is_QS_panel_enable(void) { return false; }
static inline bool is_QS_scaler_enable(void) { return false; }
static inline bool is_QS_i2c_enable(void) { return false; }
static inline bool is_QS_pq_enable(void) { return false; }
static inline bool is_QS_acpu_enable(void) { return false; }
static inline bool is_QS_vcpu_enable(void) { return false; }
static inline bool is_QS_amp_enable(void) { return false; }
static inline bool is_QS_hp_enable(void) { return false; }
static inline bool is_QS_typec_enable(void) { return false; }
static inline int get_QS_portnum(void) { return -ENODEV; }
static inline PC_MODE_INFO get_QS_pc_mode(void) { return PC_MODE_UNKNOWN; }
static inline LOW_LATENCY_MODE_INFO get_QS_low_latency_mode(void) { return LOW_LATENCY_MODE_DISABLE; }
static inline bool is_QS_game_mode(void) { return false; }
static inline bool is_QS_freesync_enable(void) { return false; }
static inline bool is_QS_gdma_enable(void) { return false; }
static inline bool is_QS_eco_mode(void) { return false; }
static inline int get_QS_mute(void) { return 0; }
static inline void set_QS_inactive(void) { }
static inline int get_QS_volume(void) { return 0; }
static inline int get_QS_audio_balance(void) { return 0; }
static bool in_quick_show = false;

#endif /* CONFIG_RTK_KDRV_QUICKSHOW */

#endif /*__QUICK_SHOW_H__ */
