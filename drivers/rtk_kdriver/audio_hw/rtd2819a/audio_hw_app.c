#ifndef BUILD_QUICK_SHOW
#include <linux/kconfig.h>
#include <linux/compiler.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/stat.h>			/* permission */
#include <linux/fs.h>			/* fs */
#include <linux/errno.h>		/* error codes */
#include <linux/types.h>		/* size_t */
#include <linux/fcntl.h>		/* O_ACCMODE */
#include <linux/uaccess.h>		/* copy_*_user */
#include <linux/semaphore.h>		/* semaphore */
#include <linux/kthread.h>		/* kernel thread */
#include <linux/freezer.h>		/* set freezable */
#include <linux/wait.h>			/* wait event */
#include <linux/jiffies.h>		/* jiffies */
#include <linux/delay.h>		/* msleep/mdelay */
#include <linux/string.h>		/* memcpy */
#else /* !BUILD_QUICK_SHOW */
#include <string.h>
#include <no_os/kernel.h>
#include <no_os/semaphore.h>
#include <no_os/kthread.h>
#include <no_os/export.h>
#include <audio_hw.h>
#endif /* !BUILD_QUICK_SHOW */

#include <rtk_kdriver/quick_show/quick_show.h>
/*
 * rbus registers
 */
#include <rbus/audio_reg.h>

#include "audio_hw_port.h"
#include "audio_hw_atv.h"
#include "audio_hw_aio.h"
#include "audio_hw_app.h"
#include "audio_hw_ioctl.h"

#define __ALOG_SUBTAG "app"

static DEFINE_SEMAPHORE(audio_sem_app);
static DEFINE_SEMAPHORE(audio_sem_eq);
static DEFINE_SEMAPHORE(audio_sem_loud);

#define APP_CH_NUM	6
#define CH_L		0
#define CH_R		1
#define CH_LS		2
#define CH_RS		3
#define CH_C		4
#define CH_SW		5

#if 0// protect to access 0x1800_x800 or 0x1800_x900 address for DC-bug
#define WAIT_HW_SEM()	{while(!rtd_inl(HD_SEM_reg));}
#define POST_HW_SEM()	{rtd_outl(HD_SEM_reg,0);}
#else
#define WAIT_HW_SEM()
#define POST_HW_SEM()
#endif

//#define FREQ_EQ_FEEDBACK
#if defined(FREQ_EQ_FEEDBACK)
#define Q824_ONE    (16777216)      // 0x01000000
#define Q214_ONE    (16384)         // 0x00004000
#define Q412_ONE    (4096)          // 0x00001000
#define SIGN_INT16  (0x8000)
#define SIGN_INT32  (0x80000000)
#define BAND_INFO_SIZE  2

#define FREQ_EQ_CALIBRATION
#endif

static int32_t m_volume_comp[APP_DVOL_ID_NUM][APP_CH_NUM];
static int32_t m_balance = 0;
static int32_t m_balance_lr = 0;
static int32_t m_balance_lsrs = 0;
static int32_t m_balance_csw = 0;
static uint32_t m_mute[APP_MUTE_NUM];
static int32_t m_volume_ts[2];
static int32_t m_balance_ts_lr = 0;
static uint32_t m_mute_ts[APP_MUTE_NUM];
#if 0
static uint32_t m_monitor = false;
#endif
static uint32_t m_app_inited = false;
 //remove merlin7 audiobb otp
/* OTP value for DAC_AIO combination
1. Reg 0xb80382b0[24:20] = DAC0-->AIO
2. Reg 0xb80382b0[19:15] = DAC0-->HP

3. Reg 0xb80382b0[9:5] = DAC1-->AIO
4. Reg 0xb80382b0[4:0] = DAC1-->HP
*/
#define AUDIO_OUT_LR_DIFFERENCE_OTP_VADDR	(0xb80382b0)
#define AUDIO_OUT_LR_OTP_MASK			(0x1F)
#define AUDIO_OUT_LR_OTP_VALUE_MASK		(0xF)

static int g_DAC0_AIO_OTP = 0;
static int g_DAC0_HP_OTP = 0;
static int g_DAC1_AIO_OTP = 0;
static int g_DAC1_HP_OTP = 0;

static int g_TSDVC_comp_gain = 0;
static int g_PBDVC_comp_gain = 0;
static int g_ENABLE_AUDIOOUTPUTCOMPENSATE = 0;
static bool g_EQLoudParaDirectChange = true;
static bool g_DVCParaDirectChange = FALSE; // googleTV default normal mode Immedicate change ryanliao 2017/03/14
static int g_AIO_path = 0; // 1: PB_path ; 2: TS_path ; 3: PB & TS path ; others: null
static int g_mute_id_pb = USR_MUTE_NUM;
static int g_mute_id_ts = USR_MUTE_NUM;
extern uint8_t g_runtime_state;
static uint32_t m_isNeed2Print = 1;

static APP_DVOL_CFG m_cfg_dvol = {
	0, // resvd1
	APP_DVOL_MODE_ZERO_AND_SOFT,    // mode
	0x1F,                           // zero_thd, 2^n level for 32bit sample
	APP_DVOL_ZCTO_1024x1,           // zero_timeout
	APP_DVOL_RAMP_STEP_1_DIV_8_DB,  // ramp_step
	0x40                            // delay_time
};

static APP_MUTE_CFG m_cfg_mute = {
	0,	// resvd1
	true,                           // on_off
	APP_DVOL_ZCTO_1024x2,           // zero_timeout
	APP_DVOL_RAMP_STEP_2_DIV_8_DB,  // ramp_step
	0x10                            // delay_time
};

static APP_DVOL_CFG m_cfg_dvol_ts = {
	0,                              // resvd1
	APP_DVOL_MODE_ZERO_AND_SOFT,    // mode
	0x1F,                           // zero_thd, 2^n level for 32bit sample
	APP_DVOL_ZCTO_1024x1,           // zero_timeout
	APP_DVOL_RAMP_STEP_1_DIV_8_DB,  // ramp_step
	0x40                            // delay_time
};

static APP_MUTE_CFG m_cfg_mute_ts = {
	0,                              // resvd1
	true,                           // on_off
	APP_DVOL_ZCTO_1024x2,           // zero_timeout
	APP_DVOL_RAMP_STEP_2_DIV_8_DB,  // ramp_step
	0x10                            // delay_time
};

// AVC1 function fine-tune parameters
static APP_AVC_CFG m_cfg_avc = {
	3,                              // avc_vol_detect max(|L|,|R|)--->volume detect
	APP_AVC_NOISE_GAIN_M6DB,        // noise_gain--->Noise gain
	APP_AVC_NOISE_THD_M60DB,        // noise_thd , here is -60dB--->Noise TH
	APP_AVC_SIGNAL_LVL_M36DB,       // level_max , here is -36dB (large volume suppressing Hi_th)--->HiTH
	APP_AVC_SIGNAL_LVL_M33DB,       // level_min , here is -42dB (large volume suppressing Lo_th)--->LoTH
	APP_AVC_STEP_GAIN_0P250DB,      // step_up--->Recover speed
	APP_AVC_STEP_GAIN_0P250DB,      // step_down--->Attack speed
	// identical to the order in register's field
	0x20,                           // thd_cnt , default after reset (unused)
	0x3FF,                          // win_zcross , default after reset (unused)
	0x40,                           // avc_mon_win (No need, please don't modify)--->Update Rate
	// identical to the order in register's field
	0,                              // avc_vol_amplif_enable (small volume amplification ON/OFF switch)
	APP_AVC_LPF_COEF_4,                             // avc1_coef--->Amp Coef
	APP_AVC_SIGNAL_LVL_M36DB,       // avc_small_level_max (small volume amplification Hi_th)--->Amp HiTH
	APP_AVC_SIGNAL_LVL_M33DB,       // avc_small_level_min (small volume amplification Lo_th)--->Amp LoTH
	APP_AVC_STEP_GAIN_0P250DB,      // avc_small_down_db--->Amp Recover speed
	APP_AVC_STEP_GAIN_0P250DB,      // avc_small_up_db--->Amp Attack speed
	0x20,                           // avc_small_cnt_thre (unused)
	// identical to the order in register's field
	0,                              // avc_lower_step_mag_thre
	0,                              // avc_amplif_step_mag_thre
	0x250,                          // avc_small_level (small volume amplification bypass_th)--->Bypass TH
	// identical to the order in register's field  <AVC_gain_step_control_0>
	0x10,                           //avc_lower_step_cnt_low_thre
	0x20,                           //avc_lower_step_cnt_hi_thre
	0x10,                           //avc_amplif_step_cnt_low_thre
	0x20,                           //avc_amplif_step_cnt_hi_thre
	// identical to the order in register's field  <AVC_gain_step_control_1>
	0,                              //reserved
	0x03,                           //avc_down_db_fine
	0x03,                           //avc_up_db_fine
	0x03,                           //avc_small_down_db_fine
	0x03                            //avc_small_up_db_fine
	// win_moniter , default after reset
};

// AVC2 function fine-tune parameters
static APP_AVC2_CFG m_cfg_avc2 = {
	0, // 	resvd1
	0, // avc2_vol_amplif_enable (small volume amplification ON/OFF switch)
	APP_AVC_LPF_COEF_4, //avc2_lpf_coef, 2^4--->Amp Coef
	3, //	avc2_vol_detect, max(|L|,|R|)--->avc2_vol_detect
	0, // fast_recov_ctrl
	APP_AVC_STEP_GAIN_0P250DB, //slow_up_db
	APP_AVC_STEP_GAIN_0P250DB, //fast_up_db--->Recover speed
	APP_AVC_STEP_GAIN_0P250DB, //down_db--->Attack speed
	APP_AVC_NOISE_GAIN_M12DB, //avc2_noise_gain--->Noise gain
	APP_AVC_NOISE_THD_M60DB, //avc2_noise_thre--->Noise TH

	// identical to the order in register's field
	0x20, // avc2_cnt_thre  (unused)
	0x3FF, // avc2_zc_win  (max)
	0x40, //	avc2_mon_win  (No need, please don't modify)--->Update Rate

	// identical to the order in register's field
	10, // avc2_noise_thre_hi
	20, // recov_win_max
	10, // recov_win_min

	// identical to the order in register's field
	1162, //200,//avc2_level_max (large volume suppressing Hi_th)--->HiTH
	923, //140//avc2_level_min (large volume suppressing Lo_th)--->LoTH

	// identical to the order in register's field
	0, // resvd3
	0x03,  //slow_up_db_fine
	0x03,  //fast_up_db_fine
	0x03,  //down_db_fine
	0x03,  //avc2_small_up_db_fine
	0x03,  //avc2_small_down_db_fine
	APP_AVC_STEP_GAIN_0P250DB, // avc2_small_up_db--->Amp Attack speed
	APP_AVC_STEP_GAIN_0P250DB, // avc2_small_down_db--->Amp Recover speed
	0x20, //	avc2_small_cnt_thre	 (unused)

	// identical to the order in register's field
	1162, //200,//avc2_small_level_max (small volume amplification Hi_th)--->Amp HiTH
	923, //140,//avc2_small_level_min (small volume amplification Lo_th)--->Amp LoTH

	// identical to the order in register's field
	0, // resvd4
	0x6000, //avc2_small_level (small volume amplification bypass_th)--->Amp Setting
};
#ifdef SUPPORT_TWO_BAND_DRC
// DRC function fine-tune parameters
static APP_DRC_CFG m_cfg_drc = {
	0, // 	resvd1
	0, // drc_vol_amplif_enable
	APP_AVC_LPF_COEF_6, //drc_lpf_coef, 2^6--->Amp Coef
	3, // drc_vol_detect, max(|L|,|R|)--->drc_vol_detect
	0, // drc_fast_recov_ctrl
	APP_AVC_STEP_GAIN_0P500DB, //drc_slow_up_db
	APP_AVC_STEP_GAIN_0P500DB, //drc_fast_up_db--->Recover speed
	APP_AVC_STEP_GAIN_0P500DB, //drc_down_db--->Attack speed
	APP_AVC_NOISE_GAIN_M12DB, //drc_noise_gain--->Noise gain
	APP_AVC_NOISE_THD_M60DB, //dr_noise_thre--->Noise TH

	// identical to the order in register's field
	0x20, // drc_cnt_thre  (unused)
	0x3ff, // drc_zc_win  (max)
	0x40, //	drc_mon_win  (No need, please don't modify)--->Update Rate

	// identical to the order in register's field
	10, // drc_noise_thre_hi
	20, // drc_recov_win_max
	10, // drc_recov_win_min

	// identical to the order in register's field
	200, //drc_level_max
	140, //drc_level_min

	// identical to the order in register's field
	0, // resvd3
	0x03,  //drc_slow_up_db_fine
	0x03,  //drc_fast_up_db_fine
	0x03,  //drc_down_db_fine
	0x03,  //drc_small_up_db_fine
	0x03,  //drc_small_down_db_fine
	APP_AVC_STEP_GAIN_0P500DB, // drc_small_up_db--->Amp Attack speed
	APP_AVC_STEP_GAIN_0P500DB, // drc_small_down_db--->Amp Recover speed
	0xf0, //	drc_small_cnt_thre	 (unused)

	// identical to the order in register's field
	200, //drc_small_level_max-->Amp HiTH
	140, //drc_small_level_min-->Amp LoTH

	// identical to the order in register's field
	0, // resvd4
	0x6000, //drc_small_level -->Amp Setting
};

// DRC_LPF function fine-tune parameters
static APP_DRC_LPF_CFG m_cfg_drc_lpf = {
	0, // 	resvd1
	0, // drc_lpf_vol_amplif_enable
	APP_AVC_LPF_COEF_6, //drc_lpf_coef, 2^6--->Amp Coef
	3, // drc_lpf_vol_detect, max(|L|,|R|)--->drc_vol_detect
	0, // drc_lpf_fast_recov_ctrl
	APP_AVC_STEP_GAIN_0P500DB, //drc_lpf_slow_up_db
	APP_AVC_STEP_GAIN_0P500DB, //drc_lpf_fast_up_db--->Recover speed
	APP_AVC_STEP_GAIN_0P500DB, //drc_lpf_down_db--->Attack speed
	APP_AVC_NOISE_GAIN_M12DB, //drc_lpf_noise_gain--->Noise gain
	APP_AVC_NOISE_THD_M60DB, //drc_lpf_noise_thre--->Noise TH

	// identical to the order in register's field
	0x20, // drc_lpf_cnt_thre  (unused)
	0x200, // drc_lpf_zc_win  (unused)
	0x40, //	drc_lpf_mon_win  (No need, please don't modify)--->Update Rate

	// identical to the order in register's field
	10, // drc_lpf_noise_thre_hi
	20, // drc_lpf_recov_win_max
	10, // drc_lpf_recov_win_min

	// identical to the order in register's field
	200, //drc_lpf_level_max
	140, //drc_lpf_level_min

	// identical to the order in register's field
	0, // resvd3
	0x03,  //drc_lpf_slow_up_db_fine
	0x03,  //drc_lpf_fast_up_db_fine
	0x03,  //drc_lpf_down_db_fine
	0x03,  //drc_lpf_small_up_db_fine
	0x03,  //drc_lpf_small_down_db_fine
	APP_AVC_STEP_GAIN_0P500DB, // drc_lpf_small_up_db--->Amp Attack speed
	APP_AVC_STEP_GAIN_0P500DB, // drc_lpf_small_down_db--->Amp Recover speed
	0x20, //	drc_lpf_small_cnt_thre	 (unused)

	// identical to the order in register's field
	200, //drc_lpf_small_level_max-->Amp HiTH
	140, //drc_lpf_small_level_min-->Amp LoTH

	// identical to the order in register's field
	0, // resvd4
	0x6000, //drc_lpf_small_level -->Amp Setting
};
#endif
static APP_DIST_CTRL_CFG m_cfg_dist = {
	3,      //dist_vol_detect = max(|L|,|R|)
	3,      //dist_coef = 2^6
	0x80,   //dist_high_thre = 0dB
	0x79,   //dist_low_thre = -0.5dB
	2,      //dist_down_db
	2,      //dist_up_db
	0X20,   //dist_cnt_thre
	0X3ff,  //dist_zc_win(max)
	0X40        //dist_mon_win
};

static APP_SE_CFG m_cfg_se = {
	// stereo setting
	0,              // resvd1
	1,              // apf_en
	0,              // psb_en
	0,              // seb_en
	0,              // sel_l
	1,              // sel_r
	0,              // resvd2
	APP_SE_M2S_L2,  // apf_coef
	0,              // resvd3
	APP_SE_SPL_L1,  // depth1
	0,              // resvd4
	APP_SE_SPL_L1   // depth2

#if 0
	// 1m setting
	0,              // resvd1
	1,              // apf_en
	0,              // psb_en
	0,              // seb_en
	0,              // sel_l
	1,              // sel_r
	0,              // resvd2
	APP_SE_M2S_L4,  // apf_coef
	0,              // resvd3
	APP_SE_SPL_L1,  // depth1
	0,              // resvd4
	APP_SE_SPL_L1   // depth2
#endif
#if 0
	// 2m setting
	0,              // resvd1
	1,              // apf_en
	0,              // psb_en
	1,              // seb_en
	0,              // sel_l
	1,              // sel_r
	0,              // resvd2
	APP_SE_M2S_L4,  // apf_coef
	0,              // resvd3
	APP_SE_SPL_L2,  // depth1
	0,              // resvd4
	APP_SE_SPL_L2   // depth2
#endif
#if 0
	// 4m setting
	0,              // resvd1
	1,              // apf_en
	0,              // psb_en
	1,              // seb_en
	0,              // sel_l
	1,              // sel_r
	0,              // resvd2
	APP_SE_M2S_L3,  // apf_coef
	0,              // resvd3
	APP_SE_SPL_L3,  // depth1
	0,              // resvd4
	APP_SE_SPL_L3   // depth2
#endif
};


static uint32_t m_capability[33] = {
	// 0,APP_ID_HWP_AVC
	APP_ID_HWP_AVC|APP_ID_HWP_TREBLE|APP_ID_HWP_BASS|APP_ID_HWP_EQ|APP_ID_HWP_SPK_COMP|APP_ID_HWP_SPL_EFFECT|
	APP_ID_SWP_SRS_TSXT|APP_ID_SWP_SRS_WOW|APP_ID_SWP_BBE_SOUND|APP_ID_SWP_RTK_RVB|APP_ID_SWP_RTK_SURROUND|APP_ID_SWP_MONITOR
	,
	// 1,APP_ID_HWP_TREBLE
	APP_ID_HWP_AVC|APP_ID_HWP_TREBLE|APP_ID_HWP_BASS|APP_ID_HWP_EQ|APP_ID_HWP_SPK_COMP|APP_ID_HWP_SPL_EFFECT|
	APP_ID_SWP_SRS_TSXT|APP_ID_SWP_SRS_WOW|APP_ID_SWP_BBE_SOUND|APP_ID_SWP_RTK_RVB|APP_ID_SWP_RTK_SURROUND|APP_ID_SWP_MONITOR
	,
	// 2,APP_ID_HWP_BASS
	APP_ID_HWP_AVC|APP_ID_HWP_TREBLE|APP_ID_HWP_BASS|APP_ID_HWP_EQ|APP_ID_HWP_SPK_COMP|APP_ID_HWP_SPL_EFFECT|
	APP_ID_SWP_SRS_TSXT|APP_ID_SWP_SRS_WOW|APP_ID_SWP_BBE_SOUND|APP_ID_SWP_RTK_RVB|APP_ID_SWP_RTK_SURROUND|APP_ID_SWP_MONITOR
	,
	// 3,APP_ID_HWP_EQ
	APP_ID_HWP_AVC|APP_ID_HWP_TREBLE|APP_ID_HWP_BASS|APP_ID_HWP_EQ|/*APP_ID_HWP_SPK_COMP |*/APP_ID_HWP_SPL_EFFECT|
	APP_ID_SWP_SRS_TSXT|APP_ID_SWP_SRS_WOW|APP_ID_SWP_BBE_SOUND|APP_ID_SWP_RTK_RVB|APP_ID_SWP_RTK_SURROUND|APP_ID_SWP_MONITOR
	,
	// 4,APP_ID_HWP_SPK_COMP
	APP_ID_HWP_AVC|APP_ID_HWP_TREBLE|APP_ID_HWP_BASS|/*APP_ID_HWP_EQ |*/APP_ID_HWP_SPK_COMP|APP_ID_HWP_SPL_EFFECT|
	APP_ID_SWP_SRS_TSXT|APP_ID_SWP_SRS_WOW|APP_ID_SWP_BBE_SOUND|APP_ID_SWP_RTK_RVB|APP_ID_SWP_RTK_SURROUND|APP_ID_SWP_MONITOR
	,
	// 5,APP_ID_HWP_SPL_EFFECT
	APP_ID_HWP_AVC|APP_ID_HWP_TREBLE|APP_ID_HWP_BASS|APP_ID_HWP_EQ|APP_ID_HWP_SPK_COMP|APP_ID_HWP_SPL_EFFECT|
	APP_ID_SWP_SRS_TSXT|APP_ID_SWP_SRS_WOW|APP_ID_SWP_BBE_SOUND|APP_ID_SWP_RTK_RVB|APP_ID_SWP_RTK_SURROUND|APP_ID_SWP_MONITOR
	,
	// 6 - 15
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	// 16,APP_ID_SWP_SRS_TSXT
	APP_ID_HWP_AVC|APP_ID_HWP_TREBLE|APP_ID_HWP_BASS|APP_ID_HWP_EQ|APP_ID_HWP_SPK_COMP|APP_ID_HWP_SPL_EFFECT|
	APP_ID_SWP_SRS_TSXT|APP_ID_SWP_MONITOR
	,
	// 17,APP_ID_SWP_SRS_WOW
	APP_ID_HWP_AVC|APP_ID_HWP_TREBLE|APP_ID_HWP_BASS|APP_ID_HWP_EQ|APP_ID_HWP_SPK_COMP|APP_ID_HWP_SPL_EFFECT|
	APP_ID_SWP_SRS_WOW|APP_ID_SWP_MONITOR
	,
	// 18,APP_ID_SWP_BBE_SOUND
	APP_ID_HWP_AVC|APP_ID_HWP_TREBLE|APP_ID_HWP_BASS|APP_ID_HWP_EQ|APP_ID_HWP_SPK_COMP|APP_ID_HWP_SPL_EFFECT|
	APP_ID_SWP_BBE_SOUND|APP_ID_SWP_MONITOR
	,
	// 19,APP_ID_SWP_RTK_RVB
	APP_ID_HWP_AVC|APP_ID_HWP_TREBLE|APP_ID_HWP_BASS|APP_ID_HWP_EQ|APP_ID_HWP_SPK_COMP|APP_ID_HWP_SPL_EFFECT|
	APP_ID_SWP_RTK_RVB|APP_ID_SWP_MONITOR
	,
	// 20,APP_ID_SWP_RTK_SURROUND
	APP_ID_HWP_AVC|APP_ID_HWP_TREBLE|APP_ID_HWP_BASS|APP_ID_HWP_EQ|APP_ID_HWP_SPK_COMP|APP_ID_HWP_SPL_EFFECT|
	APP_ID_SWP_RTK_SURROUND|APP_ID_SWP_MONITOR
	,
	// 21,APP_ID_SWP_MONITOR
	APP_ID_HWP_AVC|APP_ID_HWP_TREBLE|APP_ID_HWP_BASS|APP_ID_HWP_EQ|APP_ID_HWP_SPK_COMP|APP_ID_HWP_SPL_EFFECT|
	APP_ID_SWP_SRS_TSXT|APP_ID_SWP_SRS_WOW|APP_ID_SWP_BBE_SOUND|APP_ID_SWP_RTK_RVB|APP_ID_SWP_RTK_SURROUND|APP_ID_SWP_MONITOR
	,
	// 22 - 31
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	,
	// APP_ID_CAPABILITY
	APP_ID_HWP_AVC|APP_ID_HWP_TREBLE|APP_ID_HWP_BASS|APP_ID_HWP_EQ|APP_ID_HWP_SPK_COMP|APP_ID_HWP_SPL_EFFECT|
	APP_ID_SWP_SRS_TSXT|APP_ID_SWP_SRS_WOW|APP_ID_SWP_BBE_SOUND|APP_ID_SWP_RTK_RVB|APP_ID_SWP_RTK_SURROUND|APP_ID_SWP_MONITOR
};

//static AUDIO_EQ_INFO      k_freqEQ_info;

#define LOUD_ADJUST_STEP_SIZE	(10)	// too large gap will cause pop noise during changing parameters.

#define MAX_LOUD_VALUE			(APP_LOUD_COEF_NUM-1)
#define ZERO_LOUD_VALUE			(MAX_LOUD_VALUE/2)

#define MAX_EQ_VALUE			(APP_EQ_COEF_NUM-1)
#define ZERO_EQ_VALUE			(MAX_EQ_VALUE/2)
#define EQ_ADJUST_STEP_SIZE		(10)	// columbus use 200 ?!, but dragon use 20

#define PATH_ERROR	(0)
#define DAC0_AIO	(1)
#define DAC0_HP		(2)
#define DAC1_AIO	(3)
#define DAC1_HP		(4)

//#define K2L_BOARD
//#define K3L_BOARD

//EQ_Smooth_paraments
//#define USE_SMOOTHEQ_THREAD

#define t1 200//400//200u
#ifdef USE_SMOOTHEQ_THREAD
#define t2 5000//1000//500u
#else
#define t2 200
#endif
#define COMPARE 0
#define P2P 1
#define P2N 2
#define N2P 3
#define N2N 4
#define INCREASE 5
#define DECREASE 6
#define extINC 7
#define extDEC 8
#define SETNEWVAL 9
#define FINISH 10
#define GOZERO 11
#define SETNEWVAL2 12
#define GOTARGET 13

#define ZERO_POS 0x8000

#define eq_num 321


#ifdef USE_SMOOTHEQ_THREAD
static DECLARE_WAIT_QUEUE_HEAD(audio_app_wait);
#define APP_TICK_TIME_5MS		(5*1000)

#define APP_TICK_THREAD_TIME		(APP_TICK_TIME_5MS*5)	/* (ATV_TICK_TIME_10MS*2) */

APP_HWP_SET_EQ_PARAMETER_T d[16];
APP_HWP_SET_LOUD_PARAMETER_T k[4];
unsigned char g_HwpSetEQ_Smooth_H0_En[16]={0};
unsigned char g_HwpSetLoud_Smooth_H0_En[4]={0};
#endif


int32_t Audio_HwpSetEQ_Smooth_H0_Run(APP_EQ_ALL_BAND band_id, APP_EQ_COEF eq_coef);
int32_t Audio_HwpSetLoud_Smooth_H0_Run(APP_LOUD_ALL_BAND band_id, APP_EQ_COEF loud_coef);


#if 0
static uint32_t BPF_PARA[eq_num][3] = {//A1, A2, h0	
									{0x851f,0x3bf1,0x4800},// 20db
									{0x851f,0x3bf1,0x46db},
									{0x851f,0x3bf1,0x45ba},
									{0x851f,0x3bf1,0x449e},
									{0x851f,0x3bf1,0x4386},
									{0x851f,0x3bf1,0x4272},
									{0x851f,0x3bf1,0x4161},
									{0x851f,0x3bf1,0x4055},//7
									{0x851f,0x3bf1,0x3f4c},//19dB
									{0x851f,0x3bf1,0x3e48},
									{0x851f,0x3bf1,0x3d46},
									{0x851f,0x3bf1,0x3c49},
									{0x851f,0x3bf1,0x3b4f},
									{0x851f,0x3bf1,0x3a59},
									{0x851f,0x3bf1,0x3966},
									{0x851f,0x3bf1,0x3877},//15
									{0x851f,0x3bf1,0x378b},// 18dB 
									{0x851f,0x3bf1,0x36a3},
									{0x851f,0x3bf1,0x35be},
									{0x851f,0x3bf1,0x34dc},
									{0x851f,0x3bf1,0x33fd},
									{0x851f,0x3bf1,0x3322},
									{0x851f,0x3bf1,0x324a},
									{0x851f,0x3bf1,0x3174},//23
									{0x851f,0x3bf1,0x30a2},// 17dB
									{0x851f,0x3bf1,0x2fd3},
									{0x851f,0x3bf1,0x2f07},
									{0x851f,0x3bf1,0x2e3e},
									{0x851f,0x3bf1,0x2d77},
									{0x851f,0x3bf1,0x2cb4},
									{0x851f,0x3bf1,0x2bf3},
									{0x851f,0x3bf1,0x2b35},//31
									{0x851f,0x3bf1,0x2a7a},// 16dB
									{0x851f,0x3bf1,0x29c1},
									{0x851f,0x3bf1,0x290b},
									{0x851f,0x3bf1,0x2857},
									{0x851f,0x3bf1,0x27a7},
									{0x851f,0x3bf1,0x26f8},
									{0x851f,0x3bf1,0x264d},
									{0x851f,0x3bf1,0x25a3},//39
									{0x851f,0x3bf1,0x24fc},// 15dB
									{0x851f,0x3bf1,0x2458},
									{0x851f,0x3bf1,0x23b5},
									{0x851f,0x3bf1,0x2316},
									{0x851f,0x3bf1,0x2278},
									{0x851f,0x3bf1,0x21dd},
									{0x851f,0x3bf1,0x2144},
									{0x851f,0x3bf1,0x20ad},//47
									{0x851f,0x3bf1,0x2018},// 14dB
									{0x851f,0x3bf1,0x1f85},
									{0x851f,0x3bf1,0x1ef5},
									{0x851f,0x3bf1,0x1e66},
									{0x851f,0x3bf1,0x1dda},
									{0x851f,0x3bf1,0x1d4f},
									{0x851f,0x3bf1,0x1cc7},
									{0x851f,0x3bf1,0x1c40},//55
									{0x851f,0x3bf1,0x1bbc},// 13dB
									{0x851f,0x3bf1,0x1b39},
									{0x851f,0x3bf1,0x1ab8},
									{0x851f,0x3bf1,0x1a39},
									{0x851f,0x3bf1,0x19bc},
									{0x851f,0x3bf1,0x1940},
									{0x851f,0x3bf1,0x18c7},
									{0x851f,0x3bf1,0x184f},//63
									{0x851f,0x3bf1,0x17d9},// 12dB
									{0x851f,0x3bf1,0x1764},
									{0x851f,0x3bf1,0x16f1},
									{0x851f,0x3bf1,0x1680},
									{0x851f,0x3bf1,0x1611},
									{0x851f,0x3bf1,0x15a3},
									{0x851f,0x3bf1,0x1536},
									{0x851f,0x3bf1,0x14cb},//71
									{0x851f,0x3bf1,0x1462},// 11dB
									{0x851f,0x3bf1,0x13fa},
									{0x851f,0x3bf1,0x1394},
									{0x851f,0x3bf1,0x132f},
									{0x851f,0x3bf1,0x12cc},
									{0x851f,0x3bf1,0x126a},
									{0x851f,0x3bf1,0x1209},
									{0x851f,0x3bf1,0x11aa},//79
									{0x851f,0x3bf1,0x114c},// 10dB
									{0x851f,0x3bf1,0x10cb},
									{0x851f,0x3bf1,0x1094},
									{0x851f,0x3bf1,0x103a},
									{0x851f,0x3bf1,0x0fe2},
									{0x851f,0x3bf1,0x0f8a},
									{0x851f,0x3bf1,0x0f34},
									{0x851f,0x3bf1,0x0edf},//87
									{0x851f,0x3bf1,0x0e8c},// 9dB
									{0x851f,0x3bf1,0x0e39},
									{0x851f,0x3bf1,0x0de8},
									{0x851f,0x3bf1,0x0d98},
									{0x851f,0x3bf1,0x0d49},
									{0x851f,0x3bf1,0x0cfb},
									{0x851f,0x3bf1,0x0cae},
									{0x851f,0x3bf1,0x0c62},//95
									{0x851f,0x3bf1,0x0c18},// 8dB
									{0x851f,0x3bf1,0x0bce},	
									{0x851f,0x3bf1,0x0b86},
									{0x851f,0x3bf1,0x0b3e},
									{0x851f,0x3bf1,0x0af8},
									{0x851f,0x3bf1,0x0ab3},
									{0x851f,0x3bf1,0x0a6e},
									{0x851f,0x3bf1,0x0a2b},//103
									{0x851f,0x3bf1,0x09e8},// 7dB
									{0x851f,0x3bf1,0x09a7},
									{0x851f,0x3bf1,0x0966},
									{0x851f,0x3bf1,0x0927},
									{0x851f,0x3bf1,0x08e8},
									{0x851f,0x3bf1,0x08aa},
									{0x851f,0x3bf1,0x086d},
									{0x851f,0x3bf1,0x0831},//111
									{0x851f,0x3bf1,0x07f6},// 6dB
									{0x851f,0x3bf1,0x07bb},
									{0x851f,0x3bf1,0x0782},
									{0x851f,0x3bf1,0x0749},
									{0x851f,0x3bf1,0x0711},
									{0x851f,0x3bf1,0x06da},
									{0x851f,0x3bf1,0x06a4},
									{0x851f,0x3bf1,0x066e},//119
									{0x851f,0x3bf1,0x0639},// 5dB
									{0x851f,0x3bf1,0x0605},
									{0x851f,0x3bf1,0x05d2},
									{0x851f,0x3bf1,0x05a0},
									{0x851f,0x3bf1,0x056e},
									{0x851f,0x3bf1,0x053d},
									{0x851f,0x3bf1,0x050c},
									{0x851f,0x3bf1,0x04dc},//127
									{0x851f,0x3bf1,0x04ad},// 4dB
									{0x851f,0x3bf1,0x047f},
									{0x851f,0x3bf1,0x0451},
									{0x851f,0x3bf1,0x0424},
									{0x851f,0x3bf1,0x03f8},
									{0x851f,0x3bf1,0x03cc},
									{0x851f,0x3bf1,0x03a1},
									{0x851f,0x3bf1,0x0376},//135
									{0x851f,0x3bf1,0x034c},// 3dB								
									{0x851f,0x3bf1,0x0323},
									{0x851f,0x3bf1,0x02fa},
									{0x851f,0x3bf1,0x02d2},
									{0x851f,0x3bf1,0x02ab},
									{0x851f,0x3bf1,0x0284},
									{0x851f,0x3bf1,0x025d},
									{0x851f,0x3bf1,0x0237},//143
									{0x851f,0x3bf1,0x0212},// 2dB
									{0x851f,0x3bf1,0x01ed},
									{0x851f,0x3bf1,0x01c9},
									{0x851f,0x3bf1,0x01a5},
									{0x851f,0x3bf1,0x0182},
									{0x851f,0x3bf1,0x015f},
									{0x851f,0x3bf1,0x013c},
									{0x851f,0x3bf1,0x011b},//151
									{0x851f,0x3bf1,0x00f9},// 1dB
									{0x851f,0x3bf1,0x00d9},
									{0x851f,0x3bf1,0x00b8},
									{0x851f,0x3bf1,0x0098},
									{0x851f,0x3bf1,0x0079},
									{0x851f,0x3bf1,0x005a},
									{0x851f,0x3bf1,0x003b},
									{0x851f,0x3bf1,0x001d},//159
									{0x851f,0x3bf1,0x0000},// 0dB
									{0x852d,0x3be2,0xffe3},
									{0x853c,0x3bd3,0xffc6},
									{0x854b,0x3bc4,0xffaa},
									{0x855a,0x3bb5,0xff8e},	
									{0x8569,0x3ba6,0xff72},	
									{0x8578,0x3b96,0xff57},		
									{0x8588,0x3b87,0xff3c},//167		
									{0x8598,0x3b77,0xff22},// -1dB	
									{0x85a8,0x3b66,0xff08},			
									{0x85b8,0x3b56,0xfeee},				
									{0x85c9,0x3b45,0xfed5},				
									{0x85da,0x3b34,0xfebc},			
									{0x85ea,0x3b23,0xfea3},			
									{0x85fc,0x3b12,0xfe8b},			
									{0x860d,0x3b00,0xfe73},//175			
									{0x861f,0x3aee,0xfe5b},// -2dB		
									{0x8631,0x3adc,0xfe44},			
									{0x8643,0x3aca,0xfe2d},			
									{0x8655,0x3ab8,0xfe17},			
									{0x8668,0x3aa5,0xfe00},			
									{0x867b,0x3a92,0xfdea},			
									{0x868e,0x3a7f,0xfdd5},			
									{0x86a1,0x3a6b,0xfdbf},//183
									{0x86b5,0x3a57,0xfdaa},// -3dB
									{0x86c9,0x3a43,0xfd96},
									{0x86dd,0x3a2f,0xfd81},
									{0x86f1,0x3a1a,0xfd6d},
									{0x8706,0x3a05,0xfd59},
									{0x871b,0x39f0,0xfd46},
									{0x8730,0x39db,0xfd32},
									{0x8741,0x39ca,0xfd23},//191
									{0x875b,0x39af,0xfd0d},// -4dB
									{0x8771,0x3999,0xfcfa},
									{0x8788,0x3982,0xfce8},
									{0x879e,0x396c,0xfcd6},
									{0x87b5,0x3954,0xfcc4},
									{0x87cd,0x393d,0xfcb3},
									{0x87e4,0x3925,0xfca2},
									{0x87fc,0x390d,0xfc91},//199
									{0x8814,0x38f5,0xfc80},// -5dB
									{0x882d,0x38dc,0xfc70},
									{0x8845,0x38c3,0xfc5f},
									{0x885e,0x38aa,0xfc50},
									{0x8878,0x3890,0xfc40},
									{0x8892,0x3876,0xfc30},
									{0x88ac,0x385c,0xfc21},
									{0x88c6,0x3841,0xfc12},//207
									{0x88e1,0x3826,0xfc03},// -6dB
									{0x88fc,0x380b,0xfbf4},
									{0x8917,0x37ef,0xfbe6},
									{0x8933,0x37d3,0xfbd8},
									{0x894f,0x37b7,0xfbca},
									{0x896c,0x379a,0xfbbc},
									{0x8988,0x377d,0xfbae},
									{0x89a6,0x3760,0xfba1},//215
									{0x89c3,0x3742,0xfb93},// -7dB
									{0x89e1,0x3724,0xfb86},
									{0x89ff,0x3705,0xfb79},
									{0x8a1e,0x36e6,0xfb6d},
									{0x8a3d,0x36c7,0xfb60},
									{0x8a5d,0x36a7,0xfb54},
									{0x8a7c,0x3687,0xfb48},
									{0x8a9d,0x3667,0xfb3c},//223
									{0x8abd,0x3646,0xfb30},// -8dB
									{0x8ade,0x3624,0xfb24},
									{0x8b00,0x3603,0xfb19},
									{0x8b22,0x35e1,0xfb0d},
									{0x8b44,0x35be,0xfb02},
									{0x8b67,0x359b,0xfaf7},
									{0x8b8a,0x3578,0xfaec},
									{0x8bad,0x3554,0xfae2},//231
									{0x8bd1,0x3530,0xfad7},// -9dB
									{0x8bf5,0x350b,0xfacd},
									{0x8c1a,0x34e6,0xfac3},
									{0x8c40,0x34c0,0xfab8},
									{0x8c65,0x349a,0xfaaf},
									{0x8c8b,0x3474,0xfaa5},
									{0x8cb2,0x344d,0xfa9b},
									{0x8cd9,0x3425,0xfa92},//239
									{0x8d01,0x33fe,0xfa88},// -10dB
									{0x8d29,0x33d5,0xfa7f},
									{0x8d51,0x33ac,0xfa76},
									{0x8d7a,0x3383,0xfa6d},
									{0x8da3,0x3359,0xfa64},
									{0x8dcd,0x332f,0xfa5b},
									{0x8df8,0x3304,0xfa53},
									{0x8e22,0x32d9,0xfa4a},//247
									{0x8e4e,0x32ad,0xfa42},// -11dB
									{0x8e7a,0x3281,0xfa39},
									{0x8ea6,0x3255,0xfa31},
									{0x8ed3,0x3227,0xfa29},
									{0x8f00,0x31fa,0xfa21},
									{0x8f2e,0x31cb,0xfa1a},
									{0x8f5c,0x319c,0xfa12},
									{0x8f8b,0x316d,0xfa0a},//255
									{0x8fbb,0x313d,0xfa03},// -12dB
									{0x8feb,0x310d,0xf9fc},
									{0x901b,0x30dc,0xf9f4},
									{0x904c,0x30ab,0xf9ed},
									{0x907e,0x3079,0xf9e6},
									{0x90b0,0x3046,0xf9df},
									{0x90e2,0x3013,0xf9d8},
									{0x9116,0x2fdf,0xf9d2},//263
									{0x9149,0x2fab,0xf9cb},// -13dB
									{0x917e,0x2f76,0xf9c4},
									{0x91b3,0x2f41,0xf9be},
									{0x91e8,0x2f0b,0xf9b8},
									{0x921e,0x2ed5,0xf9b1},
									{0x9255,0x2e9e,0xf9ab},
									{0x928c,0x2e66,0xf9a5},
									{0x92c4,0x2e2e,0xf99f},//271
									{0x92fc,0x2df5,0xf999},// -14dB
									{0x9335,0x2dbc,0xf993},
									{0x936e,0x2d82,0xf98e},
									{0x93a8,0x2d47,0xf988},
									{0x93e3,0x2d0c,0xf982},
									{0x941e,0x2cd0,0xf97d},
									{0x945a,0x2c94,0xf977},
									{0x9497,0x2c57,0xf972},//279
									{0x94d4,0x2c19,0xf96d},// -15dB
									{0x9511,0x2bdb,0xf967},
									{0x9550,0x2b9c,0xf962},
									{0x958f,0x2b5d,0xf95d},
									{0x95ce,0x2b1c,0xf958},
									{0x960e,0x2adc,0xf953},
									{0x964f,0x2a9a,0xf94f},
									{0x9690,0x2a58,0xf94a},//287
									{0x96d3,0x2a16,0xf945},// -16dB
									{0x9715,0x29d3,0xf940},
									{0x9758,0x298f,0xf93c},
									{0x979c,0x294a,0xf937},
									{0x97e1,0x2905,0xf933},
									{0x9826,0x28bf,0xf92f},
									{0x986c,0x2879,0xf92a},
									{0x98b2,0x2832,0xf926},//295
									{0x98f9,0x27ea,0xf922},// -17dB
									{0x9941,0x27a2,0xf91e},
									{0x998a,0x2759,0xf91a},
									{0x99d3,0x270f,0xf916},
									{0x9a1c,0x26c5,0xf912},
									{0x9a67,0x267a,0xf90e},
									{0x9ab2,0x262e,0xf90a},
									{0x9afd,0x25e2,0xf906},//303
									{0x9b49,0x2595,0xf902},// -18dB
									{0x9b96,0x2548,0xf8ff},
									{0x9be4,0x24f9,0xf8fb},
									{0x9c32,0x24aa,0xf8f7},
									{0x9c81,0x245b,0xf8f4},
									{0x9cd0,0x250b,0xf8f0},
									{0x9d20,0x23ba,0xf8ed},
									{0x9d71,0x2368,0xf8ea},//311
									{0x9dc3,0x2316,0xf8e6},// -19dB
									{0x9e15,0x22c4,0xf8e3},
									{0x9e67,0x2270,0xf8e0},
									{0x9ebb,0x221c,0xf8dd},
									{0x9f0f,0x21c7,0xf8d9},
									{0x9f63,0x2172,0xf8d6},
									{0x9fb9,0x211c,0xf8d3},
									{0xa00f,0x20c5,0xf8d0},//319
									{0xa065,0x206e,0xf8cd}// -20dB
									};
#endif


#ifdef TV001_BOARD
#ifdef K2L_BOARD
static int32_t g_adc1_offset = 23; //+2.875dB
static int32_t g_adc2_offset = 23;
static int32_t g_dac0_offset = 0; //
static int32_t g_dac1_offset = 0;
#elif defined K3L_BOARD      
static int32_t g_adc1_offset = 4; //+0.5dB
static int32_t g_adc2_offset = 4;
static int32_t g_dac0_offset = 0; //
static int32_t g_dac1_offset = 0;
#elif defined K4L_BOARD      
static int32_t g_adc1_offset = 5; //+0.625dB
#if defined(SUPPORT_2_SET_ADC) && defined(SUPPORT_ADC_DVC)
static int32_t g_adc2_offset = 5;
#endif
static int32_t g_dac0_offset = 0; //
static int32_t g_dac1_offset = 0;
#elif defined K5L_BOARD      
static int32_t g_adc1_offset = 15; //+1.875dB
#if defined(SUPPORT_2_SET_ADC) && defined(SUPPORT_ADC_DVC)
static int32_t g_adc2_offset = 15;
#endif
static int32_t g_dac0_offset = 0; //
static int32_t g_dac1_offset = 0;
#elif defined K7L_BOARD      
static int32_t g_adc1_offset = 15; //+1.875dB
#if defined(SUPPORT_2_SET_ADC) && defined(SUPPORT_ADC_DVC)
static int32_t g_adc2_offset = 15;
#endif
static int32_t g_dac0_offset = 0; //
static int32_t g_dac1_offset = 0;
#else//2851c,2885p
static int32_t g_adc_dig_offset=0;//0.375db/step
static int32_t g_dac0_offset = 0;
static int32_t g_dac1_offset = 0;
#endif
#else
static int32_t g_adc1_offset = 15; //+1.875dB

static int32_t g_dac0_offset = 0;
static int32_t g_dac1_offset = 0;
#endif


static void HwpSetGain_ADC(uint32_t adc_ch_id, int32_t gain);
static void HwpSetGain_DAC0(uint32_t dac0_ch_id, int32_t gain);
static void HwpSetGain_DAC1(uint32_t dac1_ch_id, int32_t gain);
#ifdef SUPPORT_ADC_DVC
static void HwpEnableVolume_ADC1(bool enable);
static void HwpEnableVolume_ADC2(bool enable);
static void HwpSetVolume_ADC2(uint32_t adc2_ch_id, int32_t vol);
#endif
void HwpSetVolume(uint32_t ch_id, int32_t vol);
static void HwpSetVolumeTS(uint32_t ch_id, int32_t vol);
static void  CompensateAudioOutput_LRDifference(uint32_t isEnable);
static int32_t GetTSPath(void);
static int32_t GetPBPath(void);
static void ResetAudioOutputCompensate(void);
#ifndef TV001_BOARD
static void GetAudioOutputFromOTP(void);
#endif


#if defined(FREQ_EQ_FEEDBACK)
static int32_t HwpGetFreqEQ(AUDIO_EQ_INFO *p_eq_info);
static void HwpCalEQInfo(APP_EQ_COEF *k_eq_coef, int eq_loud_type);
#endif

#if 0
int32_t Audio_HwpSetBalanceLR(int32_t  balance)
{
	int32_t i;
	int32_t comp_vol;

	alog("[AUDIO][APP] set balance L/R : %d\n", balance);

	if (balance >= APP_MAX_BAL) {
		balance = APP_MAX_BAL;
		Audio_HwpSetMute(AIO_DISABLE, APP_CH_ID_CH1, BAL_MUTE_ID);
		Audio_HwpSetMute(AIO_ENABLE, APP_CH_ID_CH0, BAL_MUTE_ID);
	} else if (balance <= APP_MIN_BAL) {
		balance = APP_MIN_BAL;
		Audio_HwpSetMute(AIO_DISABLE, APP_CH_ID_CH0, BAL_MUTE_ID);
		Audio_HwpSetMute(AIO_ENABLE, APP_CH_ID_CH1, BAL_MUTE_ID);
	} else {
		Audio_HwpSetMute(AIO_DISABLE, APP_CH_ID_CH0|APP_CH_ID_CH1, BAL_MUTE_ID);
	}

	m_balance_lr = balance;

	if (m_balance_lr >= 0) { // attenuate L channel
		for (i = 0,comp_vol = 0; i < APP_DVOL_ID_NUM; i++) {
			comp_vol += m_volume_comp[i][CH_L];
		}

		HwpSetVolume(APP_CH_ID_CH0, comp_vol - m_balance_lr);

		for (i = 0,comp_vol = 0; i < APP_DVOL_ID_NUM; i++) {
			comp_vol += m_volume_comp[i][CH_R];
		}

		HwpSetVolume(APP_CH_ID_CH1, comp_vol);
	} else {                 // attenuate R channel
		for (i = 0,comp_vol = 0; i < APP_DVOL_ID_NUM; i++) {
			comp_vol += m_volume_comp[i][CH_R];
		}

		HwpSetVolume(APP_CH_ID_CH1, comp_vol + m_balance_lr);


		for (i = 0,comp_vol = 0; i < APP_DVOL_ID_NUM; i++) {
			comp_vol += m_volume_comp[i][CH_L];
		}

		HwpSetVolume(APP_CH_ID_CH0, comp_vol);

	}

	return AIO_OK;
}
#endif

int32_t Audio_HwpSetBalanceTS(int32_t  balance)
{
	alog("[AUDIO][APP] set ts_balance L/R : %d\n", balance);

	if (balance >= APP_MAX_BAL) {
		balance = APP_MAX_BAL;
		Audio_HwpSetMuteTS(AIO_ENABLE, APP_CH_ID_CH0, BAL_MUTE_ID);
	} else if (balance <= APP_MIN_BAL) {
		balance = APP_MIN_BAL;
		Audio_HwpSetMuteTS(AIO_ENABLE, APP_CH_ID_CH1, BAL_MUTE_ID);
	} else {
		Audio_HwpSetMuteTS(AIO_DISABLE, APP_CH_ID_CH0|APP_CH_ID_CH1, BAL_MUTE_ID);
	}

	m_balance_ts_lr = balance;

	if (m_balance_ts_lr > 0) {   // attenuate L channel
		HwpSetVolumeTS(APP_CH_ID_CH0, m_volume_ts[CH_L] - m_balance_ts_lr);
	} else {                 // attenuate R channel
		HwpSetVolumeTS(APP_CH_ID_CH1, m_volume_ts[CH_R] + m_balance_ts_lr);
	}

	return AIO_OK;
}

int32_t Audio_HwpSetBalance(int32_t  Balance, APP_BALANCE_CH_ID Channel)
{
	int32_t i;
	int32_t comp_vol;
	audio_lr_digital_volume_control_RBUS   DVOL_CH_LR_REG;
	audio_lsrs_digital_volume_control_RBUS DVOL_CH_LSRS_REG;
	//audio_csw_digital_volume_control_RBUS  DVOL_CH_CSW_REG;//mac9q dvc 4ch

	alog("[AUDIO][APP] set balance L/R : %d, ch=%d\n", Balance,Channel);


	DVOL_CH_LR_REG.regValue = AIO_ReadRegister(AUDIO_LR_Digital_Volume_Control_reg);
	DVOL_CH_LSRS_REG.regValue = AIO_ReadRegister(AUDIO_LsRs_Digital_Volume_Control_reg);	
	//DVOL_CH_CSW_REG.regValue = AIO_ReadRegister(AUDIO_CSW_Digital_Volume_Control_reg);	
	
	if((Channel==APP_BALANCE_CH_ID_CH01) && ((DVOL_CH_LR_REG.mute_l==1) || (DVOL_CH_LR_REG.mute_r==1)))
	{
		alog("Balance_CH01=%d,mute_l=%d,mute_r=%d\n",Balance,DVOL_CH_LR_REG.mute_l,DVOL_CH_LR_REG.mute_r);
	if (Balance >= APP_MAX_BAL) {
		Audio_HwpSetMute(AIO_DISABLE, APP_CH_ID_CH1, BAL_MUTE_ID);
		Audio_HwpSetMute(AIO_ENABLE, APP_CH_ID_CH0, BAL_MUTE_ID);
	} else if (Balance <= APP_MIN_BAL) {
		Audio_HwpSetMute(AIO_DISABLE, APP_CH_ID_CH0, BAL_MUTE_ID);
		Audio_HwpSetMute(AIO_ENABLE, APP_CH_ID_CH1, BAL_MUTE_ID);
	} else {
		Audio_HwpSetMute(AIO_DISABLE, APP_CH_ID_CH0|APP_CH_ID_CH1, BAL_MUTE_ID);
	}
	}else if((Channel==APP_BALANCE_CH_ID_CH23) && ((DVOL_CH_LSRS_REG.mute_ls==1) || (DVOL_CH_LSRS_REG.mute_rs==1)))
	{
		alog("Balance_CH23=%d,mute_ls=%d,mute_rs=%d\n",Balance,DVOL_CH_LSRS_REG.mute_ls,DVOL_CH_LSRS_REG.mute_rs);		
		if (Balance >= APP_MAX_BAL) {
			Audio_HwpSetMute(AIO_DISABLE, APP_CH_ID_CH3, BAL_MUTE_ID);
			Audio_HwpSetMute(AIO_ENABLE, APP_CH_ID_CH2, BAL_MUTE_ID);
		} else if (Balance <= APP_MIN_BAL) {
			Audio_HwpSetMute(AIO_DISABLE, APP_CH_ID_CH2, BAL_MUTE_ID);
			Audio_HwpSetMute(AIO_ENABLE, APP_CH_ID_CH3, BAL_MUTE_ID);
		} else {
			Audio_HwpSetMute(AIO_DISABLE, APP_CH_ID_CH2|APP_CH_ID_CH3, BAL_MUTE_ID);
		}	
	}
#if 0	//mac9q dvc 4ch
	else if((Channel==APP_BALANCE_CH_ID_CH45) && ((DVOL_CH_CSW_REG.mute_c==1) || (DVOL_CH_CSW_REG.mute_sw==1)))
	{
		alog("Balance_CH45=%d,mute_c=%d,mute_sw=%d\n",Balance,DVOL_CH_CSW_REG.mute_c,DVOL_CH_CSW_REG.mute_sw);			
		if (Balance >= APP_MAX_BAL) {
			Audio_HwpSetMute(AIO_DISABLE, APP_CH_ID_CH5, BAL_MUTE_ID);
			Audio_HwpSetMute(AIO_ENABLE, APP_CH_ID_CH4, BAL_MUTE_ID);
		} else if (Balance <= APP_MIN_BAL) {
			Audio_HwpSetMute(AIO_DISABLE, APP_CH_ID_CH4, BAL_MUTE_ID);
			Audio_HwpSetMute(AIO_ENABLE, APP_CH_ID_CH5, BAL_MUTE_ID);
		} else {
			Audio_HwpSetMute(AIO_DISABLE, APP_CH_ID_CH4|APP_CH_ID_CH5, BAL_MUTE_ID);
		}	
	}
#endif	
	if (Balance >= APP_MAX_BAL) {
		Balance = APP_MAX_BAL;
	} else if (Balance <= APP_MIN_BAL) {
		Balance = APP_MIN_BAL;
	}

	m_balance = Balance;

	if (m_balance >= 0) { // attenuate L channel
		for (i = 0,comp_vol = 0; i < APP_DVOL_ID_NUM; i++) {
			if (Channel == APP_BALANCE_CH_ID_CH01)
				comp_vol += m_volume_comp[i][CH_L];
			else if (Channel == APP_BALANCE_CH_ID_CH23)
				comp_vol += m_volume_comp[i][CH_LS];
			else if (Channel == APP_BALANCE_CH_ID_CH45)
				comp_vol += m_volume_comp[i][CH_C];
		}
		alog("Balance=%d,comp_vol=%d\n",m_balance,comp_vol);
		if(comp_vol==APP_MIN_BAL)
		{
			alog("Balance skip, comp_vol==APP_MIN_BAL\n");
			return AIO_OK;
		}
		if (Channel == APP_BALANCE_CH_ID_CH01)
		{
			HwpSetVolume(APP_CH_ID_CH0, comp_vol - m_balance);
			if (m_balance >= (-APP_MIN_BAL))
				Audio_HwpSetMuteInternal(AIO_ENABLE, APP_CH_ID_CH0, BAL_MUTE_ID);			
		}else if (Channel == APP_BALANCE_CH_ID_CH23)
		{
			HwpSetVolume(APP_CH_ID_CH2, comp_vol - m_balance);
			if (m_balance >= (-APP_MIN_BAL))
				Audio_HwpSetMuteInternal(AIO_ENABLE, APP_CH_ID_CH2, BAL_MUTE_ID);				
		}else if (Channel == APP_BALANCE_CH_ID_CH45)
		{
			HwpSetVolume(APP_CH_ID_CH4, comp_vol - m_balance);
			if (m_balance >= (-APP_MIN_BAL))
				Audio_HwpSetMuteInternal(AIO_ENABLE, APP_CH_ID_CH4, BAL_MUTE_ID);			
		}

		for (i = 0,comp_vol = 0; i < APP_DVOL_ID_NUM; i++) {
			if (Channel == APP_BALANCE_CH_ID_CH01)
				comp_vol += m_volume_comp[i][CH_R];
			else if (Channel == APP_BALANCE_CH_ID_CH23)
				comp_vol += m_volume_comp[i][CH_RS];
			else if (Channel == APP_BALANCE_CH_ID_CH45)
				comp_vol += m_volume_comp[i][CH_SW];
		}

		if (Channel == APP_BALANCE_CH_ID_CH01)
			HwpSetVolume(APP_CH_ID_CH1, comp_vol);
		else if (Channel == APP_BALANCE_CH_ID_CH23)
			HwpSetVolume(APP_CH_ID_CH3, comp_vol);
		else if (Channel == APP_BALANCE_CH_ID_CH45)
			HwpSetVolume(APP_CH_ID_CH5, comp_vol);

	} else {                 // attenuate R channel
		for (i = 0,comp_vol = 0; i < APP_DVOL_ID_NUM; i++) {
			if (Channel == APP_BALANCE_CH_ID_CH01)
				comp_vol += m_volume_comp[i][CH_R];
			else if (Channel == APP_BALANCE_CH_ID_CH23)
				comp_vol += m_volume_comp[i][CH_RS];
			else if (Channel == APP_BALANCE_CH_ID_CH45)
				comp_vol += m_volume_comp[i][CH_SW];
		}
		alog("Balance=%d,comp_vol=%d\n",m_balance,comp_vol);		
		if(comp_vol==APP_MIN_BAL)
		{
			alog("Balance skip, comp_vol==APP_MIN_BAL\n");
			return AIO_OK;
		}
		if (Channel == APP_BALANCE_CH_ID_CH01)
		{
			HwpSetVolume(APP_CH_ID_CH1, comp_vol + m_balance);
			if (m_balance <= APP_MIN_BAL)
				Audio_HwpSetMuteInternal(AIO_ENABLE, APP_CH_ID_CH1, BAL_MUTE_ID);
		}else if (Channel == APP_BALANCE_CH_ID_CH23)
		{
			HwpSetVolume(APP_CH_ID_CH3, comp_vol + m_balance);
			if (m_balance <= APP_MIN_BAL)
				Audio_HwpSetMuteInternal(AIO_ENABLE, APP_CH_ID_CH3, BAL_MUTE_ID);			
		}else if (Channel == APP_BALANCE_CH_ID_CH45)
		{
			HwpSetVolume(APP_CH_ID_CH5, comp_vol + m_balance);
			if (m_balance <= APP_MIN_BAL)
				Audio_HwpSetMuteInternal(AIO_ENABLE, APP_CH_ID_CH5, BAL_MUTE_ID);				
		}

		for (i = 0,comp_vol = 0; i < APP_DVOL_ID_NUM; i++) {
			if (Channel == APP_BALANCE_CH_ID_CH01)
				comp_vol += m_volume_comp[i][CH_L];
			else if (Channel == APP_BALANCE_CH_ID_CH23)
				comp_vol += m_volume_comp[i][CH_LS];
			else if (Channel == APP_BALANCE_CH_ID_CH45)
				comp_vol += m_volume_comp[i][CH_C];
		}

		if (Channel == APP_BALANCE_CH_ID_CH01)
			HwpSetVolume(APP_CH_ID_CH0, comp_vol);
		else if (Channel == APP_BALANCE_CH_ID_CH23)
			HwpSetVolume(APP_CH_ID_CH2, comp_vol);
		else if (Channel == APP_BALANCE_CH_ID_CH45)
			HwpSetVolume(APP_CH_ID_CH4, comp_vol);

	}

	return AIO_OK;
}

void Audio_HwpPrescale(int vol, int ch_id)
{
#if 0//mac9q remove register
	int32_t a;
	int32_t b = 0;
	//APP_CH_ID ch_id = (APP_CH_ID)(APP_CH_ID_CH0|APP_CH_ID_CH1);

	audio_hw_in_pre_scale_RBUS hw_in_pre_scale;

	if (vol > APP_MAX_VOL)
		vol = APP_MAX_VOL;

	if (vol < APP_MIN_VOL)
		vol = APP_MIN_VOL;

	// change dvol setting to DVOL mode
	hw_in_pre_scale.regValue = AIO_ReadRegister(AUDIO_hw_in_pre_scale_reg);

	a = vol / 6;
	b = vol % 6;
	if (b < 0) {
		a--;
		b += 6;
	}

	if (ch_id&APP_CH_ID_CH0 || ch_id&APP_CH_ID_CH1) {
		hw_in_pre_scale.ch01_scale_a = a;
		hw_in_pre_scale.ch01_scale_b = b;
	}
	#ifdef SUPPORT_6_CHANNEL_PRESCALE
	if (ch_id&APP_CH_ID_CH2 || ch_id&APP_CH_ID_CH3) {
		hw_in_pre_scale.ch23_scale_a = a;
		hw_in_pre_scale.ch23_scale_b = b;
	}

	if (ch_id&APP_CH_ID_CH4 || ch_id&APP_CH_ID_CH5) {
		hw_in_pre_scale.ch45_scale_a = a;
		hw_in_pre_scale.ch45_scale_b = b;
	}
	#endif

	AIO_WriteRegister(AUDIO_hw_in_pre_scale_reg,	hw_in_pre_scale.regValue);
#endif	
	return;

}
EXPORT_SYMBOL(Audio_HwpPrescale);

int32_t Audio_HwpSetVolume(uint32_t ch_id, int32_t vol)
{
	// testing
	/*
	    int a;
	    a = AIO_ReadRegister(0xb8006718);
	    if(a == 1)
	    {
	    if(ch_id& APP_CH_ID_CH0)
	    {
	    ch_id = ch_id & (~APP_CH_ID_CH0);
	    ch_id |= APP_CH_ID_CH4;
	    }
	
	    if(ch_id& APP_CH_ID_CH1)
	    {
	    ch_id = ch_id & (~APP_CH_ID_CH1);
	    ch_id |= APP_CH_ID_CH5;
	    }
	    }
	*/
	// ryanliao 2015/2/24
	// Add Audio output compensate
	// [start]
	//alog("[Audio][Debug]PB: g_ENABLE_AUDIOOUTPUTCOMPENSATE= %x \n", g_ENABLE_AUDIOOUTPUTCOMPENSATE);
	if (g_ENABLE_AUDIOOUTPUTCOMPENSATE == 1)
		CompensateAudioOutput_LRDifference(1);
	else
		CompensateAudioOutput_LRDifference(0);

	// [end]

	Audio_HwpSetVolumeCompensation(ch_id, vol, APP_DVOL_ID_USR);

	return AIO_OK;
}
EXPORT_SYMBOL(Audio_HwpSetVolume);

int32_t Audio_HwpSetVolumeCompensation(uint32_t ch_id, int32_t vol, uint32_t comp_id)
{
	int32_t i;
	int32_t comp_vol;

	if (!m_app_inited) {
		alog("[AUDIO][APP] library not ready, ignore %s()\n", __FUNCTION__);
		return AIO_ERR_CODE_APP;
	}

	down(&audio_sem_app);

	if (comp_id != APP_DVOL_ID_AIO)
		alog("3[AUDIO][APP]1 set volume_comp[%d] ch=%d,vol=%d\n", comp_id, ch_id, vol);

	if (comp_id >= APP_DVOL_ID_NUM) {
		up(&audio_sem_app);
		return AIO_ERR_APP_INVALID_VOL;
	}

	if (vol > APP_MAX_VOL)
		vol = APP_MAX_VOL;

	if (vol < APP_MIN_VOL)
		vol = APP_MIN_VOL;

	// if user choose the min_vol, library will mute the output
	if (APP_DVOL_ID_USR == comp_id) {
		alog("[APP][%s] %d\n", __FUNCTION__, __LINE__);
		if (vol == APP_MIN_VOL) {
			Audio_HwpSetMuteInternal(AIO_ENABLE, ch_id, VOL_MUTE_ID);
		}
	}


	if (ch_id&APP_CH_ID_CH0) {
		m_volume_comp[comp_id][CH_L] = vol;

		for (i = 0,comp_vol = 0; i < APP_DVOL_ID_NUM; i++) {
			comp_vol += m_volume_comp[i][CH_L];
		}

//		if (vol != APP_MIN_VOL) {
			if (m_balance > 0)    // attenuate L channel
				HwpSetVolume(APP_CH_ID_CH0, comp_vol - m_balance);
			else
				HwpSetVolume(APP_CH_ID_CH0, comp_vol);
//		}
	}

	if (ch_id&APP_CH_ID_CH1) {
		m_volume_comp[comp_id][CH_R] = vol;

		for (i = 0,comp_vol = 0; i < APP_DVOL_ID_NUM; i++) {
			comp_vol += m_volume_comp[i][CH_R];
		}

//		if (vol != APP_MIN_VOL) {
			if (m_balance < 0)    // attenuate R channel
				HwpSetVolume(APP_CH_ID_CH1, comp_vol + m_balance);
			else
				HwpSetVolume(APP_CH_ID_CH1, comp_vol);
//		}
	}

	if (ch_id&APP_CH_ID_CH2) {
		m_volume_comp[comp_id][CH_LS] = vol;

		for (i = 0,comp_vol = 0; i < APP_DVOL_ID_NUM; i++) {
			comp_vol += m_volume_comp[i][CH_LS];
		}
		#if 0
		HwpSetVolume(APP_CH_ID_CH2, comp_vol);
		#else
//		if (vol != APP_MIN_VOL)
//		{
			if (m_balance > 0)    // attenuate R channel
				HwpSetVolume(APP_CH_ID_CH2, comp_vol - m_balance);
			else
				HwpSetVolume(APP_CH_ID_CH2, comp_vol);
//		}
		#endif
	}

	if (ch_id&APP_CH_ID_CH3) {
		m_volume_comp[comp_id][CH_RS] = vol;

		for (i = 0,comp_vol = 0; i < APP_DVOL_ID_NUM; i++) {
			comp_vol += m_volume_comp[i][CH_RS];
		}
		#if 0
		HwpSetVolume(APP_CH_ID_CH3, comp_vol);
		#else
//		if (vol != APP_MIN_VOL)
//		{
			if (m_balance < 0)    // attenuate R channel
				HwpSetVolume(APP_CH_ID_CH3, comp_vol + m_balance);
			else
				HwpSetVolume(APP_CH_ID_CH3, comp_vol);
//		}
		#endif
	}

	if (ch_id&APP_CH_ID_CH4) {
		m_volume_comp[comp_id][CH_C] = vol;

		for (i = 0,comp_vol = 0; i < APP_DVOL_ID_NUM; i++) {
			comp_vol += m_volume_comp[i][CH_C];
		}
		#if 0
		HwpSetVolume(APP_CH_ID_CH4, comp_vol);
		#else
//		if (vol != APP_MIN_VOL)
//		{
			if (m_balance > 0)    // attenuate R channel
				HwpSetVolume(APP_CH_ID_CH4, comp_vol - m_balance);
			else
				HwpSetVolume(APP_CH_ID_CH4, comp_vol);
//		}
		#endif
	}

	if (ch_id&APP_CH_ID_CH5) {
		m_volume_comp[comp_id][CH_SW] = vol;

		for (i = 0,comp_vol = 0; i < APP_DVOL_ID_NUM; i++) {
			comp_vol += m_volume_comp[i][CH_SW];
		}
		#if 0
		HwpSetVolume(APP_CH_ID_CH5, comp_vol);
		#else
//		if (vol != APP_MIN_VOL)
//		{
			if (m_balance < 0)    // attenuate R channel
				HwpSetVolume(APP_CH_ID_CH5, comp_vol + m_balance);
			else
				HwpSetVolume(APP_CH_ID_CH5, comp_vol);
//		}
		#endif
	}

	// if user choose the min_vol, library will mute the output
	if ((APP_DVOL_ID_USR == comp_id) && (vol != APP_MIN_VOL)) {
//		alog("[APP][%s] %d\n", __FUNCTION__, __LINE__);
		if (m_mute[VOL_MUTE_ID] & ch_id) {
			Audio_HwpSetMuteInternal(AIO_DISABLE, ch_id, VOL_MUTE_ID);
		}
	}

	up(&audio_sem_app);

	return AIO_OK;
}


#if IS_ENABLED(CONFIG_RTK_KDRV_QUICKSHOW)
void Audio_Hw_sync_volume_from_qs(uint32_t ch_id, int32_t vol)
{
	uint32_t comp_id = APP_DVOL_ID_USR;

	down(&audio_sem_app);

	if (vol > APP_MAX_VOL)
		vol = APP_MAX_VOL;

	if (vol < APP_MIN_VOL)
		vol = APP_MIN_VOL;

	if (ch_id&APP_CH_ID_CH0) {
		m_volume_comp[comp_id][CH_L] = vol;
	}

	if (ch_id&APP_CH_ID_CH1) {
		m_volume_comp[comp_id][CH_R] = vol;
	}

	if (ch_id&APP_CH_ID_CH2) {
		m_volume_comp[comp_id][CH_LS] = vol;
	}

	if (ch_id&APP_CH_ID_CH3) {
		m_volume_comp[comp_id][CH_RS] = vol;
	}

	if (ch_id&APP_CH_ID_CH4) {
		m_volume_comp[comp_id][CH_C] = vol;
	}

	if (ch_id&APP_CH_ID_CH5) {
		m_volume_comp[comp_id][CH_SW] = vol;
	}

	up(&audio_sem_app);
}
EXPORT_SYMBOL(Audio_Hw_sync_volume_from_qs);


void Audio_Hw_sync_balance_from_qs(int32_t balance)
{
	m_balance = balance;
}
EXPORT_SYMBOL(Audio_Hw_sync_balance_from_qs);
#endif /* CONFIG_RTK_KDRV_QUICKSHOW */


int32_t Audio_HwpSetVolumeTS(uint32_t ch_id, int32_t vol)
{
	// ryanliao 2015/2/24
	// Add Audio output compensate
	// [start]
	//alog("[Audio][Debug]TS: g_ENABLE_AUDIOOUTPUTCOMPENSATE= %x \n", g_ENABLE_AUDIOOUTPUTCOMPENSATE);
	if (g_ENABLE_AUDIOOUTPUTCOMPENSATE == 1)
		CompensateAudioOutput_LRDifference(1);
	else
		CompensateAudioOutput_LRDifference(0);

	// [end]
	alog("[AUDIO][APP] set ts_volume ch=%d,vol=%d\n", ch_id, vol);

	if (vol > APP_MAX_VOL)
		vol = APP_MAX_VOL;

	if (vol < APP_MIN_VOL)
		vol = APP_MIN_VOL;

	if (ch_id&APP_CH_ID_CH0) {
		m_volume_ts[CH_L] = vol;

		if (vol != APP_MIN_VOL) {
			if (m_balance_ts_lr > 0) // attenuate L channel
				HwpSetVolumeTS(APP_CH_ID_CH0, vol - m_balance_ts_lr);
			else
				HwpSetVolumeTS(APP_CH_ID_CH0, vol);
		}
	}

	if (ch_id&APP_CH_ID_CH1) {
		m_volume_ts[CH_R] = vol;

		if (vol != APP_MIN_VOL) {
			if (m_balance_ts_lr < 0) // attenuate R channel
				HwpSetVolumeTS(APP_CH_ID_CH1, vol + m_balance_ts_lr);
			else
				HwpSetVolumeTS(APP_CH_ID_CH1, vol);
		}
	}

	// if user choose the min_vol, library will mute the output
	//	if(vol == APP_MIN_VOL) {
	//		Audio_HwpSetMuteTS(AIO_ENABLE, ch_id, VOL_MUTE_ID);
	//	}else if(m_mute_ts[VOL_MUTE_ID]) {
	//		Audio_HwpSetMuteTS(AIO_DISABLE, ch_id, VOL_MUTE_ID);
	//	}
	return AIO_OK;
}

int32_t Audio_HwpSetMute(bool para, uint32_t ch_id, uint32_t mute_id)
{
	int32_t ret;

	if (!m_app_inited) {
		alog("[AUDIO][APP] library not ready, ignore %s()\n", __FUNCTION__);
		return AIO_ERR_CODE_APP;
	}

	/*
	    int a;
	    a = AIO_ReadRegister(0xb8006718);
	    if(a == 1)
	    {
	    if(ch_id& APP_CH_ID_CH0)
	    {
	    ch_id = ch_id & (~APP_CH_ID_CH0);
	    ch_id |= APP_CH_ID_CH4;
	    }
	
	    if(ch_id& APP_CH_ID_CH1)
	    {
	    ch_id = ch_id & (~APP_CH_ID_CH1);
	    ch_id |= APP_CH_ID_CH5;
	    }
	    }
	*/

	//APP_PROFILE_START
#ifdef NO_USE_HW_AQ
if(g_user_config == AUDIO_USER_CONFIG_TV001){
	alog("No HW Mute para %d ch_id %d mute_id %d\n", para, ch_id, mute_id);
	return 0;
}
#endif
	down(&audio_sem_app);
	ret = Audio_HwpSetMuteInternal(para, ch_id, mute_id);
	/* //remove LGE dummy code(already return 0)
	if(para == true)
	{
	g_mute_id_pb = mute_id;
	alog("PB mute ID %d\n",g_mute_id_pb);
	}
	else
	{
	g_mute_id_pb = USR_MUTE_NUM;
	}
	*/
	up(&audio_sem_app);
	//APP_PROFILE_END

	return ret;
}

int32_t Audio_GetMuteID_PB(void)
{
	return g_mute_id_pb;
}

int32_t Audio_HwpSetMuteTS(bool para, uint32_t ch_id, uint32_t mute_id)
{
	int32_t ret;

	if (!m_app_inited) {
		alog("[AUDIO][APP] library not ready, ignore %s()\n", __FUNCTION__);
		return AIO_ERR_CODE_APP;
	}

#ifdef NO_USE_HW_AQ
if(g_user_config == AUDIO_USER_CONFIG_TV001){
	alog("No HW MuteTS para %d ch_id %d mute_id %d\n", para, ch_id, mute_id);
	return 0;
}
#endif
	down(&audio_sem_app);
	ret = Audio_HwpSetMuteInternalTS(para, ch_id, mute_id);
	/* //remove LGE dummy code(already return 0)
	g_mute_id_ts = mute_id;
	if(para == true)
	{
	g_mute_id_ts = mute_id;
	alog("PB mute ID %d\n",g_mute_id_pb);
	}
	else
	{
	g_mute_id_ts = USR_MUTE_NUM;
	}
	*/
	up(&audio_sem_app);

	return ret;
}

int32_t Audio_GetMuteID_TS(void)
{
	return g_mute_id_ts;
}

int32_t Audio_HwpGetMuteStatus(uint32_t ch_id, uint32_t *p_status)
{
	uint32_t i,
		 mask;

	for (i = 0,mask = 0; i < APP_MUTE_NUM; i++) {
		mask |= (m_mute[i]&ch_id);
	}

	*p_status = mask;

	return AIO_OK;
}

int32_t Audio_HwpGetMuteStatusTS(uint32_t ch_id, uint32_t *p_status)
{
	uint32_t i,
		 mask;

	for (i = 0,mask = 0; i < APP_MUTE_NUM; i++) {
		mask |= (m_mute_ts[i]&ch_id);
	}

	*p_status = mask;

	return AIO_OK;
}

int32_t Audio_HwpCfgVolume(APP_DVOL_CFG *p_cfg_dvol)
{
	audio_digital_volume_control_0_RBUS DVOL_0_CTRL_REG;
	audio_digital_volume_control_1_RBUS DVOL_1_CTRL_REG;
	
	down(&audio_sem_app);
	DVOL_1_CTRL_REG.regValue	= Audio_HwpReadRegister(AUDIO_Digital_Volume_Control_1_reg);
	DVOL_1_CTRL_REG.zcto		= p_cfg_dvol->zero_timeout;
	DVOL_1_CTRL_REG.ramp_step	= p_cfg_dvol->ramp_step;
	DVOL_1_CTRL_REG.delay_ctrl	= p_cfg_dvol->delay_time;
	Audio_HwpWriteRegister(AUDIO_Digital_Volume_Control_1_reg, DVOL_1_CTRL_REG.regValue);

	DVOL_0_CTRL_REG.regValue	= AIO_ReadRegister(AUDIO_Digital_Volume_Control_0_reg);
	DVOL_0_CTRL_REG.zcth		= p_cfg_dvol->zero_thd;
	//DVOL_0_CTRL_REG.dvcm		= p_cfg_dvol->mode;

	// for googleTV init flow ryanliao 2017/03/14
	if (g_DVCParaDirectChange == FALSE){
		DVOL_0_CTRL_REG.dvcm = p_cfg_dvol->mode; //normal mode change
		alog_debug("[AUDIO][APP] DVCPara Change Mode_0 = %x\n", g_DVCParaDirectChange);
  	}
	else{
		DVOL_0_CTRL_REG.dvcm = APP_DVOL_MODE_NORMAL; //direct mode Immedicate change
		alog_debug("[AUDIO][APP] DVCPara Change Mode_1 = %x\n", g_DVCParaDirectChange);
  	}

	AIO_WriteRegister(AUDIO_Digital_Volume_Control_0_reg, DVOL_0_CTRL_REG.regValue);

	up(&audio_sem_app);

	return AIO_OK;
}

int32_t Audio_HwpCfgVolumeTS(APP_DVOL_CFG *p_cfg_dvol)
{
//2851c/2885p no TS DVC
	return AIO_OK;
}

int32_t Audio_HwpCfgMute(APP_MUTE_CFG *p_cfg_mute)
{
	audio_digital_volume_control_1_RBUS	DVOL_CTRL_REG;

	down(&audio_sem_app);
	DVOL_CTRL_REG.regValue = Audio_HwpReadRegister(AUDIO_Digital_Volume_Control_1_reg);
	DVOL_CTRL_REG.zcto_mute       = p_cfg_mute->zero_timeout;
	DVOL_CTRL_REG.ramp_step_mute  = p_cfg_mute->ramp_step;
	DVOL_CTRL_REG.delay_ctrl_mute = p_cfg_mute->delay_time;
	Audio_HwpWriteRegister(AUDIO_Digital_Volume_Control_1_reg, DVOL_CTRL_REG.regValue);

	memcpy((void *)&m_cfg_mute, (void *)p_cfg_mute, sizeof(APP_MUTE_CFG));

	up(&audio_sem_app);

	return AIO_OK;
}

int32_t Audio_HwpCfgMuteTS(APP_MUTE_CFG *p_cfg_mute)
{
//2851c/2885p no support ts dvc

	return AIO_OK;
}

static int set_mute(unsigned int para, unsigned int ch_id)
{
	audio_lr_digital_volume_control_RBUS   DVOL_CH_LR_REG;

	audio_lsrs_digital_volume_control_RBUS DVOL_CH_LSRS_REG;
//	audio_csw_digital_volume_control_RBUS  DVOL_CH_CSW_REG;
//	audio_ch67_digital_volume_control_RBUS  DVOL_CH_ch67_REG;	

	audio_digital_volume_control_0_RBUS    DVOL_CTRL_0_REG;
	audio_digital_volume_control_1_RBUS    DVOL_CTRL_REG;
	uint32_t                               time_out         = 0;
//avoid making noise when mute/unmute
	if (ch_id == 0)
		return AIO_OK;

//avoid making noise when mute/unmute
#if 0
	//clear PB FIFO
	audio_down_sample_control_for_playback_RBUS  down_sample_control_for_palyback;

	
	if (ch_id == 0)
		return AIO_OK;
	
	down_sample_control_for_palyback.regValue = AIO_ReadRegister(AUDIO_down_sample_control_for_playback_reg);
	down_sample_control_for_palyback.backend_play_en = 0;
	AIO_WriteRegister(AUDIO_down_sample_control_for_playback_reg, down_sample_control_for_palyback.regValue);

	down_sample_control_for_palyback.regValue = AIO_ReadRegister(AUDIO_down_sample_control_for_playback_reg);
	down_sample_control_for_palyback.backend_play_en = 1;
	AIO_WriteRegister(AUDIO_down_sample_control_for_playback_reg, down_sample_control_for_palyback.regValue);
	
#endif

	DVOL_CTRL_0_REG.regValue = AIO_ReadRegister(AUDIO_Digital_Volume_Control_0_reg);
	DVOL_CTRL_REG.regValue   = AIO_ReadRegister(AUDIO_Digital_Volume_Control_1_reg);
//set ramp down, but not zero crossing+ramp down, due to at low frequency, zero crossing would to decade too long and timeout
	if (g_DVCParaDirectChange == FALSE) {
		// normal mode change
		if (1 == para) { //mute
			DVOL_CTRL_0_REG.dvcm = 2; //Soft Ramp
			DVOL_CTRL_REG.ramp_step_mute = 2; //4/8dB
			DVOL_CTRL_REG.zcto_mute = 1; //1024*2^1/Fs
			AIO_WriteRegister(AUDIO_Digital_Volume_Control_0_reg, DVOL_CTRL_0_REG.regValue);
			AIO_WriteRegister(AUDIO_Digital_Volume_Control_1_reg, DVOL_CTRL_REG.regValue);
		} else { //unmute
			DVOL_CTRL_0_REG.dvcm = 2; //Soft Ramp
			DVOL_CTRL_REG.ramp_step_mute = 1; //2/8dB
			DVOL_CTRL_REG.zcto_mute = 3; //1024*2^3/Fs
			AIO_WriteRegister(AUDIO_Digital_Volume_Control_0_reg, DVOL_CTRL_0_REG.regValue);
			AIO_WriteRegister(AUDIO_Digital_Volume_Control_1_reg, DVOL_CTRL_REG.regValue);
		}
	}
//remove dvol_apply polling here, need to move to end of setting
/*
	while (1) {
		DVOL_CTRL_REG.regValue = Audio_HwpReadRegister(AUDIO_Digital_Volume_Control_1_reg);
		if (DVOL_CTRL_REG.dvol_apply == 1) {
			//alog("[AUDIO][DRV] dvol_apply = 1\n");
		} else {
			break;
		}

		time_out++;

		if (time_out > 0x800000) {
			alog("[AUDIO][DRV] %s() TimeOut(0x%08x)!!!\n", __FUNCTION__, time_out);
			return AIO_ERROR;
		}
	}
*/
	//alog("[AUDIO][DRV] %s() try_cntr(0x%08x)\n",__FUNCTION__,time_out);

	// change dvol setting to MUTE mode
	DVOL_CTRL_REG.regValue = Audio_HwpReadRegister(AUDIO_Digital_Volume_Control_1_reg);
	DVOL_CTRL_REG.mute_apply = true;
	Audio_HwpWriteRegister(AUDIO_Digital_Volume_Control_1_reg, DVOL_CTRL_REG.regValue);


	if (ch_id&APP_CH_ID_CH0) {
		DVOL_CH_LR_REG.regValue = AIO_ReadRegister(AUDIO_LR_Digital_Volume_Control_reg);
		DVOL_CH_LR_REG.mute_l = para;
		DVOL_CH_LR_REG.rep_l_status = AIO_ENABLE;
		DVOL_CH_LR_REG.hpf_l_enable = AIO_ENABLE; //AIO_ENABLE for RTD2874
		AIO_WriteRegister(AUDIO_LR_Digital_Volume_Control_reg, DVOL_CH_LR_REG.regValue);
		time_out=0;
		while(1)//check hpf double buffer done
		{
		    DVOL_CH_LR_REG.regValue = AIO_ReadRegister(AUDIO_LR_Digital_Volume_Control_reg);
		    if (DVOL_CH_LR_REG.hpf_l_enable == 1)
			    break;
		    time_out++;
		    if (time_out > 0x800000) {
			    alog("[AUDIO][DRV] %s() TimeOut(0x%08x)!!!\n", __FUNCTION__, time_out);
			    return AIO_ERROR;
		    }
		}
	}

	if (ch_id&APP_CH_ID_CH1) {
		DVOL_CH_LR_REG.regValue = AIO_ReadRegister(AUDIO_LR_Digital_Volume_Control_reg);
		DVOL_CH_LR_REG.mute_r = para;
		DVOL_CH_LR_REG.rep_r_status = AIO_ENABLE;
		DVOL_CH_LR_REG.hpf_r_enable = AIO_ENABLE; //AIO_ENABLE for RTD2874
		AIO_WriteRegister(AUDIO_LR_Digital_Volume_Control_reg, DVOL_CH_LR_REG.regValue);
		time_out=0;
		while(1)//check hpf double buffer done
		{
		    DVOL_CH_LR_REG.regValue = AIO_ReadRegister(AUDIO_LR_Digital_Volume_Control_reg);
		    if (DVOL_CH_LR_REG.hpf_r_enable == 1)
			    break;
		    time_out++;
		    if (time_out > 0x800000) {
			    alog("[AUDIO][DRV] %s() TimeOut(0x%08x)!!!\n", __FUNCTION__, time_out);
			    return AIO_ERROR;
		    }
		}
	}

	if (ch_id&APP_CH_ID_CH2) {
		//modify HW CSW/LsRs naming
		DVOL_CH_LSRS_REG.regValue = AIO_ReadRegister(AUDIO_LsRs_Digital_Volume_Control_reg);
		DVOL_CH_LSRS_REG.mute_ls = para;
		DVOL_CH_LSRS_REG.rep_ls_status = AIO_ENABLE;
		DVOL_CH_LSRS_REG.hpf_ls_enable = AIO_ENABLE; //AIO_ENABLE for RTD2874
		AIO_WriteRegister(AUDIO_LsRs_Digital_Volume_Control_reg, DVOL_CH_LSRS_REG.regValue);
		time_out=0;
		while(1)//check hpf double buffer done
		{
		    DVOL_CH_LSRS_REG.regValue = AIO_ReadRegister(AUDIO_LsRs_Digital_Volume_Control_reg);
		    if (DVOL_CH_LSRS_REG.hpf_ls_enable == 1)
			    break;
		    time_out++;
		    if (time_out > 0x800000) {
			    alog("[AUDIO][DRV] %s() TimeOut(0x%08x)!!!\n", __FUNCTION__, time_out);
			    return AIO_ERROR;
		    }
		}
	}

	if (ch_id&APP_CH_ID_CH3) {
		//modify HW CSW/LsRs naming
		DVOL_CH_LSRS_REG.regValue = AIO_ReadRegister(AUDIO_LsRs_Digital_Volume_Control_reg);
		DVOL_CH_LSRS_REG.mute_rs = para;
		DVOL_CH_LSRS_REG.rep_rs_status = AIO_ENABLE;
		DVOL_CH_LSRS_REG.hpf_rs_enable = AIO_ENABLE; //AIO_ENABLE for RTD2874
		AIO_WriteRegister(AUDIO_LsRs_Digital_Volume_Control_reg, DVOL_CH_LSRS_REG.regValue);
		time_out=0;
		while(1)//check hpf double buffer done
		{
		    DVOL_CH_LSRS_REG.regValue = AIO_ReadRegister(AUDIO_LsRs_Digital_Volume_Control_reg);
		    if (DVOL_CH_LSRS_REG.hpf_rs_enable == 1)
			    break;
		    time_out++;
		    if (time_out > 0x800000) {
			    alog("[AUDIO][DRV] %s() TimeOut(0x%08x)!!!\n", __FUNCTION__, time_out);
			    return AIO_ERROR;
		    }
		}
	}
#if 0 //mac9q dvc 4ch
	if (ch_id&APP_CH_ID_CH4) {
		//modify HW CSW/LsRs naming
		DVOL_CH_CSW_REG.regValue = AIO_ReadRegister(AUDIO_CSW_Digital_Volume_Control_reg);
		DVOL_CH_CSW_REG.mute_c = para;
		DVOL_CH_CSW_REG.rep_c_status = AIO_ENABLE;
		DVOL_CH_CSW_REG.hpf_c_enable = AIO_ENABLE; //AIO_ENABLE
		AIO_WriteRegister(AUDIO_CSW_Digital_Volume_Control_reg, DVOL_CH_CSW_REG.regValue);
		time_out=0;
		while(1)//check hpf double buffer done
		{
		    DVOL_CH_CSW_REG.regValue = AIO_ReadRegister(AUDIO_CSW_Digital_Volume_Control_reg);
		    if (DVOL_CH_CSW_REG.hpf_c_enable == 1)
			    break;
		    time_out++;
		    if (time_out > 0x800000) {
			    alog("[AUDIO][DRV] %s() TimeOut(0x%08x)!!!\n", __FUNCTION__, time_out);
			    return AIO_ERROR;
		    }
		}
	}

	if (ch_id&APP_CH_ID_CH5) {
		//modify HW CSW/LsRs naming
		DVOL_CH_CSW_REG.regValue = AIO_ReadRegister(AUDIO_CSW_Digital_Volume_Control_reg);
		DVOL_CH_CSW_REG.mute_sw = para;
		DVOL_CH_CSW_REG.rep_sw_status = AIO_ENABLE;
		DVOL_CH_CSW_REG.hpf_sw_enable = AIO_ENABLE; //AIO_ENABLE
		AIO_WriteRegister(AUDIO_CSW_Digital_Volume_Control_reg, DVOL_CH_CSW_REG.regValue);
		time_out=0;
		while(1)//check hpf double buffer done
		{
		    DVOL_CH_CSW_REG.regValue = AIO_ReadRegister(AUDIO_CSW_Digital_Volume_Control_reg);
		    if (DVOL_CH_CSW_REG.hpf_sw_enable == 1)
			    break;
		    time_out++;
		    if (time_out > 0x800000) {
			    alog("[AUDIO][DRV] %s() TimeOut(0x%08x)!!!\n", __FUNCTION__, time_out);
			    return AIO_ERROR;
		    }
		}
	}

	if (ch_id&APP_CH_ID_CH6) {
		//modify HW ch67 naming
		DVOL_CH_ch67_REG.regValue = AIO_ReadRegister(AUDIO_ch67_Digital_Volume_Control_reg);
		DVOL_CH_ch67_REG.mute_ch6 = para;
		DVOL_CH_ch67_REG.rep_ch6_status = AIO_ENABLE;
		DVOL_CH_ch67_REG.hpf_ch6_enable = AIO_ENABLE; //AIO_ENABLE
		AIO_WriteRegister(AUDIO_ch67_Digital_Volume_Control_reg, DVOL_CH_ch67_REG.regValue);
		time_out=0;
		while(1)//check hpf double buffer done
		{
		    DVOL_CH_ch67_REG.regValue = AIO_ReadRegister(AUDIO_ch67_Digital_Volume_Control_reg);
		    if (DVOL_CH_ch67_REG.hpf_ch6_enable == 1)
			    break;
		    time_out++;
		    if (time_out > 0x800000) {
			    alog("[AUDIO][DRV] %s() TimeOut(0x%08x)!!!\n", __FUNCTION__, time_out);
			    return AIO_ERROR;
		    }
		}
	}

	if (ch_id&APP_CH_ID_CH7) {
		//modify HW ch67 naming
		DVOL_CH_ch67_REG.regValue = AIO_ReadRegister(AUDIO_ch67_Digital_Volume_Control_reg);
		DVOL_CH_ch67_REG.mute_ch7 = para;
		DVOL_CH_ch67_REG.rep_ch7_status = AIO_ENABLE;
		DVOL_CH_ch67_REG.hpf_ch7_enable = AIO_ENABLE; //AIO_ENABLE
		AIO_WriteRegister(AUDIO_ch67_Digital_Volume_Control_reg, DVOL_CH_ch67_REG.regValue);
		time_out=0;
		while(1)//check hpf double buffer done
		{
		    DVOL_CH_ch67_REG.regValue = AIO_ReadRegister(AUDIO_ch67_Digital_Volume_Control_reg);
		    if (DVOL_CH_ch67_REG.hpf_ch7_enable == 1)
			    break;
		    time_out++;
		    if (time_out > 0x800000) {
			    alog("[AUDIO][DRV] %s() TimeOut(0x%08x)!!!\n", __FUNCTION__, time_out);
			    return AIO_ERROR;
		    }
		}
	}

	DVOL_CTRL_REG.regValue = Audio_HwpReadRegister(AUDIO_Digital_Volume_Control_1_reg);
	DVOL_CTRL_REG.dvol_apply = AIO_ENABLE;
	Audio_HwpWriteRegister(AUDIO_Digital_Volume_Control_1_reg, DVOL_CTRL_REG.regValue);
//wait for ramp down
	while (1) {
		DVOL_CTRL_REG.regValue = Audio_HwpReadRegister(AUDIO_Digital_Volume_Control_1_reg);
		if (DVOL_CTRL_REG.dvol_apply == 1) {
			//alog("[AUDIO][DRV] dvol_apply = 1\n");
		} else {
			break;
		}

		time_out++;

		if (time_out > 0x800000) {
			alog("[AUDIO][DRV] %s() TimeOut(0x%08x)!!!\n", __FUNCTION__, time_out);
			return AIO_ERROR;
		}
	}
#endif

	return AIO_OK;
}
#if 0//ndef TV001_BOARD
static int set_fifo_mute(unsigned int para, unsigned int ch_id)
{
	audio_down_sample_fifo_source_sel_RBUS Down_Sample_FIFO_Source_Sel;
	//audio_down_sample_control_for_ts_RBUS Down_Sample_Control_for_TS;
	int                                    set_flag                    = 0;

	if (ch_id == 0)
		return AIO_OK;

	Down_Sample_FIFO_Source_Sel.regValue = AIO_ReadRegister(AUDIO_down_sample_fifo_source_sel_reg);
	//Down_Sample_Control_for_TS.regValue = AIO_ReadRegister(AUDIO_down_sample_control_for_ts_reg);

	if ((ch_id&APP_CH_ID_CH0) || (ch_id&APP_CH_ID_CH1)) {
		if (para == true) {
			if (Down_Sample_FIFO_Source_Sel.fifo_ch01_sel == AIO_FIFO_SEL_ATV) {
				Down_Sample_FIFO_Source_Sel.fifo_ch01_sel = AIO_FIFO_SEL_BBADC2;
				set_flag = 1;

			}
		} else {
			if (Down_Sample_FIFO_Source_Sel.fifo_ch01_sel == AIO_FIFO_SEL_BBADC2) {
				Down_Sample_FIFO_Source_Sel.fifo_ch01_sel = AIO_FIFO_SEL_ATV;
				set_flag = 1;
			}
		}
	}

	#ifdef SUPPORT_6_CHANNEL_PRESCALE
	if ((ch_id&APP_CH_ID_CH2) || (ch_id&APP_CH_ID_CH3)) {
		if (para == true) {
			if (Down_Sample_FIFO_Source_Sel.fifo_ch23_sel == AIO_FIFO_SEL_ATV) {
				Down_Sample_FIFO_Source_Sel.fifo_ch23_sel = AIO_FIFO_SEL_BBADC2;
				set_flag = 1;
			}
		} else {
			if (Down_Sample_FIFO_Source_Sel.fifo_ch23_sel == AIO_FIFO_SEL_BBADC2) {
				Down_Sample_FIFO_Source_Sel.fifo_ch23_sel = AIO_FIFO_SEL_ATV;
				set_flag = 1;
			}
		}
	}

	if ((ch_id&APP_CH_ID_CH4) || (ch_id&APP_CH_ID_CH5)) {
		if (para == true) {
			if (Down_Sample_FIFO_Source_Sel.fifo_ch45_sel == AIO_FIFO_SEL_ATV) {
				Down_Sample_FIFO_Source_Sel.fifo_ch45_sel = AIO_FIFO_SEL_BBADC2;
				set_flag = 1;
			}
		} else {
			if (Down_Sample_FIFO_Source_Sel.fifo_ch45_sel == AIO_FIFO_SEL_BBADC2) {
				Down_Sample_FIFO_Source_Sel.fifo_ch45_sel = AIO_FIFO_SEL_ATV;
				set_flag = 1;
			}
		}
	}
	#endif

	if (set_flag == 1) {
		AIO_WriteRegister(AUDIO_down_sample_fifo_source_sel_reg, Down_Sample_FIFO_Source_Sel.regValue);
	}

	return AIO_OK;
}
#endif
int32_t Audio_HwpSetMuteInternal(bool para, uint32_t ch_id, uint32_t mute_id)
{
	uint32_t i,
		 mask;

	if (mute_id >= APP_MUTE_NUM)
		return AIO_ERR_APP_INVALID_MUTE;

	if (para == AIO_ENABLE) {
		m_mute[mute_id] |= ch_id;   // store mute record

		if (m_isNeed2Print)
			alog("[AUDIO][APP] Mute[%d] CH:0x%x\n", mute_id, ch_id);

		//mute
		set_mute(AIO_ENABLE,  ch_id);

		Audio_AioSetMute(AIO_ENABLE, ch_id, AIO_AMPMUTE_APP);

	} else {
		m_mute[mute_id] &= ~ch_id;  // clear mute record
		mask = 0;
		for (i = 0; i < APP_MUTE_NUM; i++)
			mask |= m_mute[i];
		mask &= ch_id;
		mask = ch_id - mask;

		if (m_isNeed2Print)
			alog("[AUDIO][APP] Unmute[%d] CH:0x%x\n", mute_id, mask);

		//unmute
		set_mute(AIO_DISABLE,  mask);

		Audio_AioSetMute(AIO_DISABLE, mask, AIO_AMPMUTE_APP);
	}
#if 0
	if (m_isNeed2Print)
		alog("[AUDIO][USR] Mute ");

	//[Clayton] Fix converity: no need to print
	/*
	for (i = 0; i < USR_MUTE_NUM; i++) {

		if (m_isNeed2Print)
			alog("(%d:0x%x)", i, m_mute[i]);
	}
	*/

	if (m_isNeed2Print)
		alog("\n");

	if (m_isNeed2Print)
		alog("[AUDIO][LIB] Mute ");

	//[Clayton] Fix converity: no need to print
	/*
	for (i = 0; i < LIB_MUTE_NUM; i++) {
		if (m_isNeed2Print)
			alog("(%d:0x%x)", i, m_mute[USR_MUTE_NUM + i]);
	}
	*/

	if (m_isNeed2Print)
		alog("\n");
#endif

	return AIO_OK;
}

static int set_mute_ts(unsigned int para, unsigned int ch_id)
{
	//2851c/2885p no support ts dvc

	return AIO_OK;
}
#if 0//ndef TV001_BOARD
static int set_fifo_mute_ts(unsigned int para, unsigned int ch_id)
{
	//audio_down_sample_fifo_source_sel_RBUS Down_Sample_FIFO_Source_Sel;
	audio_down_sample_control_for_ts_RBUS Down_Sample_Control_for_TS;
	int                                   set_flag                   = 0;

	if (ch_id == 0)
		return AIO_OK;

	//Down_Sample_FIFO_Source_Sel.regValue = AIO_ReadRegister(AUDIO_down_sample_fifo_source_sel_reg);
	Down_Sample_Control_for_TS.regValue = AIO_ReadRegister(AUDIO_down_sample_control_for_ts_reg);

	if ((ch_id&APP_CH_ID_CH0) || (ch_id&APP_CH_ID_CH1)) {
		if (para == true) {
			if (Down_Sample_Control_for_TS.fifo_ch01_sel == AIO_FIFO_SEL_ATV) {
				Down_Sample_Control_for_TS.fifo_ch01_sel = AIO_FIFO_SEL_BBADC2;
				set_flag = 1;

			}
		} else {
			if (Down_Sample_Control_for_TS.fifo_ch01_sel == AIO_FIFO_SEL_BBADC2) {
				Down_Sample_Control_for_TS.fifo_ch01_sel = AIO_FIFO_SEL_ATV;
				set_flag = 1;
			}
		}
	}

	if (set_flag == 1) {
		AIO_WriteRegister(AUDIO_down_sample_control_for_ts_reg, Down_Sample_Control_for_TS.regValue);
	}

	return AIO_OK;
}
#endif
int32_t Audio_HwpSetMuteInternalTS(bool para, uint32_t ch_id, uint32_t mute_id)
{
	uint32_t i,
		 mask;

	if (mute_id >= APP_MUTE_NUM)
		return AIO_ERR_APP_INVALID_MUTE;

	if (para == AIO_ENABLE) {
		m_mute_ts[mute_id] |= ch_id;    // store mute record
		alog("[AUDIO][APP] TS_Mute[%d] CH:0x%x\n", mute_id, ch_id);
		//mute
		set_mute_ts(AIO_ENABLE,  ch_id);

	} else {
		m_mute_ts[mute_id] &= ~ch_id;   // clear mute record
		mask = 0;
		for (i = 0; i < APP_MUTE_NUM; i++)
			mask |= m_mute_ts[i];
		mask &= ch_id;
		mask = ch_id - mask;
		alog("[AUDIO][APP] TS_Unmute[%d] CH:0x%x\n", mute_id, mask);

		//unmute
		set_mute_ts(AIO_DISABLE, mask);
	}

	alog("[AUDIO][USR] TS_Mute ");
	for (i = 0; i < USR_MUTE_NUM; i++) {
		alog("(%d:0x%x)", i, m_mute_ts[i]);
	}

	alog("\n");

	alog("[AUDIO][LIB] TS_Mute ");
	for (i = 0; i < LIB_MUTE_NUM; i++) {
		alog("(%d:0x%x)", i, m_mute_ts[USR_MUTE_NUM + i]);
	}

	alog("\n");

	return AIO_OK;
}

void Audio_HwpCalibrateADDA(void)
{
	#ifdef SUPPORT_ADC_DVC//2851c,2885p not support it
	bool    adc1_enable = true;
	bool    adc2_enable = true;
	int32_t adc2_gain   = 0;
	#endif
	int32_t dac0_gain   = 0;
	int32_t dac1_gain   = 0;
	int32_t adc1_gain   = 0;
//	APP_AUDIO_OTP_T Audio_OTP;
//	unsigned int	Gain_Resolution = 4;	// 1:1/8dB ; 4:4/8dB 

	#ifdef SUPPORT_ADC_DVC//2851c,2885p not support it
	HwpEnableVolume_ADC1(adc1_enable);
	HwpSetVolume_ADC1(APP_ADC_CH_ID_L|APP_ADC_CH_ID_R, adc1_gain);
	HwpEnableVolume_ADC2(adc2_enable);
	HwpSetVolume_ADC2(APP_ADC_CH_ID_L|APP_ADC_CH_ID_R, adc2_gain);
	#else
	HwpSetGain_ADC(APP_ADC_CH_ID_L|APP_ADC_CH_ID_R, adc1_gain);
	#endif

	HwpSetGain_DAC0(APP_DAC_CH_ID_L|APP_DAC_CH_ID_R, dac0_gain);
	HwpSetGain_DAC1(APP_DAC_CH_ID_L|APP_DAC_CH_ID_R, dac1_gain);
}

int32_t Audio_HwpSetAVC(bool para, APP_AVC_CFG *p_cfg_avc)
{

	/* --------------------------------------------------*/
	/* ?Œâ??€?€?? ?Œâ??€?€?? ?Œâ??€?€?? ?Œâ??€?€?€?€?€?
	 * ?‚AVC1  ?œâ??¤dvol  ?œâ??¤AVC2  ?œâ??¤HW-limiter  ?
	 * ?”â??€?€?? ?”â??€?€?? ?”â??€?€?? ?”â??€?€?€?€?€?
	 * --------------------------------------------------*/
	/* ryanliao
	  1. m_cfg_tsavc--->HWSD suggest paramters. (only large volume suppressing)
	  2. p_cfg_tsavc--->User modify paramters.(only large volume suppressing)
	  3. AGC (Auto Volume control) use mothed
	  --->Need to enable "small volume amplification" (avc_vol_amplif_enable)
	  --->Paramters must setup the same Hi_th and Lo_th value.
	  */

	audio_hw_process_control_RBUS    HWP_CTRL_REG;
	audio_auto_volume_control_0_RBUS AVC_CTRL0_REG;
	audio_avc_volume_control_1_RBUS  AVC_CTRL1_REG;
	audio_avc_volume_control_2_RBUS  AVC_CTRL2_REG;  // Clayton 2013/1/29
	audio_avc_volume_control_3_RBUS  AVC_CTRL3_REG;
	APP_AVC_CFG                      *t_cfg_avc;

	if (p_cfg_avc == NULL) {
		//apply default setting
		t_cfg_avc = &m_cfg_avc;
	} else {
		t_cfg_avc = p_cfg_avc;
	}

	//Clayton 2013/1/29
	// Fixed LPF of AVC1 at 2^6, the best setting for attack/release time.
	AVC_CTRL2_REG.regValue = AIO_ReadRegister(AUDIO_AVC_Volume_Control_2_reg);
	AVC_CTRL2_REG.avc1_coef = 	APP_AVC_LPF_COEF_4;     // Th// always fixed at 2^4 would be better
	AIO_WriteRegister(AUDIO_AVC_Volume_Control_2_reg, AVC_CTRL2_REG.regValue);
	/*
	 * disable H/W-AVC before configure it.
	 * This register is a double buffer register,we should check it after setting,
	 * but this version of code, we just assume that H/W reaction is always fast enough.
	 */
	HWP_CTRL_REG.regValue = Audio_HwpReadRegister(AUDIO_HW_Process_Control_reg);
	HWP_CTRL_REG.auto_volenable = AIO_DISABLE;
	Audio_HwpWriteRegister(AUDIO_HW_Process_Control_reg, HWP_CTRL_REG.regValue);

	if (para == AIO_ENABLE) {
		// configure H/W-AVC
		AVC_CTRL0_REG.regValue = AIO_ReadRegister(AUDIO_Auto_Volume_Control_0_reg);
		AVC_CTRL0_REG.avc_noise_gain = t_cfg_avc->noise_gain;
		AVC_CTRL0_REG.avc_noise_thre = t_cfg_avc->noise_thd; // Function modify OK after Mac2 and Sirius SOC
		AVC_CTRL0_REG.avc_level_max  = t_cfg_avc->level_max;
		AVC_CTRL0_REG.avc_level_min  = t_cfg_avc->level_min;
		AVC_CTRL0_REG.avc_down_db    = t_cfg_avc->step_down;
		AVC_CTRL0_REG.avc_up_db      = t_cfg_avc->step_up;
		AVC_CTRL0_REG.avc_vol_detect = t_cfg_avc->vol_detect;   // Clayton 2013/8/23
		AIO_WriteRegister(AUDIO_Auto_Volume_Control_0_reg, AVC_CTRL0_REG.regValue);

		AVC_CTRL1_REG.regValue = AIO_ReadRegister(AUDIO_AVC_Volume_Control_1_reg);
		AVC_CTRL1_REG.avc_cnt_thre = t_cfg_avc->win_moniter / 2; //ryanliao
		AVC_CTRL1_REG.avc_mon_win  = t_cfg_avc->win_moniter;
		AVC_CTRL1_REG.avc_zc_win   = 0x3ff;//t_cfg_avc->win_zcross;    //set zc timeout to max, to avoid po noise at 20hz
		AIO_WriteRegister(AUDIO_AVC_Volume_Control_1_reg, AVC_CTRL1_REG.regValue);

		AVC_CTRL2_REG.regValue = AIO_ReadRegister(AUDIO_AVC_Volume_Control_2_reg);
		AVC_CTRL2_REG.avc_vol_amplif_enable = t_cfg_avc->avc_vol_amplif_enable;
		AVC_CTRL2_REG.avc_small_level_max   = t_cfg_avc->avc_small_level_max;
		AVC_CTRL2_REG.avc_small_level_min = t_cfg_avc->avc_small_level_min;
		AVC_CTRL2_REG.avc_small_down_db  = t_cfg_avc->avc_small_down_db;
		AVC_CTRL2_REG.avc_small_up_db   = t_cfg_avc->avc_small_up_db;
		AVC_CTRL2_REG.avc_small_cnt_thre   = t_cfg_avc->win_moniter / 2; //ryanliao
        AVC_CTRL2_REG.avc1_coef = t_cfg_avc->avc1_coef;		
		AIO_WriteRegister(AUDIO_AVC_Volume_Control_2_reg, AVC_CTRL2_REG.regValue);

		AVC_CTRL3_REG.regValue = AIO_ReadRegister(AUDIO_AVC_Volume_Control_3_reg);
		AVC_CTRL3_REG.avc_small_level = t_cfg_avc->avc_small_level;
		AIO_WriteRegister(AUDIO_AVC_Volume_Control_3_reg, AVC_CTRL3_REG.regValue);

		// enable H/W-AVC again.
		HWP_CTRL_REG.regValue = Audio_HwpReadRegister(AUDIO_HW_Process_Control_reg);
		HWP_CTRL_REG.auto_volenable = AIO_ENABLE;
		Audio_HwpWriteRegister(AUDIO_HW_Process_Control_reg, HWP_CTRL_REG.regValue);
	}

	return AIO_OK;
}

int32_t Audio_HwpSetAVC2(bool para, APP_AVC2_CFG *p_cfg_avc2)
{
	/* --------------------------------------------------*/
	/* ?Œâ??€?€?? ?Œâ??€?€?? ?Œâ??€?€?? ?Œâ??€?€?€?€?€?
	 * ?‚AVC1  ?œâ??¤dvol  ?œâ??¤AVC2  ?œâ??¤HW-limiter  ?
	 * ?”â??€?€?? ?”â??€?€?? ?”â??€?€?? ?”â??€?€?€?€?€?
	 * --------------------------------------------------*/
	/* ryanliao
	   1. m_cfg_tsavc--->HWSD suggest paramters.(only large volume suppressing)
	   2. p_cfg_tsavc--->User modify paramters.(only large volume suppressing)
	   3. AGC (Auto Volume control) use mothed
	   --->Need to enable "small volume amplification" (avc2_vol_amplif_enable)
	   --->Paramters must setup the same Hi_th and Lo_th value.
	   */

	//hw_process_control_RBUS		HWP_CTRL_REG;
	audio_avc2_control_1_ch01_RBUS AVC2_CTRL0_REG;
	audio_avc2_control_2_ch01_RBUS AVC2_CTRL1_REG;
	audio_avc2_control_3_ch01_RBUS AVC2_CTRL2_REG;
	audio_avc2_control_4_ch01_RBUS AVC2_CTRL3_REG;
	audio_avc2_control_5_ch01_RBUS AVC2_CTRL4_REG;
	audio_avc2_control_6_ch01_RBUS AVC2_CTRL5_REG;
	audio_avc2_control_7_ch01_RBUS AVC2_CTRL6_REG;
	APP_AVC2_CFG                   *t_cfg_avc2;
	audio_hw_process_control_RBUS  hw_process_control;

	if (p_cfg_avc2 == NULL) {
		//apply default setting
		t_cfg_avc2 = &m_cfg_avc2;
	} else {
		t_cfg_avc2 = p_cfg_avc2;
	}

	/*
	 * disable H/W-AVC before configure it.
	 * This register is a double buffer register,we should check it after setting,
	 * but this version of code, we just assume that H/W reaction is always fast enough.
	 */
	hw_process_control.regValue = Audio_HwpReadRegister(AUDIO_HW_Process_Control_reg);  // Clayton 2013/1/28
	hw_process_control.avc2_enable = AIO_DISABLE;   // Clayton 2013/1/28
	Audio_HwpWriteRegister(AUDIO_HW_Process_Control_reg, hw_process_control.regValue);  // Clayton 2013/1/28

	// Clayton 2013/11/11
	// Please refer "APP_AVC2_LPF_COEF" enum
	// [Start]
	AVC2_CTRL0_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_1_ch01_reg);
	AIO_WriteRegister(AUDIO_AVC2_Control_1_ch01_reg, AVC2_CTRL0_REG.regValue);
	// [End]

	if (para == AIO_ENABLE) {
		// configure H/W-AVC2
		AVC2_CTRL0_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_1_ch01_reg);
		AVC2_CTRL0_REG.avc2_noise_gain = t_cfg_avc2->avc2_noise_gain;
		AVC2_CTRL0_REG.avc2_noise_thre = t_cfg_avc2->avc2_noise_thre;   // Function modify OK after Mac2 and Sirius SOC
		AVC2_CTRL0_REG.fast_up_db = t_cfg_avc2->fast_up_db;
		AVC2_CTRL0_REG.slow_up_db = t_cfg_avc2->slow_up_db;
		AVC2_CTRL0_REG.down_db = t_cfg_avc2->down_db;
		AVC2_CTRL0_REG.avc2_vol_detect = t_cfg_avc2->avc2_vol_detect;   // Clayton 2013/4/10
		AVC2_CTRL0_REG.avc2_vol_amplif_enable = t_cfg_avc2->avc2_vol_amplif_enable;
		AVC2_CTRL0_REG.avc2_lpf_coef = t_cfg_avc2->avc2_lpf_coef;
		AIO_WriteRegister(AUDIO_AVC2_Control_1_ch01_reg, AVC2_CTRL0_REG.regValue);

		AVC2_CTRL1_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_2_ch01_reg);
		AVC2_CTRL1_REG.avc2_cnt_thre =  t_cfg_avc2->avc2_mon_win / 2; //ryanliao
		AVC2_CTRL1_REG.avc2_mon_win = t_cfg_avc2->avc2_mon_win;
		AVC2_CTRL1_REG.avc2_zc_win = 0x3ff;//t_cfg_avc2->avc2_zc_win;  // Clayton 2014/12/3, Speed up AVC control with ZC_win = Mon_win (Default is 0x200, too long)
		AIO_WriteRegister(AUDIO_AVC2_Control_2_ch01_reg, AVC2_CTRL1_REG.regValue);

		AVC2_CTRL2_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_3_ch01_reg); // RTD289X add new condition ryanliao 2016/12/19
		AVC2_CTRL2_REG.avc2_noise_thre_hi = t_cfg_avc2->avc2_noise_thre_hi;
		AIO_WriteRegister(AUDIO_AVC2_Control_3_ch01_reg, AVC2_CTRL2_REG.regValue);

		AVC2_CTRL3_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_4_ch01_reg);
		AVC2_CTRL3_REG.avc2_level_max = t_cfg_avc2->avc2_level_max;
		AVC2_CTRL3_REG.avc2_level_min = t_cfg_avc2->avc2_level_min;
		AIO_WriteRegister(AUDIO_AVC2_Control_4_ch01_reg, AVC2_CTRL3_REG.regValue);

		AVC2_CTRL4_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_5_ch01_reg);
		AVC2_CTRL4_REG.avc2_small_up_db = t_cfg_avc2->avc2_small_up_db;
		AVC2_CTRL4_REG.avc2_small_down_db = t_cfg_avc2->avc2_small_down_db;
		AVC2_CTRL4_REG.avc2_small_cnt_thre = t_cfg_avc2->avc2_mon_win / 2; //ryanliao
		AIO_WriteRegister(AUDIO_AVC2_Control_5_ch01_reg, AVC2_CTRL4_REG.regValue);

		AVC2_CTRL5_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_6_ch01_reg);
		AVC2_CTRL5_REG.avc2_small_level_max = t_cfg_avc2->avc2_small_level_max;
		AVC2_CTRL5_REG.avc2_small_level_min = t_cfg_avc2->avc2_small_level_min;
		AIO_WriteRegister(AUDIO_AVC2_Control_6_ch01_reg, AVC2_CTRL5_REG.regValue);

		AVC2_CTRL6_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_7_ch01_reg);
		AVC2_CTRL6_REG.avc2_small_level = t_cfg_avc2->avc2_small_level;
		AIO_WriteRegister(AUDIO_AVC2_Control_7_ch01_reg, AVC2_CTRL6_REG.regValue);


		// enable H/W-AVC again.
		//HWP_CTRL_REG.regValue = AIO_ReadRegister(AUDIO_HW_Process_Control_reg);
		//HWP_CTRL_REG.auto_volenable = AIO_ENABLE;
		//AIO_WriteRegister(AUDIO_HW_Process_Control_reg, HWP_CTRL_REG.regValue);
		hw_process_control.regValue = Audio_HwpReadRegister(AUDIO_HW_Process_Control_reg);  // Clayton 2013/1/28
		hw_process_control.avc2_enable = AIO_ENABLE;    // Clayton 2013/1/28
		Audio_HwpWriteRegister(AUDIO_HW_Process_Control_reg, hw_process_control.regValue);  // Clayton 2013/1/28

	}

	return AIO_OK;
}

int32_t Audio_HwpSetDistortionCtrl(bool para, APP_DIST_CTRL_CFG *p_cfg_dist)
{
	/* --------------------------------------------------*/
	/* ?Œâ??€?€?? ?Œâ??€?€?? ?Œâ??€?€?? ?Œâ??€?€?€?€?€?
	 * ?‚Dist  ?œâ??¤AVC1  ?œâ??¤DVC   ?œâ??¤AVC2  ?
	 * ?”â??€?€?? ?”â??€?€?? ?”â??€?€?? ?”â??€?€?€?€?€?
	 * --------------------------------------------------*/

	//audio_hw_process_control_RBUS		HWP_CTRL_REG;
	audio_hw_distortion_control_0_RBUS hw_distortion_control_0_REG;
	audio_hw_distortion_control_1_RBUS hw_distortion_control_1_REG;
	audio_hw_process_control_RBUS      hw_process_control;
	APP_DIST_CTRL_CFG                  *t_cfg_dist;

	if (p_cfg_dist == NULL) {
		//apply default setting
		t_cfg_dist = &m_cfg_dist;
	} else {
		t_cfg_dist = p_cfg_dist;
	}

	// Disable Distortion control before setting
	hw_process_control.regValue = Audio_HwpReadRegister(AUDIO_HW_Process_Control_reg);
	hw_process_control.distortion_enable = AIO_DISABLE;
	Audio_HwpWriteRegister(AUDIO_HW_Process_Control_reg, hw_process_control.regValue);

	// Set Distortion control LPF coef
	hw_distortion_control_0_REG.regValue = AIO_ReadRegister(AUDIO_hw_distortion_Control_0_reg);
	hw_distortion_control_0_REG.dist_coef = t_cfg_dist->dist_coef;
	AIO_WriteRegister(AUDIO_hw_distortion_Control_0_reg, hw_distortion_control_0_REG.regValue);

	if (para == AIO_ENABLE) {
		// Set Distortion control TH & speed related
		hw_distortion_control_0_REG.regValue = AIO_ReadRegister(AUDIO_hw_distortion_Control_0_reg);
		hw_distortion_control_0_REG.dist_vol_detect = t_cfg_dist->dist_vol_detect;
		hw_distortion_control_0_REG.dist_high_thre = t_cfg_dist->dist_high_thre;
		hw_distortion_control_0_REG.dist_low_thre = t_cfg_dist->dist_low_thre;
		hw_distortion_control_0_REG.dist_down_db = t_cfg_dist->dist_down_db;
		hw_distortion_control_0_REG.dist_up_db = t_cfg_dist->dist_up_db;
		AIO_WriteRegister(AUDIO_hw_distortion_Control_0_reg, hw_distortion_control_0_REG.regValue);

		// Set Distortion control detected window related
		hw_distortion_control_1_REG.regValue = AIO_ReadRegister(AUDIO_hw_distortion_Control_1_reg);
		hw_distortion_control_1_REG.dist_cnt_thre = t_cfg_dist->dist_mon_win / 2;
		hw_distortion_control_1_REG.dist_mon_win = t_cfg_dist->dist_mon_win;
		hw_distortion_control_1_REG.dist_zc_win = 0x3ff;//t_cfg_dist->dist_zc_win;//set to max, to avoid pop noise at 20hz
		AIO_WriteRegister(AUDIO_hw_distortion_Control_1_reg, hw_distortion_control_1_REG.regValue);

		// Enable Distortion control
		audio_hw_usleep(40); //Frank, need 20us delay
		hw_process_control.regValue = Audio_HwpReadRegister(AUDIO_HW_Process_Control_reg);
		hw_process_control.distortion_enable = AIO_ENABLE;
		Audio_HwpWriteRegister(AUDIO_HW_Process_Control_reg, hw_process_control.regValue);

	}

	return AIO_OK;
}
#if 0
static int set_eq_reg(unsigned int address, int data, int shift)
{
	return AIO_OK;

}
#endif
/**
 * Setup H/W Treble coefficients
 *
 * @param <p_cfg_treble>	{ pointer of Treble-Band control block }
 * @return					{ AIO_OK or AIO_ERROR_CODE }
 * @note
 *    This function is mutual with Speaker Compensation.
 *
 * @ingroup lib_audio
 */
#if 0 
static int set_loud_coef(unsigned int band_id)
{
	return AIO_OK;
}
#endif
int audio_drv_set_bass_val(APP_BASS_VAL *p_bass_val)
{
	return AIO_OK;
}

int32_t Audio_HwpSetBassValue(bool para, APP_BASS_VAL *p_val_bass)
{
	return AIO_OK;
}

// Clayton 2013/12/2
// Use audio table only for 48KHz sample rate case
int audio_drv_set_bass_val_48K(APP_BASS_VAL *p_bass_val)
{
	return AIO_OK;
}

// Clayton 2013/12/2
// Use audio table only for 48KHz sample rate case
int32_t Audio_HwpSetBassValue_48K(bool para, APP_BASS_VAL val_bass)
{
	return AIO_OK;
}

int audio_drv_set_treble_val(APP_TREBLE_VAL *p_treble_val)
{
	return AIO_OK;
}

// Clayton 2013/12/4
// Use audio table only for 48KHz sample rate case
int audio_drv_set_treble_val_48K(APP_TREBLE_VAL *p_treble_val)
{
	return AIO_OK;
}

int audio_drv_set_treble2_val(APP_TREBLE_VAL *p_treble2_val)
{
	return AIO_OK;
}

// Clayton 2013/12/4
// Use audio table only for 48KHz sample rate case
int audio_drv_set_treble2_val_48K(APP_TREBLE_VAL *p_treble2_val)
{
	return AIO_OK;
}

int32_t Audio_HwpSetTrebleValue(bool para, APP_TREBLE_VAL val_treble)
{
	return AIO_OK;
}

// Clayton 2013/12/4
// Use audio table only for 48KHz sample rate case
int32_t Audio_HwpSetTrebleValue_48K(bool para, APP_TREBLE_VAL val_treble)
{
	return AIO_OK;
}

// Clayton 2013/10/17
// Add new API function for Treble2
int32_t Audio_HwpSetTreble2Value(bool para, APP_TREBLE_VAL val_treble2)
{
	return AIO_OK;
}

// Clayton 2013/12/4
// Use audio table only for 48KHz sample rate case
int32_t Audio_HwpSetTreble2Value_48K(bool para, APP_TREBLE_VAL val_treble2)
{
	return AIO_OK;
}

int audio_drv_set_treble_coef(APP_TREBLE_CFG *p_treble_cfg)
{
	return AIO_OK;
}

// Clayton 2013/12/2
// Use audio table only for 48KHz sample rate case
int audio_drv_set_treble_coef_48K(APP_TREBLE_48K_CFG *p_treble_cfg)
{
	return AIO_OK;
}

// Clayton 2013/10/17
// Add new API for Treble2
int audio_drv_set_treble2_coef(APP_TREBLE_CFG *p_treble2_cfg)
{
	return AIO_OK;
}

// Clayton 2013/12/2
// Use audio table only for 48KHz sample rate case
int audio_drv_set_treble2_coef_48K(APP_TREBLE_48K_CFG *p_treble2_cfg)
{
	return AIO_OK;
}

int32_t Audio_HwpSetTrebleCoef(APP_TREBLE_CFG *p_cfg_treble)
{

	if (p_cfg_treble == NULL)
		return AIO_ERR_APP_INVALID_TREBLE_COEF;

	audio_drv_set_treble_coef(p_cfg_treble);

	return AIO_OK;
}

// Clayton 2013/12/2
// Use audio table only for 48KHz sample rate case
int32_t Audio_HwpSetTrebleCoef_48K(APP_TREBLE_48K_CFG *p_cfg_treble)
{

	if (p_cfg_treble == NULL)
		return AIO_ERR_APP_INVALID_TREBLE_COEF;

	audio_drv_set_treble_coef_48K(p_cfg_treble);

	return AIO_OK;
}

// Add BP2 of Loudness for treble2
int32_t Audio_HwpSetTreble2Coef(APP_TREBLE_CFG *p_cfg_treble2)
{

	if (p_cfg_treble2 == NULL)
		return AIO_ERR_APP_INVALID_TREBLE_COEF; //check] Should add "AIO_ERR_APP_INVALID_TREBLE2_VOL" ?

	audio_drv_set_treble2_coef(p_cfg_treble2);

	return AIO_OK;
}

// Clayton 2013/12/2
// Use audio table only for 48KHz sample rate case
int32_t Audio_HwpSetTreble2Coef_48K(APP_TREBLE_48K_CFG *p_cfg_treble2)
{

	if (p_cfg_treble2 == NULL)
		return AIO_ERR_APP_INVALID_TREBLE_COEF; //check] Should add "AIO_ERR_APP_INVALID_TREBLE2_VOL" ?

	audio_drv_set_treble2_coef_48K(p_cfg_treble2);

	return AIO_OK;
}

int audio_drv_set_bass_coef(APP_BASS_CFG *p_bass_cfg)
{
	return AIO_OK;
}

// Clayton 2013/12/2
// Use audio table only for 48KHz sample rate case
int audio_drv_set_bass_coef_48K(APP_BASS_48K_CFG *p_bass_cfg)
{
	return AIO_OK;
}

int32_t Audio_HwpSetBassCoef(APP_BASS_CFG *p_cfg_bass)
{

	if (p_cfg_bass == NULL)
		return AIO_ERR_APP_INVALID_BASS_COEF;

	audio_drv_set_bass_coef(p_cfg_bass);

	return AIO_OK;
}

// Clayton 2013/12/2
// Use audio table only for 48KHz sample rate case
int32_t Audio_HwpSetBassCoef_48K(APP_BASS_48K_CFG *p_cfg_bass)
{

	if (p_cfg_bass == NULL)
		return AIO_ERR_APP_INVALID_BASS_COEF;

	audio_drv_set_bass_coef_48K(p_cfg_bass);

	return AIO_OK;
}
#if 0
static int set_eq_coef(unsigned int band_id)
{
	return AIO_OK;
}
#endif
int audio_drv_set_eq_val(APP_EQ_VAL *p_eq_val)
{
	return AIO_OK;
}

// Clayton 2013/12/4
// Use audio table only for 48KHz sample rate case
int audio_drv_set_eq_val_48K(APP_EQ_VAL *p_eq_val)
{
	return AIO_OK;
}

void DisableEQPara(void)
{
}

//[end]
void DisableBassPara(void)
{
	
}

void DisableTreblePara(void)
{
	
}

void DisableTreble2Para(void)
{
	
}

/**
 * Setup H/W Equalizer values
 *
 * @param <para>		{ AIO_ENABLE/AIO_DISABLE }
 * @param <p_val_eq>	{ pointer of a set of EQ value }
 * @return				{ AIO_OK or AIO_ERROR_CODE }
 * @ingroup lib_audio
 */
int32_t Audio_HwpSetEQValue(bool para, APP_EQ_VAL val_eq)
{
	return AIO_OK;
}

// Clayton 2013/12/2
// Use audio table only for 48KHz sample rate case
// API using guide
// 1. Parameter1: Band_enable (1: Band enable / 0: Band disable)
//  effective 9bit: xxxx xxx1 1111 1111
//  bit0: EQ_LPF0
//  bit1~bit7: EQ_BPF1 ~ EQ_BPF7
/* if  (APP_EQ_BAND_NUM == 9)
     bit8: EQ_HPF
   else if (APP_EQ_BAND_NUM == 11)
     bit8~9: EQ_BPF8~BPF9
     bit10: EQ_HPF

*/
int32_t Audio_HwpSetEQValue_48K(uint32_t Band_enable, APP_EQ_VAL *p_val_eq)
{
	return AIO_OK;
}

int audio_drv_set_eq_coef(APP_EQ_CFG *p_eq_cfg)
{
	return AIO_OK;
}

// Clayton 2013/12/2
// Use audio table only for 48KHz sample rate case
int audio_drv_set_eq_coef_48K(APP_EQ_48K_CFG *p_eq_cfg)
{
	return AIO_OK;
}

int32_t Audio_HwpSetEQCoef(APP_EQ_CFG *p_cfg_eq)
{
	if (p_cfg_eq == NULL)
		return AIO_ERR_APP_INVALID_EQ_COEF;

	audio_drv_set_eq_coef(p_cfg_eq);

	return AIO_OK;
}

// Clayton 2013/12/2
// Use audio table only for 48KHz sample rate case
int32_t Audio_HwpSetEQCoef_48K(APP_EQ_48K_CFG *p_cfg_eq)
{
	if (p_cfg_eq == NULL)
		return AIO_ERR_APP_INVALID_EQ_COEF;

	audio_drv_set_eq_coef_48K(p_cfg_eq);

	return AIO_OK;
}

// Clayton 2015/4/30
int32_t Audio_HwpSetEQLoudValue_Change_Mode(APP_EQ_LOUD_VALUE_CHANGE_MODE eqmode)
{
	if (eqmode == APP_EQ_LOUD_VALUE_DIRECT)
		g_EQLoudParaDirectChange = true;
	else
		g_EQLoudParaDirectChange = false;

	alog("[AUDIO][APP] EQLoudValue Change Mode = %x\n", g_EQLoudParaDirectChange);

	return AIO_OK;
}

// ryanliao 2017/3/14
int32_t Audio_HwpSetDVCValue_Change_Mode(APP_DVC_VALUE_CHANGE_MODE dvcmode)
{
	audio_digital_volume_control_0_RBUS       DVOL_CTRL_0_REG;
//	audio_lrrr_digital_volume_control_1_RBUS	DVOL_CTRL1_REG;//2851c/1885p not support ts dvc


	DVOL_CTRL_0_REG.regValue = AIO_ReadRegister(AUDIO_Digital_Volume_Control_0_reg);
//	DVOL_CTRL1_REG.regValue = Audio_HwpReadRegister(AUDIO_Digital_Volume_Control_1_reg);

	
	if(dvcmode == APP_DVC_VALUE_DIRECT){
		g_DVCParaDirectChange = TRUE; //direct mode Immedicate change
		// for googleTV init flow ryanliao 2017/03/14
		DVOL_CTRL_0_REG.dvcm = APP_DVOL_MODE_NORMAL; //direct mode Immedicate change
//		DVOL_CTRL1_REG.lrrr_dvcm = APP_DVOL_MODE_NORMAL; //direct mode Immedicate change
   		AIO_WriteRegister(AUDIO_Digital_Volume_Control_0_reg, DVOL_CTRL_0_REG.regValue); 
//		Audio_HwpWriteRegister(AUDIO_Digital_Volume_Control_1_reg, DVOL_CTRL1_REG.regValue);
		}
	else{
		g_DVCParaDirectChange = FALSE; //normal mode mute/unmute have soft-ramp sound
		DVOL_CTRL_0_REG.dvcm = APP_DVOL_MODE_ZERO_AND_SOFT; //normal mode change Soft Ramp on Zero Crossing
//		DVOL_CTRL1_REG.lrrr_dvcm = APP_DVOL_MODE_ZERO_AND_SOFT; //normal mode change Soft Ramp on Zero Crossing
   		AIO_WriteRegister(AUDIO_Digital_Volume_Control_0_reg, DVOL_CTRL_0_REG.regValue);
//		Audio_HwpWriteRegister(AUDIO_Digital_Volume_Control_1_reg, DVOL_CTRL1_REG.regValue);
		}
	alog("[AUDIO][APP] DVCPara Change Mode = %x\n", g_DVCParaDirectChange);

	return AIO_OK;
}


int audio_drv_set_eq_val_9band(APP_9BAND_EQ_VAL *p_9band_eq_val)
{
	return AIO_OK;
}

int32_t Audio_HwpSetEQValue_9Band(bool para, APP_9BAND_EQ_VAL *p_val_eq_9band)
{
	return AIO_OK;
}

int audio_drv_set_eq_coef_9band(APP_9BAND_EQ_CFG *p_9band_eq_cfg)
{
	return AIO_OK;
}

int32_t Audio_HwpSetEQCoef_9Band(APP_9BAND_EQ_CFG *p_cfg_eq_9band)
{

	if (p_cfg_eq_9band == NULL)
		return AIO_ERR_APP_INVALID_EQ_COEF;

	audio_drv_set_eq_coef_9band(p_cfg_eq_9band);

	return AIO_OK;
}

int32_t Audio_HwpSetSpatialEffect(bool para, APP_SE_CFG *p_cfg_se)
{

	audio_hw_process_control_RBUS       HWP_CTRL_REG;
	audio_spatial_effect_control_0_RBUS SPL_CTRL0_REG;
	audio_spatial_effect_control_1_RBUS SPL_CTRL1_REG;
	APP_SE_CFG                          *t_cfg_se;

	APP_SE_CFG null_cfg = {0};
	//if p_cfg_se = null_cfg, using default table;
	if (memcmp(&null_cfg, p_cfg_se, sizeof(APP_SE_CFG)) == 0) {
		//return AIO_ERR_APP_INVALID_SPATIAL;
		t_cfg_se = &m_cfg_se;
	} else {
		t_cfg_se = p_cfg_se;
	}
	 //disable H/W-SE before configure it.
	 //This register is a double buffer register,we should check it after setting,
	 //but this version of code, we just assume that H/W reaction is always fast enough.

//	HWP_CTRL_REG.regValue = Audio_HwpReadRegister(AUDIO_HW_Process_Control_reg);
//	HWP_CTRL_REG.spatial_ctrlenable = AIO_DISABLE;
//	Audio_HwpWriteRegister(AUDIO_HW_Process_Control_reg, HWP_CTRL_REG.regValue);

	SPL_CTRL0_REG.regValue = Audio_HwpReadRegister(AUDIO_Spatial_Effect_control_0_reg);
	SPL_CTRL0_REG.spatial_zc_apply = 0;//disable zero cross, if input mono audio file will make no sound
	Audio_HwpWriteRegister(AUDIO_Spatial_Effect_control_0_reg, SPL_CTRL0_REG.regValue);

	if (para == AIO_ENABLE) {
		// combine spatial & mono-to-stereo together.
		SPL_CTRL0_REG.regValue = AIO_ReadRegister(AUDIO_Spatial_Effect_control_0_reg);
		SPL_CTRL0_REG.apf_en = t_cfg_se->apf_en;
		SPL_CTRL0_REG.f_coef = t_cfg_se->apf_coef;
		SPL_CTRL0_REG.psb_en = t_cfg_se->psb_en;
		SPL_CTRL0_REG.seb_en = t_cfg_se->seb_en;
		SPL_CTRL0_REG.spatial_in_l_sel = t_cfg_se->sel_l;
		SPL_CTRL0_REG.spatial_in_r_sel = t_cfg_se->sel_r;
		AIO_WriteRegister(AUDIO_Spatial_Effect_control_0_reg, SPL_CTRL0_REG.regValue);

		SPL_CTRL1_REG.regValue = AIO_ReadRegister(AUDIO_Spatial_Effect_control_1_reg);
		SPL_CTRL1_REG.depth1 = t_cfg_se->depth1;
		SPL_CTRL1_REG.depth2 = t_cfg_se->depth2;
		AIO_WriteRegister(AUDIO_Spatial_Effect_control_1_reg, SPL_CTRL1_REG.regValue);

		// enable H/W-Spatial Effect
		HWP_CTRL_REG.regValue = Audio_HwpReadRegister(AUDIO_HW_Process_Control_reg);
		HWP_CTRL_REG.spatial_ctrlenable = AIO_ENABLE;
		Audio_HwpWriteRegister(AUDIO_HW_Process_Control_reg, HWP_CTRL_REG.regValue);

	}

	// Clayton 2012/11/1 ; adding disable HW spatial funtion
	else {
		HWP_CTRL_REG.regValue = Audio_HwpReadRegister(AUDIO_HW_Process_Control_reg);
		HWP_CTRL_REG.spatial_ctrlenable = 0;
		Audio_HwpWriteRegister(AUDIO_HW_Process_Control_reg, HWP_CTRL_REG.regValue);
	}

	return AIO_OK;

}

#ifdef USE_SW_SOUND_SURROUND
int32_t Audio_SwpSetSRS_TrusurroundHD(bool para, APP_TSXT_CFG *pTSXT)
{
	//#ifdef ENABLE_SRS_SURROUND // gene modify => default enable it
	CLNT_STRUCT clnt = prepareCLNT(BLOCK_MODE|USE_INTR_BUF|SEND_AUDIO_CPU,
				       AUDIO_SYSTEM, VERSION);
	HRESULT retval, *result;
	AUDIO_RPC_TSXT info;

	info.instanceID = -1; // do not care
	info.pp_Tsxt_Enable = para;

	//	if(info.ModeType > TSXT_kCSII)
	//		hr1 =  S_FALSE;

	info.ModeType = (unsigned char)(pTSXT->ModeType);
	info.TruBassGain = (unsigned char)pTSXT->TruBassGain;
	info.TruBassSize = (unsigned char)pTSXT->TruBassSize;
	info.FocusGain = (unsigned char)pTSXT->FocusGain;
	info.InputGain = (unsigned char)pTSXT->InputGain;

	info.IsTSXTEnable = pTSXT->IsTSXTEnable;

	info.IsHeadphoneMode = (unsigned char)pTSXT->IsHeadphoneMode;
	info.IsTruBassEnable = (unsigned char)pTSXT->IsTruBassEnable;
	info.IsFocusEnable = (unsigned char)pTSXT->IsFocusEnable;

	info.DefinitionGain = (unsigned char)pTSXT->DefinitionGain;
	info.IsDefEnable = (unsigned char)pTSXT->IsDefEnable;

	result = (HRESULT *)AUDIO_RPC_ToAgent_PP_TSXT_Config_0(&info, &clnt);
	retval = *result;
	free(result);
	return retval;
	//#else
	//	return AIO_OK;
	//#endif

#if 0
	SWP_SET_INFO	swp_info;
	APP_TSXT_CFG	t_tsxt_cfg;

	/*
	 * In this version, TrusurroundHD is implemented by TrusurroundXT upgrade version.
	 */
	swp_info.on_off   = para;
	swp_info.type     = SWP_CMD_SET_SRS_TSXT;
	swp_info.sub_type = type_id;

	if(p_cfg_tshd == NULL) {
		t_tsxt_cfg.ModeType = 8; //TSXT_kPassiveMatrix;
		t_tsxt_cfg.InputGain = 100; // 85;
		t_tsxt_cfg.FocusGain = 100; //30;
		t_tsxt_cfg.TruBassGain = 100; //20;
		t_tsxt_cfg.TruBassSize = 3; //TSXT_kSpeakerLFResponse150Hz;// 100Hz
		t_tsxt_cfg.IsTSXTEnable = 1;
		t_tsxt_cfg.IsFocusEnable = 1;
		t_tsxt_cfg.IsTruBassEnable = 1;
		t_tsxt_cfg.IsHeadphoneMode = 0;
		t_tsxt_cfg.DefinitionGain = 40;
		t_tsxt_cfg.IsDefEnable = 1;
		t_tsxt_cfg.Volume_compensate = 0;

		swp_info.p_coef = (void*)&t_tsxt_cfg;
	} else {
		swp_info.p_coef = p_cfg_tshd;
	}

	ioctl(audio_fd, SWP_CMD_SET_SRS_TSXT, (void*)&swp_info);
#endif

	return AIO_OK;
}

int32_t Audio_SwpSetSRS_WOW(bool para, uint32_t type_id, void *p_cfg_wow)
{
#if 0
	SWP_SET_INFO	swp_info;

	swp_info.on_off   = para;
	swp_info.type     = SWP_CMD_SET_SRS_WOW;
	swp_info.sub_type = type_id;
	swp_info.p_coef   = p_cfg_wow;

	ioctl(audio_fd, SWP_CMD_SET_SRS_WOW, (void*)&swp_info);
#endif

	return AIO_OK;
}

int32_t Audio_SwpSetBBE(bool para, uint32_t type_id, void *p_cfg_bbe)
{
#if 0
	SWP_SET_INFO	swp_info;

	swp_info.on_off   = para;
	swp_info.type     = SWP_CMD_SET_BBE;
	swp_info.sub_type = type_id;
	swp_info.p_coef   = p_cfg_bbe;

	ioctl(audio_fd, SWP_CMD_SET_BBE, (void*)&swp_info);
#endif

	return AIO_OK;
}

int32_t Audio_SwpSetRVB(bool para, uint32_t type_id, void *p_cfg_rvb)
{
#if 0
	SWP_SET_INFO	swp_info;

	swp_info.on_off   = AIO_DISABLE;
	swp_info.type     = SWP_CMD_SET_RVB;
	swp_info.sub_type = type_id;
	swp_info.p_coef   = p_cfg_rvb;

	ioctl(audio_fd, SWP_CMD_SET_RVB, (void*)&swp_info);

	if(para == AIO_ENABLE) {
		swp_info.on_off   = AIO_ENABLE;
		swp_info.type     = SWP_CMD_SET_RVB;
		swp_info.sub_type = type_id;
		swp_info.p_coef   = p_cfg_rvb;

		ioctl(audio_fd, SWP_CMD_SET_RVB, (void*)&swp_info);
	}
#endif

	return AIO_OK;
}

int32_t Audio_SwpSetRTSURD(bool para, uint32_t type_id, void *p_cfg_rtsurd)
{
#if 0
	SWP_SET_INFO	swp_info;

	swp_info.on_off   = para;
	swp_info.type     = SWP_CMD_SET_RTSURD;
	swp_info.sub_type = type_id;
	swp_info.p_coef   = p_cfg_rtsurd;

	ioctl(audio_fd, SWP_CMD_SET_RTSURD, (void*)&swp_info);
#endif

	return AIO_OK;
}

int32_t Audio_SwpSetMonitor(bool para)
{
#if 0
	uint32_t  rdata[2];

	if(p_aout_element == NULL) {
		alog("[AUDIO][APP] fail to SetMonitor\n");
		return AIO_ERROR;
	}

	m_monitor = (para > 0)? AIO_ENABLE : AIO_DISABLE;

	alog("[AUDIO][APP] ======== set audio spectrum ========\n");

	Audio_ElmControl(p_aout_element, AOUT_CTRL_CMD_GET_SPECTRUM, &m_monitor, 1, rdata, 1);

	Audio_AmwConvertPhy2VirAddr(rdata[0], (uint32_t*)&m_spectrum_info);

	alog("[AUDIO][APP] phy spectrum_ptr = 0x%08x\n",rdata[0]);
	alog("[AUDIO][APP] vir spectrum_ptr = 0x%08x\n",(uint32_t)m_spectrum_info);
#endif

	return AIO_OK;
}

int32_t Audio_SwpGetMonitorInfo(APP_MONI_MODE mode, APP_MONI_INFO *p_moni_info)
{
	return AIO_OK;
}
#endif

int32_t Audio_AppGetCapability(APP_ID id, uint32_t *p_list)
{
	uint32_t i,
		 j;

	if (id == APP_ID_CAPABILITY)
		*p_list = m_capability[32];
	else {
		for (i = 0,j = 1; i < 32; i++) {
			if (id&j) {
				*p_list = m_capability[i];
				break;
			} else {
				j <<= 1;
			}
		}
	}

	return AIO_OK;
}

int32_t Audio_AppSemInit(void)
{
	return 0;
}

void  Audio_AppSemDeInit(void)
{
	return;
}


#ifdef USE_SMOOTHEQ_THREAD
//static DECLARE_WAIT_QUEUE_HEAD(audio_app_wait);
static struct task_struct *audio_app_thread = NULL;
static int AppThread(void *arg);
#endif /* USE_PACFIC_STYLE */

int32_t Audio_AppInit(void)
{
	audio_hw_process_control_RBUS            HWP_CTRL_REG;
//	audio_lrrr_digital_volume_control_1_RBUS TS_DVOL_CTRL1_REG;
	audio_adfilter_src_4_reg_RBUS	AUDIO_ADFILTER_SRC_4_REG;
	#if !(defined(RTD2842) || defined(RTD2874))
	audio_pll_64512_detect_RBUS             PLL_64512_DETECT_REG;
	audio_pll_64512_1_detect_RBUS           PLL_64512_1_DETECT_REG;
	#endif
	int32_t                                  i,
						 j;

	if (m_app_inited)
		return AIO_OK;
	
	//	uint32_t	flag = 0;
if(!g_runtime_state)
{
#if 0	
	for (i = 0; i < APP_EQ_BAND_NUM; i++) {
		k_eq_curr_coef[i].a1 = 0;
		k_eq_curr_coef[i].a2 = 0;
		k_eq_curr_coef[i].h0 = 0;
	}

	memset((void *)&k_treble_cfg, 0, sizeof(APP_TREBLE_CFG));
	memset((void *)&k_treble_val, 0, sizeof(APP_TREBLE_VAL));
	memset((void *)&k_treble_curr_coef, 0, sizeof(APP_EQ_COEF));
	memset((void *)&k_bass_cfg, 0, sizeof(APP_BASS_CFG));
	memset((void *)&k_bass_val, 0, sizeof(APP_BASS_VAL));
	memset((void *)&k_bass_curr_coef, 0, sizeof(APP_EQ_COEF));

	memset((void *)&k_eq_cfg, 0, sizeof(APP_EQ_CFG));
	memset((void *)&k_eq_val, 0, sizeof(APP_EQ_VAL));
	//	memset((void*)&k_spk_cfg, 0,sizeof(APP_SPK_CFG));
	//	memset((void*)&k_moni_info, 0,sizeof(APP_MONI_INFO));

	memset((void *)&k_freqEQ_info, 0, sizeof(AUDIO_EQ_INFO));

	alog("[AUDIO][APP] init app driver -- start ----\n");

	Audio_HwpWriteRegister(AUDIO_LR_Loud_lp_reg, 0);
	Audio_HwpWriteRegister(AUDIO_LR_Loud_bp1_1_reg, 0);
	Audio_HwpWriteRegister(AUDIO_LR_Loud_bp2_1_reg, 0);
	Audio_HwpWriteRegister(AUDIO_CSW_Loud_lp_reg, 0);
	Audio_HwpWriteRegister(AUDIO_CSW_Loud_bp1_1_reg, 0);
	Audio_HwpWriteRegister(AUDIO_CSW_Loud_bp2_1_reg, 0);
	Audio_HwpWriteRegister(AUDIO_LSRS_Loud_lp_reg, 0);
	Audio_HwpWriteRegister(AUDIO_LSRS_Loud_bp1_1_reg, 0);
	Audio_HwpWriteRegister(AUDIO_LSRS_Loud_bp2_1_reg, 0);
	audio_hw_usleep(200);
	alog("[AUDIO][APP] clear loud value\n");
#endif
	alog("[AUDIO][APP] init app driver -- start ----\n");
	m_balance = 0;
	m_balance_lr    = 0;    // center point
	m_balance_lsrs  = 0;    // center point
	m_balance_csw   = 0;    // center point

	m_balance_ts_lr = 0;    // center point

	g_mute_id_pb = USR_MUTE_NUM;
	g_mute_id_ts = USR_MUTE_NUM;
	for (i = 0; i < APP_MUTE_NUM; i++) {
		m_mute[i] = 0;   // unmute
		m_mute_ts[i] = 0;   // unmute
	}

	for (i = 0; i < APP_CH_NUM; i++) {
		for (j = 0; j < APP_DVOL_ID_NUM; j++) {
			m_volume_comp[j][i] = 0;
		}
	}

	// HWP init. , this part may move to device_driver init.
	HWP_CTRL_REG.regValue = Audio_HwpReadRegister(AUDIO_HW_Process_Control_reg);
	//HWP_CTRL_REG.basic_premium_sel  = 0;	// ros will init
	HWP_CTRL_REG.dvol_enable        = AIO_ENABLE;
	//HWP_CTRL_REG.deq_enable         = AIO_ENABLE;//remove
	//HWP_CTRL_REG.loud_enable        = AIO_ENABLE;//remove
	HWP_CTRL_REG.spatial_ctrlenable = AIO_ENABLE;
	Audio_HwpWriteRegister(AUDIO_HW_Process_Control_reg, HWP_CTRL_REG.regValue);

//	TS_DVOL_CTRL1_REG.regValue = Audio_HwpReadRegister(AUDIO_LRRR_Digital_Volume_Control_1_reg);
//	TS_DVOL_CTRL1_REG.lrrr_dvol_enable = AIO_ENABLE;
//	Audio_HwpWriteRegister(AUDIO_LRRR_Digital_Volume_Control_1_reg, TS_DVOL_CTRL1_REG.regValue);
	#if !(defined(RTD2842) || defined(RTD2874))
	//PLL_64_512 detection
	PLL_64512_DETECT_REG.regValue = AIO_ReadRegister(AUDIO_pll_64512_detect_reg);
	PLL_64512_1_DETECT_REG.regValue = AIO_ReadRegister(AUDIO_pll_64512_1_detect_reg);
	PLL_64512_DETECT_REG.pll_64512_detect_en = 0;
	PLL_64512_1_DETECT_REG.pll_64512_1_detect_en = 0;
	AIO_WriteRegister(AUDIO_pll_64512_detect_reg,   PLL_64512_DETECT_REG.regValue);
	AIO_WriteRegister(AUDIO_pll_64512_1_detect_reg, PLL_64512_1_DETECT_REG.regValue);
	#endif

#ifdef RTD289X
	//K3L AA filter gain = -2.49dB
	audio_analog_reserved_RBUS analog_reserved;
	analog_reserved.regValue = AIO_ReadRegister(AUDIO_analog_reserved_reg);
	analog_reserved.reg_bb_micbst_enpdrl = 1; //0:-4.22dB/-3.64dB; 1: -3.06dB/-2.49dB
	analog_reserved.reg_bb_micbst_enpdrr = 1; //0:-4.22dB/-3.64dB; 1: -3.06dB/-2.49dB
	AIO_WriteRegister(AUDIO_analog_reserved_reg, analog_reserved.regValue);

	audio_analog_ci02_RBUS analog_ci02;
	analog_ci02.regValue = AIO_ReadRegister(AUDIO_analog_CI02_reg);
	analog_ci02.reg_bb_adc_fset = 1; //0:-4.22dB/-3.06dB; 1: -3.64dB/-2.49dB
	analog_ci02.reg_bb_adc2_fset = 1; //0:-4.22dB/-3.06dB; 1: -3.64dB/-2.49dB
	AIO_WriteRegister(AUDIO_analog_CI02_reg, analog_ci02.regValue);
#endif

	// [Clayton] 2014/5/29
	// Distortion control default setting
	// Dist_Hi_TH=0dB(0x80) / Dist_Lo_TH = -0.5dB(0x79)
	// Dist control attack time ~ 34msec for +5dB to -0.5dB
	// [start]
	//Audio_HwpSetDistortionCtrl(AIO_ENABLE, &m_cfg_dist);
	alog("Audio_HwpSetDistortionCtrl Disable\n");
	Audio_HwpSetDistortionCtrl(AIO_DISABLE, &m_cfg_dist);
	alog_debug("[AUDIO][APP] Distortion control default setting: \n");
	alog_debug("[AUDIO][APP] Dist_Hi_TH=%x ; Dist_Lo_TH=%x \n", m_cfg_dist.dist_high_thre, m_cfg_dist.dist_low_thre);

	// [end]
	

	AUDIO_ADFILTER_SRC_4_REG.regValue = AIO_ReadRegister(AUDIO_adfilter_src_4_reg_reg);
	AUDIO_ADFILTER_SRC_4_REG.adc4_src_en = 0; //Disable ADC SRC
	AIO_WriteRegister(AUDIO_adfilter_src_4_reg_reg, AUDIO_ADFILTER_SRC_4_REG.regValue);
	//Add for Calibrate
	Audio_HwpCalibrateADDA();

	Audio_HwpCfgVolume(&m_cfg_dvol);
	Audio_HwpCfgVolumeTS(&m_cfg_dvol_ts);
	Audio_HwpCfgMute(&m_cfg_mute);
	Audio_HwpCfgMuteTS(&m_cfg_mute_ts);
}else
	alog("[AUDIO][APP] resume from runtime powerdown, skip variable initial\n");
	m_app_inited = true;
	alog("[AUDIO][APP] init app driver -- end ------\n");
	// ryanliao 2015/2/24
	// Read OTP value of Audio Output compensation
	//GetAudioOutputFromOTP();
	//printf("[Audio][Debug]Audio_GetAudioOutputFromOTP \n");
	// [debug test]start
	//Audio_EnableCompensateAudioOutput(1, PB_TS_path);
	// end



#ifndef BUILD_QUICK_SHOW
#ifdef USE_SMOOTHEQ_THREAD
	if (IS_ERR_OR_NULL(audio_app_thread)) {
		//AppThreadState = false;
		audio_app_thread = kthread_run(AppThread, NULL, "AppThread%d", 0);
		if (IS_ERR(audio_app_thread))
			alog_err("audio_app_thread creation failed\n");
		else
			alog_info("audio_app_thread creation ok\n");
	}else
		alog_err("audio_app_thread already exist! bypass kthread_run");
#endif
#endif /* !BUILD_QUICK_SHOW */
	return AIO_OK;
}
EXPORT_SYMBOL(Audio_AppInit);

#ifndef BUILD_QUICK_SHOW
#ifdef USE_SMOOTHEQ_THREAD
int AppThread(void *arg)
{
	unsigned int i=0;
	alog_info("AppThread ceate...\n");

	(void)arg;  /* suppress compile warning */


	set_freezable();

	do {
//EQ		
		for(i=0;i<=APP_EQ_ALL_BPF_BAND14;i++)
		{			
			if(g_HwpSetEQ_Smooth_H0_En[i]==1)
				Audio_HwpSetEQ_Smooth_H0_Run(d[i].band_id,d[i].eq_coef);
		}
		for(i=0;i<=APP_LOUD_ALL_BPF_BAND2;i++)
		{		
			if(g_HwpSetLoud_Smooth_H0_En[i]==1)
				Audio_HwpSetLoud_Smooth_H0_Run(k[i].band_id,k[i].loud_coef);
		}
		
		wait_event_freezable_timeout(audio_app_wait,
					     (kthread_should_stop()),
					     usecs_to_jiffies(APP_TICK_THREAD_TIME));

		if (down_interruptible(&audio_sem_app)) {
			alog_warning("interrupted, no semaphore held\n");
			break;
		}

		up(&audio_sem_app);	
	} while (!kthread_should_stop());

	//AtvThreadState = false;

	alog_info("AppThread exiting\n");

	return 0;
}
#endif /* USE_PACFIC_STYLE */
#endif /* !BUILD_QUICK_SHOW */


int32_t Audio_AppDeInit(void)
{
	//	Audio_ElmDelete((void*)p_aout_element);

#ifndef BUILD_QUICK_SHOW
#ifdef USE_SMOOTHEQ_THREAD
	int32_t res;

	if (!IS_ERR_OR_NULL(audio_app_thread)) {	
		res = kthread_stop(audio_app_thread);
		if (res) {
			alog_info("audio_app_thread cancellation failed %d\n", res);
			audio_app_thread = NULL;
		} else {
			alog_info("audio_app_thread cancellation ok\n");
			audio_app_thread = NULL;
		}
	}else
		alog_info("audio_app_thread IS_ERR_OR_NULL=1, bypass kthread_stop\n");
#endif
#endif /* !BUILD_QUICK_SHOW */
	m_app_inited = false;
	return AIO_OK;
}

uint32_t Audio_AppGetInfo(APP_GET_INFO info)
{
	int32_t  i;
	uint32_t ret = 0;

	switch (info) {
	case APP_INFO_MUTE:
		for (i = 0; i < APP_MUTE_NUM; i++) {
			if (m_mute[i] != 0) {
				ret = true; // in mute state
				break;
			}
		}

		if (i == APP_MUTE_NUM)
			ret = false; // in unmute state
		break;

	default:
		ret = 0;
		break;
	}

	//alog("[AUDIO][APP] info(%d) = %x\n", info, ret);
	return ret;
}

/**
 * Set Digital Volume ( Playback path )
 *
 * @param <ch_id>	{ APP_CH_ID_L / APP_CH_ID_R / ... ,refer to audio_app.h }
 * @param <vol>		{ range from APP_MIN_VOL ~ APP_MAX_VOL }
 * @return			{ AIO_OK or AIO_ERROR_CODE }
 * @ingroup lib_audio
 */
void HwpSetVolume(uint32_t ch_id, int32_t vol)
{
	audio_digital_volume_control_1_RBUS  DVOL_CTRL_REG;
	audio_lr_digital_volume_control_RBUS DVOL_SET_REG;   // assume register of L/R , Ls/Rs and C/SW is identical.
	audio_digital_volume_control_0_RBUS    DVOL_CTRL_0_REG;	
	int32_t                              a,
					     b,
					     c,
					     lr_idx        = 0;
	uint32_t                             reg_addr_set,
					     reg_addr_ctrl;


/*
	while (1) {
		DVOL_CTRL_REG.regValue = Audio_HwpReadRegister(AUDIO_Digital_Volume_Control_1_reg);
		if (DVOL_CTRL_REG.dvol_apply == 1) {
			//alog("[AUDIO][APP] dvol_apply = 1\n");
		} else {
			break;
		}

		time_out++;

		if (time_out > 0x800000) {
			alog("[AUDIO][APP] %s() TimeOut(0x%08x)!!!\n", __FUNCTION__, time_out);
			return;
		}
	}
	*/
//fix balance switch volume fast from makig noise
	DVOL_CTRL_0_REG.regValue = AIO_ReadRegister(AUDIO_Digital_Volume_Control_0_reg);
	DVOL_CTRL_0_REG.dvcm = 1; //Zero crossing
	AIO_WriteRegister(AUDIO_Digital_Volume_Control_0_reg, DVOL_CTRL_0_REG.regValue);
	//alog("[AUDIO][APP] TimeOut = %d\n",time_out);

	// 2015/2/24 ryanliao
	// Compensate Audio out mismatch from OTP, Only compensate @ Rch.
	// So, L>R ==> g_PBDVC_comp_gain>0 ; L<R ==> g_PBDVC_comp_gain<0
	// [Start]
	if (ch_id == APP_CH_ID_CH1)
		vol = vol + g_PBDVC_comp_gain;

	// [End]

	if (vol > APP_MAX_VOL)
		vol = APP_MAX_VOL;

	if (vol < APP_MIN_VOL)
		vol = APP_MIN_VOL;

	if(vol!=APP_MIN_VOL)
	{
	// change dvol setting to DVOL mode
	DVOL_CTRL_REG.regValue = Audio_HwpReadRegister(AUDIO_Digital_Volume_Control_1_reg);
	DVOL_CTRL_REG.mute_apply = false;
	Audio_HwpWriteRegister(AUDIO_Digital_Volume_Control_1_reg, DVOL_CTRL_REG.regValue);
	}
	c = vol&0x7;

	a = (vol >> 3) / 6;
	b = (vol >> 3) % 6;
	if (b < 0) {
		a--;
		b += 6;
	}

	//	alog("[AUDIO][APP] vol=%d, a=%d,b=%d,c=%d\n",vol,a,b,c);

	switch (ch_id) {
	case APP_CH_ID_CH0:
		lr_idx = 1;
		reg_addr_set = AUDIO_LR_Digital_Volume_Control_reg;
		reg_addr_ctrl = AUDIO_Digital_Volume_Control_1_reg;
		break;

	case APP_CH_ID_CH1:
		lr_idx = 0;
		reg_addr_set = AUDIO_LR_Digital_Volume_Control_reg;
		reg_addr_ctrl = AUDIO_Digital_Volume_Control_1_reg;
		break;

	case APP_CH_ID_CH2:
		lr_idx = 1;
		//modify HW CSW/LsRs naming
		reg_addr_set = AUDIO_LsRs_Digital_Volume_Control_reg;
		reg_addr_ctrl = AUDIO_Digital_Volume_Control_1_reg;
		break;

	case APP_CH_ID_CH3:
		lr_idx = 0;
		//modify HW CSW/LsRs naming
		reg_addr_set = AUDIO_LsRs_Digital_Volume_Control_reg;
		reg_addr_ctrl = AUDIO_Digital_Volume_Control_1_reg;
		break;
#if 0
	case APP_CH_ID_CH4:
		lr_idx = 1;
		//modify HW CSW/LsRs naming
		reg_addr_set = AUDIO_CSW_Digital_Volume_Control_reg;
		reg_addr_ctrl = AUDIO_Digital_Volume_Control_1_reg;
		break;

	case APP_CH_ID_CH5:
		lr_idx = 0;
		//modify HW CSW/LsRs naming
		reg_addr_set = AUDIO_CSW_Digital_Volume_Control_reg;
		reg_addr_ctrl = AUDIO_Digital_Volume_Control_1_reg;
		break;

	case APP_CH_ID_CH6:
		lr_idx = 1;
		//modify HW CSW/LsRs naming
		reg_addr_set = AUDIO_ch67_Digital_Volume_Control_reg;
		reg_addr_ctrl = AUDIO_Digital_Volume_Control_1_reg;
		break;

	case APP_CH_ID_CH7:
		lr_idx = 0;
		//modify HW CSW/LsRs naming
		reg_addr_set = AUDIO_ch67_Digital_Volume_Control_reg;
		reg_addr_ctrl = AUDIO_Digital_Volume_Control_1_reg;
		break;
#endif
	default:
		return;
	}

	DVOL_SET_REG.regValue = AIO_ReadRegister(reg_addr_set);

	if (lr_idx) {
		DVOL_SET_REG.rep_l_status = 1;
		DVOL_SET_REG.hpf_l_enable = 0; //1
		DVOL_SET_REG.vol_l_a = a;
		DVOL_SET_REG.vol_l_b = ((b << 3)|c);
	} else {
		DVOL_SET_REG.rep_r_status = 1;
		DVOL_SET_REG.hpf_r_enable = 0; //1
		DVOL_SET_REG.vol_r_a = a;
		DVOL_SET_REG.vol_r_b = (b << 3)|c;
	}

	//alog("rw : [0x%08x] = 0x%08x\n",reg_addr_set,DVOL_SET_REG.regValue);
	AIO_WriteRegister(reg_addr_set, DVOL_SET_REG.regValue);

	// is it necessary to protect application before IP finish the previous setting ?
	DVOL_CTRL_REG.regValue = Audio_HwpReadRegister(reg_addr_ctrl);
	DVOL_CTRL_REG.dvol_apply = 1;
	Audio_HwpWriteRegister(reg_addr_ctrl, DVOL_CTRL_REG.regValue);

}

/**
 * Set Digital Volume ( Time-Shift path )
 *
 * @param <ch_id>	{ APP_CH_ID_L / APP_CH_ID_R / ... ,refer to audio_app.h }
 * @param <vol>		{ range from APP_MIN_VOL ~ APP_MAX_VOL }
 * @return			{ AIO_OK or AIO_ERROR_CODE }
 * @ingroup lib_audio
 */
static void HwpSetVolumeTS(uint32_t ch_id, int32_t vol)
{
	//2851c/2885p no support ts dvc

}

#ifdef SUPPORT_ADC_DVC
/**
 * Set ADC1  Digital Volume
 *
 * @param <vol>		{ step = 0.125, range from APP_MIN_VOL ~ APP_MAX_VOL }
 */
void HwpSetVolume_ADC1(uint32_t adc1_ch_id, int32_t vol)
{
	audio_adc_digital_volume_control_1_RBUS  DVOL_CTRL_REG;
	audio_adc_lr_digital_volume_control_RBUS DVOL_SET_REG;
	int32_t                                  a,
						 b,
						 c;



	vol += g_adc1_offset;

/*
	while (1) {
		DVOL_CTRL_REG.regValue = Audio_HwpReadRegister(AUDIO_ADC_Digital_Volume_Control_1_reg);
		if (DVOL_CTRL_REG.dvol_apply == 1) {
			//alog("[AUDIO][APP] dvol_apply = 1\n");
		} else {
			break;
		}

		time_out++;

		if (time_out > 0x80000) {
			alog("[AUDIO][APP] %s() TimeOut(0x%08x)!!!\n", __FUNCTION__, time_out);
			return;
		}
	}
	*/

	//alog("[AUDIO][APP] TimeOut = %d\n",time_out);
	// change dvol setting to DVOL mode
	DVOL_CTRL_REG.regValue = Audio_HwpReadRegister(AUDIO_ADC_Digital_Volume_Control_1_reg);
	DVOL_CTRL_REG.mute_apply = false;
	Audio_HwpWriteRegister(AUDIO_ADC_Digital_Volume_Control_1_reg, DVOL_CTRL_REG.regValue);

	if (vol > APP_MAX_VOL)
		vol = APP_MAX_VOL;

	if (vol < APP_MIN_VOL)
		vol = APP_MIN_VOL;

	c = vol&0x7;

	a = (vol >> 3) / 6;
	b = (vol >> 3) % 6;
	if (b < 0) {
		a--;
		b += 6;
	}

	alog_debug("[AUDIO][APP] adc1_vol=%d, a=%d,b=%d,c=%d\n", vol, a, b, c);
	DVOL_SET_REG.regValue = Audio_HwpReadRegister(AUDIO_ADC_LR_Digital_Volume_Control_reg);

	//left channel
	if (adc1_ch_id & APP_ADC_CH_ID_L) {
		DVOL_SET_REG.rep_l_status = 1;
		DVOL_SET_REG.hpf_l_enable = 0; //1
		DVOL_SET_REG.mute_l = 0;
		DVOL_SET_REG.vol_l_a = a;
		DVOL_SET_REG.vol_l_b = (b << 3)|c;
	}

	//right channel
	if (adc1_ch_id & APP_ADC_CH_ID_R) {
		DVOL_SET_REG.rep_r_status = 1;
		DVOL_SET_REG.hpf_r_enable = 0; //1
		DVOL_SET_REG.mute_r = 0;
		DVOL_SET_REG.vol_r_a = a;
		DVOL_SET_REG.vol_r_b = (b << 3)|c;
	}

	Audio_HwpWriteRegister(AUDIO_ADC_LR_Digital_Volume_Control_reg, DVOL_SET_REG.regValue);


	// is it necessary to protect application before IP finish the previous setting ?
	DVOL_CTRL_REG.regValue = Audio_HwpReadRegister(AUDIO_ADC_Digital_Volume_Control_1_reg);
	DVOL_CTRL_REG.dvol_apply = 1;
	Audio_HwpWriteRegister(AUDIO_ADC_Digital_Volume_Control_1_reg, DVOL_CTRL_REG.regValue);

}

/**
 * Set ADC2  Digital Volume
 *
 * @param <vol>		{ step = 0.125, range from APP_MIN_VOL ~ APP_MAX_VOL }
 */
static void HwpSetVolume_ADC2(uint32_t adc2_ch_id, int32_t vol)
{
#if defined(SUPPORT_2_SET_ADC) && defined(SUPPORT_ADC_DVC)
	audio_adc_digital_volume_control_1_RBUS    DVOL_CTRL_REG;
	audio_adc_double_buffer_ack_1_RBUS         ADC2_CTRL_REG; //adc2
	audio_adc_lsrs_digital_volume_control_RBUS DVOL_SET_REG; //adc2
	int32_t                                    a,
						   b,
						   c,
						   lr_idx        = 0;
	uint32_t                                   time_out      = 0;

	vol += g_adc2_offset;
	/*
	while (1) {
		ADC2_CTRL_REG.regValue = Audio_HwpReadRegister(AUDIO_ADC_Double_buffer_ack_1_reg);
		if (ADC2_CTRL_REG.dvol_apply_1 == 1) {
			//alog("[AUDIO][APP] dvol_apply = 1\n");
		} else {
			break;
		}

		time_out++;

		if (time_out > 0x80000) {
			alog("[AUDIO][APP] %s() TimeOut(0x%08x)!!!\n", __FUNCTION__, time_out);
			return;
		}
	}
	*/

	//alog("[AUDIO][APP] TimeOut = %d\n",time_out);

	// change dvol setting to DVOL mode
	DVOL_CTRL_REG.regValue = Audio_HwpReadRegister(AUDIO_ADC_Digital_Volume_Control_1_reg);
	DVOL_CTRL_REG.mute_apply = false;
	Audio_HwpWriteRegister(AUDIO_ADC_Digital_Volume_Control_1_reg, DVOL_CTRL_REG.regValue);


	if (vol > APP_MAX_VOL)
		vol = APP_MAX_VOL;

	if (vol < APP_MIN_VOL)
		vol = APP_MIN_VOL;

	c = vol&0x7;

	a = (vol >> 3) / 6;
	b = (vol >> 3) % 6;
	if (b < 0) {
		a--;
		b += 6;
	}

	alog("[AUDIO][APP] adc1_vol=%d, a=%d,b=%d,c=%d\n", vol, a, b, c);

	DVOL_SET_REG.regValue = Audio_HwpReadRegister(AUDIO_ADC_LsRs_Digital_Volume_Control_reg);

	//left channel
	if (adc2_ch_id == APP_ADC_CH_ID_L) {
		DVOL_SET_REG.rep_ls_status = 1;
		DVOL_SET_REG.hpf_ls_enable = 0; //1
		DVOL_SET_REG.mute_ls = 0;
		DVOL_SET_REG.vol_ls_a = a;
		DVOL_SET_REG.vol_ls_b = (b << 3)|c;
	}

	//right channel
	if (adc2_ch_id == APP_ADC_CH_ID_R) {
		DVOL_SET_REG.rep_rs_status = 1;
		DVOL_SET_REG.hpf_rs_enable = 0; //1
		DVOL_SET_REG.mute_rs = 0;
		DVOL_SET_REG.vol_rs_a = a;
		DVOL_SET_REG.vol_rs_b = (b << 3)|c;
	}

	Audio_HwpWriteRegister(AUDIO_ADC_LsRs_Digital_Volume_Control_reg, DVOL_SET_REG.regValue);


	// is it necessary to protect application before IP finish the previous setting ?
	ADC2_CTRL_REG.regValue = Audio_HwpReadRegister(AUDIO_ADC_Double_buffer_ack_1_reg);
	ADC2_CTRL_REG.dvol_apply_1 = 1;
	Audio_HwpWriteRegister(AUDIO_ADC_Double_buffer_ack_1_reg, ADC2_CTRL_REG.regValue);
#endif
}

#endif
/**
 * Set ADC gain
 *
 * @param <gain>		{range: -17.625~30 dB, 0.375dB per step}
 */
static void HwpSetGain_ADC(uint32_t adc_ch_id, int32_t gain)
{
	audio_adfilter_src_4_reg_RBUS ADC_again;
	int32_t                reg_gain;

	gain += g_adc_dig_offset;
	if (gain > 0x50)
		gain = 0x50;

	if (gain < -0x2f)
		gain = -0x2f;

	reg_gain = 0x2f + gain;
	ADC_again.regValue = Audio_HwpReadRegister(AUDIO_adfilter_src_4_reg_reg);
	if (adc_ch_id&APP_ADC_CH_ID_L) {
		ADC_again.adc4_ad_gain_l = reg_gain;
	}

	if (adc_ch_id&APP_ADC_CH_ID_R) {
		ADC_again.adc4_ad_gain_r = reg_gain;
	}

	Audio_HwpWriteRegister(AUDIO_adfilter_src_4_reg_reg, ADC_again.regValue);

}


/**
 * Set DAC0 gain
 *
 * @param <gain>		{range: -65.25~0 dB, 0.75dB per step}
 */
 #if !(defined(RTD2874))
static void HwpSetGain_DAC0(uint32_t dac0_ch_id, int32_t gain)
{
	audio_da0_asrc_ip_RBUS DAC0_again;
	int32_t                reg_gain;

	gain += g_dac0_offset;
	if (gain > 0)
		gain = 0;

	if (gain < -87)
		gain = -87;

	reg_gain = 0x57 + gain;
	DAC0_again.regValue = Audio_HwpReadRegister(AUDIO_da0_asrc_ip_reg);
	if (dac0_ch_id&APP_DAC_CH_ID_L) {
		DAC0_again.da0_gain_l = reg_gain;
	}

	if (dac0_ch_id&APP_DAC_CH_ID_R) {
		DAC0_again.da0_gain_r = reg_gain;
	}

	Audio_HwpWriteRegister(AUDIO_da0_asrc_ip_reg, DAC0_again.regValue);

}
 #else
static void HwpSetGain_DAC0(uint32_t dac0_ch_id, int32_t gain)
{
	audio_dafilter_src_reg_1_RBUS DAC0_again;
	int32_t                reg_gain;

	gain += g_dac0_offset;
	if (gain > 0)
		gain = 0;

	if (gain < -87)
		gain = -87;

	reg_gain = 0xAF + gain;
	DAC0_again.regValue = Audio_HwpReadRegister(AUDIO_dafilter_src_reg_1_reg);
	if (dac0_ch_id&APP_DAC_CH_ID_L) {
		DAC0_again.dac0_gain_l = reg_gain;
	}

	if (dac0_ch_id&APP_DAC_CH_ID_R) {
		DAC0_again.dac0_gain_r = reg_gain;
	}

	Audio_HwpWriteRegister(AUDIO_dafilter_src_reg_1_reg, DAC0_again.regValue);

}
 #endif

/**
 * Set DAC1 gain
 *
 * @param <gain>		{range: -65.25~0 dB, 0.75dB per step}
 */
#if !(defined(RTD2874))
static void HwpSetGain_DAC1(uint32_t dac1_ch_id, int32_t gain)
{
	audio_da1_asrc_ip_RBUS DAC1_again;
	int32_t                reg_gain;

	gain += g_dac1_offset;
	if (gain > 0)
		gain = 0;

	if (gain < -87)
		gain = -87;

	reg_gain = 0x57 + gain;
	DAC1_again.regValue = Audio_HwpReadRegister(AUDIO_da1_asrc_ip_reg);
	if (dac1_ch_id&APP_DAC_CH_ID_L) {
		DAC1_again.da1_gain_l = reg_gain;
	}

	if (dac1_ch_id&APP_DAC_CH_ID_R) {
		DAC1_again.da1_gain_r = reg_gain;
	}

	Audio_HwpWriteRegister(AUDIO_da1_asrc_ip_reg, DAC1_again.regValue);

}
#else
static void HwpSetGain_DAC1(uint32_t dac1_ch_id, int32_t gain)
{
	audio_dafilter_src_reg_1_RBUS DAC1_again;
	int32_t                reg_gain;

	gain += g_dac1_offset;
	if (gain > 0)
		gain = 0;

	if (gain < -87)
		gain = -87;

	reg_gain = 0xAF + gain;
	DAC1_again.regValue = Audio_HwpReadRegister(AUDIO_dafilter_src_reg_1_reg);
	if (dac1_ch_id&APP_DAC_CH_ID_L) {
		DAC1_again.dac1_gain_l = reg_gain;
	}

	if (dac1_ch_id&APP_DAC_CH_ID_R) {
		DAC1_again.dac1_gain_r = reg_gain;
	}

	Audio_HwpWriteRegister(AUDIO_dafilter_src_reg_1_reg, DAC1_again.regValue);

}
#endif

#ifdef SUPPORT_ADC_DVC
/**
 * Enable ADC1  Digital Volume
 *
 * @param <enable>		{true: enable ADC1 DVC}
 */
static void HwpEnableVolume_ADC1(bool enable)
{
	audio_adc_digital_volume_control_0_RBUS ADC1_DVC_0_REG;

	ADC1_DVC_0_REG.regValue = Audio_HwpReadRegister(AUDIO_ADC_Digital_Volume_Control_0_reg);
	ADC1_DVC_0_REG.adc_dvol_enable = enable;
	ADC1_DVC_0_REG.adc4_l_inverter_en = 1;
	ADC1_DVC_0_REG.adc4_r_inverter_en = 1;
	Audio_HwpWriteRegister(AUDIO_ADC_Digital_Volume_Control_0_reg, ADC1_DVC_0_REG.regValue);
}

/**
 * Enable ADC2  Digital Volume
 *
 * @param <enable>		{true: enable ADC2 DVC}
 */
static void HwpEnableVolume_ADC2(bool enable)
{
#if defined(SUPPORT_2_SET_ADC) && defined(SUPPORT_ADC_DVC)
	audio_adc_double_buffer_ack_1_RBUS ADC2_DVC_0_REG;

	ADC2_DVC_0_REG.regValue = Audio_HwpReadRegister(AUDIO_ADC_Double_buffer_ack_1_reg);
	ADC2_DVC_0_REG.adc_dvol_enable_1 = enable;
	Audio_HwpWriteRegister(AUDIO_ADC_Double_buffer_ack_1_reg, ADC2_DVC_0_REG.regValue);
#endif
}
#endif//ifdef SUPPORT_ADC_DVC
////////////////////////////////////////////////////////////////////
#if defined(FREQ_EQ_FEEDBACK)
#if defined(FREQ_EQ_CALIBRATION)

static double bpfilter_response(long freq, double a1_float, double a2_float, double h0_float)
{
	double bpfilter_mag;
	double bpa1_coefficient_rounding;
	double bpa2_coefficient_rounding;
	double bph0_coefficient_rounding;
	double w;
	float  pi                        = 3.14159265;

	w = 2 * pi * freq / 48000;

	bpa1_coefficient_rounding = a1_float;
	bpa2_coefficient_rounding = a2_float;
	bph0_coefficient_rounding = h0_float;

	bpfilter_mag = (sqrt(pow(1 + bph0_coefficient_rounding * (1 - bpa2_coefficient_rounding) + bpa1_coefficient_rounding * cos(w) +
				 (bpa2_coefficient_rounding + bph0_coefficient_rounding * bpa2_coefficient_rounding - bph0_coefficient_rounding) * cos(2 * w), 2) +
			     pow(bpa1_coefficient_rounding * sin(w) + (bpa2_coefficient_rounding + bph0_coefficient_rounding * bpa2_coefficient_rounding - bph0_coefficient_rounding) * sin(2 * w), 2))) /
		       (sqrt(pow(1 + bpa1_coefficient_rounding * cos(w) + bpa2_coefficient_rounding * cos(2 * w), 2) +
			     pow(bpa1_coefficient_rounding * sin(w) + bpa2_coefficient_rounding * sin(2 * w), 2)));

	return bpfilter_mag;
}
#endif

static void HwpCalEQInfo(APP_EQ_COEF *k_eq_coef, int eq_loud_type)
{

	int   hexBPF_H0,
	      hexBPF_a1,
	      hexBPF_a2,
	      hexBPF_db;
	int   hexLP_H0,
	      hexLP_a1,
	      hexLP_db;
	int   hexHP_a1,
	      hexHP_H0,
	      hexHP_db;
	float fc;
	float d,
	      BPF_H0,
	      BPF_a1,
	      BPF_a2,
	      BPF_db,
	      BPF_bw;
	float LP_H0,
	      LP_a1,
	      LP_db,
	      LP_bw;
	float HP_a1,
	      HP_bw,
	      HP_db;
	float BPF_H0_arr[APP_EQ_BAND_NUM]  = {0};
	float BPF_a1_arr[APP_EQ_BAND_NUM]  = {0};
	float BPF_a2_arr[APP_EQ_BAND_NUM]  = {0};
	float BPF_db_arr[APP_EQ_BAND_NUM]  = {0};
	int   fs          = 48000;
	int   *p_cfg;
	int   band_num;
	float pi          = 3.14159265;
	int   i;

	int   cfg_eq[6 * BAND_INFO_SIZE + 1]      =
	       {   //band_num,(band_idx,band_type),...
		       6,
		       0, APP_FREQ_EQ_LPF_TYPE,
		       1, APP_FREQ_EQ_BPF_TYPE,
		       2, APP_FREQ_EQ_BPF_TYPE,
		       3, APP_FREQ_EQ_BPF_TYPE,
		       4, APP_FREQ_EQ_BPF_TYPE,
		       5, APP_FREQ_EQ_BPF_TYPE
	       };
	int   cfg_bass[1 * BAND_INFO_SIZE + 1]    =
	     {   //band_num,(band_idx,band_type),...
		     1, APP_EQ_BASS_BAND_ID, APP_FREQ_EQ_LPF_TYPE
	     };
	int   cfg_treble[1 * BAND_INFO_SIZE + 1]  =
	   {   //band_num,(band_idx,band_type),...
		   1, APP_EQ_TREBLE_BAND_ID, APP_FREQ_EQ_BPF_TYPE
	   };
	int   cfg_treble2[1 * BAND_INFO_SIZE + 1] =
	{   //band_num,(band_idx,band_type),...
		1, APP_EQ_TREBLE2_BAND_ID, APP_FREQ_EQ_BPF_TYPE
	};

	k_freqEQ_info.valid_mask = 0;

	if (eq_loud_type == APP_EQ_TYPE_ID) {
		p_cfg = cfg_eq;
	} else if (eq_loud_type == APP_BASS_TYPE_ID) {
		p_cfg = cfg_bass;
	} else if (eq_loud_type == APP_TREBLE_TYPE_ID) {
		p_cfg = cfg_treble;
	} else {
		p_cfg = cfg_treble2;
	}

	band_num = *p_cfg++;
	for (i = 0; i < band_num; i++) {
		if (p_cfg[i * BAND_INFO_SIZE + 1] == APP_FREQ_EQ_BPF_TYPE) {
			if (eq_loud_type == APP_EQ_TYPE_ID) {
				hexBPF_H0 = k_eq_coef[p_cfg[i * BAND_INFO_SIZE]].h0&0x0000ffff;
				hexBPF_a1 = k_eq_coef[p_cfg[i * BAND_INFO_SIZE]].a1&0x0000ffff;
				hexBPF_a2 = k_eq_coef[p_cfg[i * BAND_INFO_SIZE]].a2&0x0000ffff;
			} else {
				hexBPF_H0 = k_eq_coef->h0&0x0000ffff;
				hexBPF_a1 = k_eq_coef->a1&0x0000ffff;
				hexBPF_a2 = k_eq_coef->a2&0x0000ffff;
			}

			if (hexBPF_H0&SIGN_INT16) {
				BPF_H0 = (float)hexBPF_H0 / Q412_ONE - 16;    // 2^4 = 16
			} else {
				BPF_H0 = (float)hexBPF_H0 / Q412_ONE;
			}

			BPF_db = log10(2 * BPF_H0 + 1) * 20;
			if (hexBPF_a1&SIGN_INT16) {
				BPF_a1 = (float)(hexBPF_a1 - 1) / Q214_ONE - 4; // 2^2 = 4
			} else {
				BPF_a1 = (float)(hexBPF_a1 - 1) / Q214_ONE;
			}

			if (hexBPF_a2&SIGN_INT16) {
				BPF_a2 = (float)hexBPF_a2 / Q214_ONE - 4; // 2^2 = 4
			} else {
				BPF_a2 = (float)hexBPF_a2 / Q214_ONE;
			}

			// the following equations refer from DAFX ch-2.3.2 peak filter
			if (BPF_db >= 0) {
				BPF_bw = atan((1 - BPF_a2) / (1 + BPF_a2)) * fs / pi;
			} else {
				BPF_bw = atan((1 - BPF_a2) * pow(10, (BPF_db / 20)) / (1 + BPF_a2)) * fs / pi;
			}

			BPF_H0_arr[p_cfg[i * BAND_INFO_SIZE]] = BPF_H0;
			BPF_a1_arr[p_cfg[i * BAND_INFO_SIZE]] = BPF_a1;
			BPF_a2_arr[p_cfg[i * BAND_INFO_SIZE]] = BPF_a2;
			BPF_db_arr[p_cfg[i * BAND_INFO_SIZE]] = BPF_db;
			d = BPF_a1 / (1 + BPF_a2);
			fc = (float)acos(-BPF_a1 / (1 + BPF_a2)) * fs / (2 * pi);
			//alog("hexBPF_H0: %x\n", hexBPF_H0);
			//alog("hexBPF_a1: %x\n", hexBPF_a1);
			//alog("hexBPF_a2: %x\n", hexBPF_a2);
			//alog("[BPF%d]db: %2.8f, bw: %2.8f, fc: %4.8f\n", p_cfg[i*BAND_INFO_SIZE], BPF_db, BPF_bw, fc);
			hexBPF_db = BPF_db * Q824_ONE; // float --> Q8.24
						       //hexBPF_bw = BPF_bw * 8388608; //Q9.23
						       //hexFc = fc * 4194304; //Q10.22

			k_freqEQ_info.fc[p_cfg[i * BAND_INFO_SIZE]] = fc + 0.5;  // rounding
			k_freqEQ_info.bw[p_cfg[i * BAND_INFO_SIZE]] = BPF_bw + 0.5; // rounding
			k_freqEQ_info.gain[p_cfg[i * BAND_INFO_SIZE]] = hexBPF_db;
			k_freqEQ_info.type[p_cfg[i * BAND_INFO_SIZE]] = APP_FREQ_EQ_BPF_TYPE; // BPF type
			k_freqEQ_info.valid_mask = k_freqEQ_info.valid_mask|(0x1 << p_cfg[i * BAND_INFO_SIZE]);
			//alog("[BPF%d]hex_db: %x, hex_bw: %x, hex_fc: %x\n", p_cfg[i*BAND_INFO_SIZE], k_freqEQ_info.gain[p_cfg[i*BAND_INFO_SIZE]], k_freqEQ_info.bw[p_cfg[i*BAND_INFO_SIZE]], k_freqEQ_info.fc[p_cfg[i*BAND_INFO_SIZE]]);

		} else if (p_cfg[i * BAND_INFO_SIZE + 1] == APP_FREQ_EQ_LPF_TYPE) {  //cal LPF
			if (eq_loud_type == APP_EQ_TYPE_ID) {
				// multi-bands
				hexLP_H0 = k_eq_coef[p_cfg[i * BAND_INFO_SIZE]].h0&0x0000ffff;
				hexLP_a1 = k_eq_coef[p_cfg[i * BAND_INFO_SIZE]].a1&0x0000ffff;
			} else {
				// bass or treble only single band
				hexLP_H0 = k_eq_coef->h0&0x0000ffff;
				hexLP_a1 = k_eq_coef->a1&0x0000ffff;
			}

			if (hexLP_H0&SIGN_INT16) {
				LP_H0 = (float)hexLP_H0 / Q412_ONE - 16; // 2^4 = 16
			} else {
				LP_H0 = (float)hexLP_H0 / Q412_ONE; // 2^4 = 16
			}

			// the following equations refer from DAFX ch-2.3.1 shelving filter
			LP_db = log10(2 * LP_H0 + 1) * 20;
			if (hexLP_a1&SIGN_INT16) {
				LP_a1 = (float)hexLP_a1 / Q214_ONE - 4; // 2^2 = 4
			} else {
				LP_a1 = (float)hexLP_a1 / Q214_ONE; // 2^2 = 4
			}

			if (LP_db >= 0) {
				LP_bw = (float)atan((1 + LP_a1) / (1 - LP_a1)) * fs / pi;
			} else {
				LP_bw = (float)atan((1 + LP_a1) * pow(10, (LP_db / 20)) / (1 - LP_a1)) * fs / pi;
			}

			//alog("hexLP_H0: %x\n", hexLP_H0);
			//alog("hexLP_a1: %x\n", hexLP_a1);
			//alog("[LP]db: %2.4f, bw: %2.4f\n", LP_db, LP_bw);
			hexLP_db = LP_db * Q824_ONE; //Q8.24
			k_freqEQ_info.bw[p_cfg[i * BAND_INFO_SIZE]] = LP_bw + 0.5; // rounding
			k_freqEQ_info.gain[p_cfg[i * BAND_INFO_SIZE]] = hexLP_db;
			k_freqEQ_info.type[p_cfg[i * BAND_INFO_SIZE]] = APP_FREQ_EQ_LPF_TYPE; // LPF type
			k_freqEQ_info.fc[p_cfg[i * BAND_INFO_SIZE]] = 0;  // LPF don't care fc
			k_freqEQ_info.valid_mask = k_freqEQ_info.valid_mask|(0x1 << p_cfg[i * BAND_INFO_SIZE]);
			/*
			alog("[LP%d]hex_db: %x, hex_bw: %x, hex_fc: %x\n",
			    p_cfg[i*BAND_INFO_SIZE],
			    k_freqEQ_info.gain[p_cfg[i*BAND_INFO_SIZE]],
			    k_freqEQ_info.bw[p_cfg[i*BAND_INFO_SIZE]],
			    k_freqEQ_info.fc[p_cfg[i*BAND_INFO_SIZE]]);
			*/
		} else {
			hexHP_H0 = k_eq_coef[p_cfg[i * BAND_INFO_SIZE]].h0&0x0000ffff;
			hexHP_a1 = k_eq_coef[p_cfg[i * BAND_INFO_SIZE]].a1&0x0000ffff;
			if (hexHP_a1&SIGN_INT16) {
				HP_a1 = (float)hexHP_a1 / Q214_ONE - 4; // 2^2 = 4
			} else {
				HP_a1 = (float)hexHP_a1 / Q214_ONE;
			}

			HP_db = -6.02; //default value refer from AQ page
			HP_bw = (float)atan((1 + HP_a1) / (2 * (1 - HP_a1))) * 2 * fs / pi;
			hexHP_db = HP_db * Q824_ONE; //Q8.24
			k_freqEQ_info.bw[p_cfg[i * BAND_INFO_SIZE]] = HP_bw + 0.5; // rounding
			k_freqEQ_info.gain[p_cfg[i * BAND_INFO_SIZE]] = hexHP_db;
			k_freqEQ_info.type[p_cfg[i * BAND_INFO_SIZE]] = APP_FREQ_EQ_HPF_TYPE; // HPF type
			k_freqEQ_info.fc[p_cfg[i * BAND_INFO_SIZE]] = 0;  // HPF don't care fc
			k_freqEQ_info.valid_mask = k_freqEQ_info.valid_mask|(0x1 << p_cfg[i * BAND_INFO_SIZE]);

			//alog("[HP]HP_bw: %2.4f\n", HP_bw);
			/*
			alog("[HP%d]hex_db: %x, hex_bw: %x, hex_fc: %x\n",
			    p_cfg[i*BAND_INFO_SIZE],
			    k_freqEQ_info.gain[p_cfg[i*BAND_INFO_SIZE]],
			    k_freqEQ_info.bw[p_cfg[i*BAND_INFO_SIZE]],
			    k_freqEQ_info.fc[p_cfg[i*BAND_INFO_SIZE]]);
			*/
		}
	}

#if defined(FREQ_EQ_CALIBRATION)
	if (eq_loud_type == APP_EQ_TYPE_ID) {
		int    band;
		double freq_gain[APP_EQ_BAND_NUM];
		double freq_gain_db[APP_EQ_BAND_NUM];
		double freq_gain_all[APP_EQ_BAND_NUM] = {0};
		//alog("BPF_a1_arr[1]: %2.4f\n", BPF_a1_arr[1]);
		//alog("BPF_a2_arr[1]: %2.4f\n", BPF_a2_arr[1]);
		//alog("BPF_h0_arr[1]: %2.4f\n", BPF_H0_arr[1]);

		// in this version, we ignore band0's influence on band1
		for (band = 1; band < APP_EQ_BAND_NUM; band++) {
			// first band influence on the next band
			if (band == 1) {
				freq_gain[band + 1] = bpfilter_response(k_freqEQ_info.fc[band + 1], BPF_a1_arr[band], BPF_a2_arr[band], BPF_H0_arr[band]);
				freq_gain_db[band + 1] = 20 * log10(freq_gain[band + 1]);
				freq_gain_all[band + 1] = freq_gain_all[band + 1] + freq_gain_db[band + 1];
			}

			// last band influence on the previous band
			if (band == (APP_EQ_BAND_NUM - 1)) {
				freq_gain[band - 1] = bpfilter_response(k_freqEQ_info.fc[band - 1], BPF_a1_arr[band], BPF_a2_arr[band], BPF_H0_arr[band]);
				freq_gain_db[band - 1] = 20 * log10(freq_gain[band - 1]);
				freq_gain_all[band - 1] = freq_gain_all[band - 1] + freq_gain_db[band - 1];
			}

			// BPF influence on the neighbor bands
			if ((band > 1) && band < (APP_EQ_BAND_NUM - 1)) {
				freq_gain[band + 1] = bpfilter_response(k_freqEQ_info.fc[band + 1], BPF_a1_arr[band], BPF_a2_arr[band], BPF_H0_arr[band]);
				freq_gain_db[band + 1] = 20 * log10(freq_gain[band + 1]);
				freq_gain_all[band + 1] = freq_gain_all[band + 1] + freq_gain_db[band + 1];

				freq_gain[band - 1] = bpfilter_response(k_freqEQ_info.fc[band - 1], BPF_a1_arr[band], BPF_a2_arr[band], BPF_H0_arr[band]);
				freq_gain_db[band - 1] = 20 * log10(freq_gain[band - 1]);
				freq_gain_all[band - 1] = freq_gain_all[band - 1] + freq_gain_db[band - 1];
			}

		}

		for (band = 1; band < APP_EQ_BAND_NUM; band++) {
			BPF_db_arr[band] = freq_gain_all[band] + BPF_db_arr[band];
			k_freqEQ_info.gain[band] = BPF_db_arr[band] * Q824_ONE; //Q8.24
										//alog("BPF_db_arr[%d]: %2.4f\n", band, BPF_db_arr[band]);
		}
	}

#endif
	HwpGetFreqEQ(&k_freqEQ_info);

}

/**
 * Get Frequency Equalizer
 *
 * @param <p_eq_info>		{ pointer of HW frequency equalizer info}
 * @return			        { AIO_OK or AIO_ERROR_CODE }
 * @ingroup lib_audio
 */
static int32_t HwpGetFreqEQ(AUDIO_EQ_INFO *p_eq_info)
{
	HRESULT           retval,
			  *result;
	CLNT_STRUCT       clnt;
	AUDIO_RPC_FREQ_EQ freqEQ_config;

	clnt = prepareCLNT(BLOCK_MODE|USE_INTR_BUF|SEND_AUDIO_CPU, AUDIO_SYSTEM, VERSION);

	int band_index;

	freqEQ_config.valid_mask = p_eq_info->valid_mask;
	for (band_index = 0; band_index < APP_EQ_BAND_MAX_NUM; band_index++) {
		if (p_eq_info->valid_mask&0x1) {
			freqEQ_config.bw[band_index] = p_eq_info->bw[band_index];
			freqEQ_config.fc[band_index] = p_eq_info->fc[band_index];
			freqEQ_config.gain[band_index] = p_eq_info->gain[band_index];
			freqEQ_config.type[band_index] = p_eq_info->type[band_index];
		}

		p_eq_info->valid_mask = p_eq_info->valid_mask >> 1;
	}

	freqEQ_config.mode = FREQ_EQ_HW_MODE;
	result = (HRESULT *)AUDIO_RPC_ToAgent_PP_FREQ_EQ_Config_0(&freqEQ_config, &clnt);

	retval = *result;
	free(result);
	return (int32_t)retval;
}

#endif

void Audio_AppEnablePrintf(uint32_t isEnable)
{
	if (isEnable == 1)
		m_isNeed2Print = 1;
	else
		m_isNeed2Print = 0;


	//	ioctl(audio_fd, HWP_CMD_SET_PRINTFLAG,  isEnable);
}

void  Audio_HwpSetLimitOuptut_DAC0(bool para, uint32_t maxLevelThresdhold)
{
	/* --------------------------------------------------*/
	/* ?Œâ??€?€?? ?Œâ??€?€?? ?Œâ??€?€?? ?Œâ??€?€?€?€?€?
	 * ?‚AVC1  ?œâ??¤dvol  ?œâ??¤AVC2  ?œâ??¤HW-limiter  ?
	 * ?”â??€?€?? ?”â??€?€?? ?”â??€?€?? ?”â??€?€?€?€?€?
	 * --------------------------------------------------*/

	// W= V*V / R
	// ex speaker max W =  8W , R = 8, V ==> 8
	//  amp is 21 db (according to pcb ) ,  21= 20* log(11.22 ) ==> 11.22 ?
	// IC v = 8/11.22 = 0.71 V
	// IC output 1 V = 0x7FFFFFFF , 0.71V * 0x7FFFFFFF  (0x80000000)  = 0x5AE1 47AE
	// threshold is 0.71V >>24   = 0x5A , so thredhold is set 0x5A or (0x5A -1)

	audio_dac0_1_src_sel_RBUS dac0_1_src_sel_RBUS_reg;

	dac0_1_src_sel_RBUS_reg.regValue = AIO_ReadRegister(AUDIO_dac0_1_src_sel_reg);
	dac0_1_src_sel_RBUS_reg.dac0_hw_out_limiter_en  = para;
	dac0_1_src_sel_RBUS_reg.dac0_hw_out_limiter     = maxLevelThresdhold;
	AIO_WriteRegister(AUDIO_dac0_1_src_sel_reg, dac0_1_src_sel_RBUS_reg.regValue);

	return;
}


void  Audio_HwpSetLimitOuptut_I2S0(bool para, uint32_t maxLevelThresdhold)
{

    audio_i2s_out_sel_2_RBUS i2s_out_sel_2_RBUS_reg;

    i2s_out_sel_2_RBUS_reg.regValue = AIO_ReadRegister(AUDIO_i2s_out_sel_2_reg);
    i2s_out_sel_2_RBUS_reg.hw_limit_en_ch01	= para;
    i2s_out_sel_2_RBUS_reg.hw_limit_ch01 	= maxLevelThresdhold;

    i2s_out_sel_2_RBUS_reg.hw_limit_en_ch23	= para;
    i2s_out_sel_2_RBUS_reg.hw_limit_ch23 	= maxLevelThresdhold;

    i2s_out_sel_2_RBUS_reg.hw_limit_en_ch45	= para;
    i2s_out_sel_2_RBUS_reg.hw_limit_ch45 	= maxLevelThresdhold;

    i2s_out_sel_2_RBUS_reg.hw_limit_en_ch67	= para;
    i2s_out_sel_2_RBUS_reg.hw_limit_ch67 	= maxLevelThresdhold;
    AIO_WriteRegister(AUDIO_i2s_out_sel_2_reg, i2s_out_sel_2_RBUS_reg.regValue);

    return;
}

void  Audio_FactoryDisableHwPp(void)
{

}

void  Audio_FactoryEnableHwPp(void)
{

}

void  Audio_FactoryDisableSwPp(void)
{

	audio_hw_process_control_RBUS			HWP_CTRL_REG;

	HWP_CTRL_REG.regValue = Audio_HwpReadRegister(AUDIO_HW_Process_Control_reg);
	HWP_CTRL_REG.basic_premium_sel = 0;
	Audio_HwpWriteRegister(AUDIO_HW_Process_Control_reg, HWP_CTRL_REG.regValue);
}

void  Audio_FactoryAVC(bool enable, uint32_t value)
{
#if 0 //no use function
	APP_AVC_CFG aAPP_AVC_CFG;

	if (enable == AIO_DISABLE) {
		Audio_HwpSetAVC(AIO_DISABLE, NULL);
		alog("*** set avc off \n");
	} else {

		aAPP_AVC_CFG.vol_detect = 3;
		aAPP_AVC_CFG.noise_gain =  APP_AVC_NOISE_GAIN_M12DB; //accoud to customer's spec
		aAPP_AVC_CFG.noise_thd = APP_AVC_NOISE_THD_NONE; //accoud to customer's spec

		aAPP_AVC_CFG.level_max = value; //accoud to customer's spec
		aAPP_AVC_CFG.level_min = value; // accoud to customer's spec

		aAPP_AVC_CFG.step_up = APP_AVC_STEP_GAIN_0P250DB; //default
		aAPP_AVC_CFG.step_down = APP_AVC_STEP_GAIN_0P250DB; //default

		aAPP_AVC_CFG.thd_cnt = 0xc8; //default
		aAPP_AVC_CFG.win_zcross = 0x200; //default
		aAPP_AVC_CFG.win_moniter = 0x200; //default//10-bit
		Audio_HwpSetAVC(AIO_ENABLE, &aAPP_AVC_CFG);

		alog("*** set avc  %d \n", value);

	}
#endif
}

void Audio_HwpSetSpdifOut_Mute(bool para, uint32_t mute_id)
{
	Audio_AioSetSpdifOutMute(para, mute_id);
}

#ifdef USE_SW_SOUND_SURROUND
int32_t Audio_SwpSetBassBack(bool para, APP_BASSBACK_TV_CFG *pBass)
{
	//#ifdef ENABLE_SRS_SURROUND // gene modify => default enable it
	CLNT_STRUCT           clnt    = prepareCLNT(BLOCK_MODE|USE_INTR_BUF|SEND_AUDIO_CPU,
								 AUDIO_SYSTEM, VERSION);


	HRESULT               retval,
			      *result;

	AUDIO_RPC_TV_BASSBACK info;

	info.instanceID = -1; // do not care
	info.pp_bassback_Enable = para;

	alog("bb  enable == %x vb = %x \n", para, pBass->vb_enable);

	//  info.pp_bassback_mode = (unsigned char ) (pBass->ModeType);
	info.pp_bassback_mode.Enable_Sys = pBass->Enable_Sys;
	info.pp_bassback_mode.StopBandFreq_Sys = pBass->StopBandFreq_Sys;
	info.pp_bassback_mode.HarmonicGain = pBass->HarmonicGain;
	info.pp_bassback_mode.vb_enable = pBass->vb_enable;
	info.pp_bassback_mode.Sys_EnhanceFreq = pBass->Sys_EnhanceFreq;
	info.pp_bassback_mode.Tru_StopbandFreq = pBass->Tru_StopbandFreq;
	info.pp_bassback_mode.Set_BassGain = pBass->Set_BassGain;


	result = (HRESULT *)AUDIO_RPC_ToAgent_PP_BASSBACK_TV_Config_0(&info, &clnt);
	retval = *result;
	free(result);
	return retval;
	return AIO_OK;
}

int32_t Audio_SwpSetBassBackGain(uint32_t bassGain)
{
	//#ifdef ENABLE_SRS_SURROUND // gene modify => default enable it
	CLNT_STRUCT                            clnt    = prepareCLNT(BLOCK_MODE|USE_INTR_BUF|SEND_AUDIO_CPU,
												   AUDIO_SYSTEM, VERSION);


	HRESULT                                retval,
					       *result;

	AUDIO_RPC_TV_BASSBACK_CHANGE_BASS_GAIN info;

	info.instanceID = -1; // do not care
	info.pp_bassback_bassgain = (u_char)bassGain;


	alog("bb  gain == %x \n", bassGain);




	result = (HRESULT *)AUDIO_RPC_ToAgent_PP_BASSBACK_Change_Bass_Gain_0(&info, &clnt);
	retval = *result;
	free(result);
	return retval;
	return AIO_OK;
}

int32_t Audio_SwpSetBassBack_PCBU(bool para, APP_BASSBACK_PCBU_CFG *pBass)
{
	//#ifdef ENABLE_SRS_SURROUND // gene modify => default enable it
	CLNT_STRUCT clnt = prepareCLNT(BLOCK_MODE|USE_INTR_BUF|SEND_AUDIO_CPU,
						      AUDIO_SYSTEM, VERSION);
	HRESULT retval;
	HRESULT *result;

	AUDIO_RPC_PCBU_BASSBACK info;

	info.instanceID = -1; // do not care
	info.pp_bassback_Enable = para;

	alog("bb  enable == %x vb = %x \n", para, pBass->vb_enable);

	//  info.pp_bassback_mode = (unsigned char ) (pBass->ModeType);
	info.pp_bassback_mode.Enable_Sys = pBass->Enable_Sys;
	info.pp_bassback_mode.StopBandFreq_Sys = pBass->StopBandFreq_Sys;
	info.pp_bassback_mode.HarmonicGain = pBass->HarmonicGain;
	info.pp_bassback_mode.SubHarmonicGain = pBass->SubHarmonicGain;
	info.pp_bassback_mode.TreHarmonicGain = pBass->TreHarmonicGain;
	info.pp_bassback_mode.vb_enable = pBass->vb_enable;
	;
	// info.pp_bassback_mode.MixEnable= pBass->MixEnable ;

	info.pp_bassback_mode.Sys_EnhanceFreq = pBass->Sys_EnhanceFreq;
	info.pp_bassback_mode.Tru_StopbandFreq = pBass->Tru_StopbandFreq;
	info.pp_bassback_mode.Set_BassGain = pBass->Set_BassGain;

	info.pp_bassback_mode.Enable_HPF = pBass->Enable_HPF;
	info.pp_bassback_mode.StopBandFreq_HPF = pBass->StopBandFreq_HPF;

	info.pp_bassback_mode.Enable_PF = pBass->Enable_PF;
	info.pp_bassback_mode.Gain_PF = pBass->Gain_PF;
	info.pp_bassback_mode.CenterFreq_PF = pBass->CenterFreq_PF;
	info.pp_bassback_mode.BandWidth_PF = pBass->BandWidth_PF;

	result = (HRESULT *)AUDIO_RPC_ToAgent_PP_BASSBACK_PCBU_Config_0(&info, &clnt);
	retval = *result;
	free(result);
	return retval;
}
#endif

#ifndef TV001_BOARD
// For Mac3 Audio output L/R mismatch compensation [Start]
static void  GetAudioOutputFromOTP(void)
{
	uint32_t Audio_out_LR_Diff_OTP;

	Audio_out_LR_Diff_OTP = AIO_ReadRegister(AUDIO_OUT_LR_DIFFERENCE_OTP_VADDR);

	g_DAC0_AIO_OTP = ((Audio_out_LR_Diff_OTP >> 20)&AUDIO_OUT_LR_OTP_MASK);
	alog("[Audio][Debug]g_DAC0_AIO_OTP= %x \n", g_DAC0_AIO_OTP);
	// only for debug test
	//g_DAC0_AIO_OTP = 0x7;
	//alog("[Audio][Debug][test]g_DAC0_AIO_OTP= %x \n", g_DAC0_AIO_OTP);
	//end

	g_DAC0_HP_OTP = ((Audio_out_LR_Diff_OTP >> 15)&AUDIO_OUT_LR_OTP_MASK);
	alog("[Audio][Debug]g_DAC0_HP_OTP= %x \n", g_DAC0_HP_OTP);
	// only for debug test
	//g_DAC0_HP_OTP = 0x7;
	//alog("[Audio][Debug][test]g_DAC0_HP_OTP= %x \n", g_DAC0_HP_OTP);
	//end

	g_DAC1_AIO_OTP = ((Audio_out_LR_Diff_OTP >> 5)&AUDIO_OUT_LR_OTP_MASK);
	alog("[Audio][Debug]g_DAC1_AIO_OTP= %x \n", g_DAC1_AIO_OTP);
	// only for debug test
	//g_DAC1_AIO_OTP = 0x5;
	//alog("[Audio][Debug][test]g_DAC1_AIO_OTP= %x \n", g_DAC1_AIO_OTP);
	//end

	g_DAC1_HP_OTP = ((Audio_out_LR_Diff_OTP >> 0)&AUDIO_OUT_LR_OTP_MASK);
	alog("[Audio][Debug]g_DAC1_HP_OTP= %x \n", g_DAC1_HP_OTP);
	// only for debug test
	//g_DAC1_HP_OTP = 0x5;
	//alog("[Audio][Debug][test]g_DAC1_HP_OTP= %x \n", g_DAC1_HP_OTP);
	//end

}
#endif

static void ResetAudioOutputCompensate(void)
{
	g_PBDVC_comp_gain = 0;
	g_TSDVC_comp_gain = 0;
}

int32_t GetPBPath(void)
{
#if 0//no requirment
	audio_dac0_1_src_sel_RBUS	  dac0_1_src_sel_reg;
	audio_analog_aio_hpout_lineout_l_RBUS	analog_aio_hpout_lineout_LR_REG;
	audio_analog_frontend_mute_RBUS	analog_frontend_mute_REG;

	dac0_1_src_sel_reg.regValue= AIO_ReadRegister(AUDIO_dac0_1_src_sel_reg);
	analog_aio_hpout_lineout_LR_REG.regValue= AIO_ReadRegister(AUDIO_analog_aio_Hpout_Lineout_L_reg);
	analog_frontend_mute_REG.regValue= AIO_ReadRegister(AUDIO_ANALOG_FRONTEND_MUTE_VADDR);

	if ((0 == dac0_1_src_sel_reg.dac0_pcm_source_sel) || (1 == dac0_1_src_sel_reg.dac0_pcm_source_sel) || (2 == dac0_1_src_sel_reg.dac0_pcm_source_sel) || (5 == dac0_1_src_sel_reg.dac0_pcm_source_sel) || (6 == dac0_1_src_sel_reg.dac0_pcm_source_sel) || (7 == dac0_1_src_sel_reg.dac0_pcm_source_sel)){
		if (7 == analog_frontend_mute_REG.reg_bb_aio_mutel)
		return DAC0_AIO;    // DAC0-->AIO
		else if (0x17 == analog_aio_hpout_lineout_LR_REG.reg_bb_hpo_mutel)
		return DAC0_HP; // DAC0-->HP
		else
		return PATH_ERROR;
	} else if ((0 == dac0_1_src_sel_reg.dac1_pcm_source_sel) || (1 == dac0_1_src_sel_reg.dac1_pcm_source_sel) || (2 == dac0_1_src_sel_reg.dac1_pcm_source_sel) || (5 == dac0_1_src_sel_reg.dac1_pcm_source_sel) || (6 == dac0_1_src_sel_reg.dac1_pcm_source_sel) || (7 == dac0_1_src_sel_reg.dac1_pcm_source_sel)){
		if (0xb == analog_frontend_mute_REG.reg_bb_aio_mutel)
		return DAC1_AIO;    // DAC1-->AIO
		else if (0x1b == analog_aio_hpout_lineout_LR_REG.reg_bb_hpo_mutel)
		return DAC1_HP; // DAC1-->HP
		else
		return PATH_ERROR;
	} else
	return PATH_ERROR;
#else
	return PATH_ERROR;
#endif
}

int32_t GetTSPath(void)
{
#if 0//no requirment
	audio_dac0_1_src_sel_RBUS	  dac0_1_src_sel_reg;
	audio_analog_aio_hpout_lineout_l_RBUS   analog_aio_hpout_lineout_LR_REG;
	audio_analog_frontend_mute_RBUS	analog_frontend_mute_REG;

	dac0_1_src_sel_reg.regValue= AIO_ReadRegister(AUDIO_dac0_1_src_sel_reg);
	analog_aio_hpout_lineout_LR_REG.regValue= AIO_ReadRegister(AUDIO_analog_aio_Hpout_Lineout_L_reg);
	analog_frontend_mute_REG.regValue= AIO_ReadRegister(AUDIO_ANALOG_FRONTEND_MUTE_VADDR);

	if ((3 == dac0_1_src_sel_reg.dac0_pcm_source_sel) || (4 == dac0_1_src_sel_reg.dac0_pcm_source_sel)){
		if (7 == analog_frontend_mute_REG.reg_bb_aio_mutel)
		return DAC0_AIO;
		else if (0x17 == analog_aio_hpout_lineout_LR_REG.reg_bb_hpo_mutel)
		return DAC0_HP;
		else
		return PATH_ERROR;
	} else if ((3 == dac0_1_src_sel_reg.dac1_pcm_source_sel) || (4 == dac0_1_src_sel_reg.dac1_pcm_source_sel)){
		if (0xb == analog_frontend_mute_REG.reg_bb_aio_mutel)
		return DAC1_AIO;
		else if (0x1b == analog_aio_hpout_lineout_LR_REG.reg_bb_hpo_mutel)
		return DAC1_HP;
		else
		return PATH_ERROR;
	} else
	return PATH_ERROR;
#else
	return PATH_ERROR;
#endif
}

/*
 * Compensation table
 * OTP value ==> Compensate Gain (dB) @ only R ch==> Compensate Gain (API para)
 * OTP value = xabcd ; x = 0 (L>R) / x = 1 (L<R)
 * <Case1: L > R, x=0>
 * (0) 00000 ==> 0dB ==> 0
 * (1) 00001 ==> 0.125dB ==> 1
 * (2) 00010 ==> 0.25dB ==> 2
 * (3) 00011 ==> 0.375dB ==> 3
 * (4) 00100 ==> 0.5dB ==> 4
 * (5) 00101 ==> 0.625dB ==> 5
 * (6) 00110 ==> 0.75dB ==> 6
 * (7) 00111 ==> 0.875dB ==> 7
 * (8) 01000 ==> 1dB ==> 8
 * (9) 01001 ==> 1.125dB ==> 9
 * (10) 01010 ==> 1.25dB ==> 10
 * <Case2: L < R, x=1>
 * (0) 10000 ==> -0dB ==> 0
 * (1) 10001 ==> -0.125dB ==> -1
 * (2) 10010 ==> -0.25dB ==> -2
 * (3) 10011 ==> -0.375dB ==> -3
 * (4) 10100 ==> -0.5dB ==> -4
 * (5) 10101 ==> -0.625dB ==> -5
 * (6) 10110 ==> -0.75dB ==> -6
 * (7) 10111 ==> -0.875dB ==> -7
 * (8) 11000 ==> -1dB ==> -8
 *(9) 11001 ==> -1.125dB ==> -9
 * (10) 11010 ==> -1.25dB ==> -10
 */
static void  CompensateAudioOutput_LRDifference(uint32_t isEnable)
{
	int32_t getPBpath       = 0,
		getTSpath       = 0;



	if (1 == isEnable) {
		getPBpath = GetPBPath();
		getTSpath = GetTSPath();

		alog("[Audio][Debug]getPBpath= %x \n", getPBpath);
		alog("[Audio][Debug]getTSpath= %x \n", getTSpath);

		if ((g_AIO_path == PB_path) || (g_AIO_path == PB_TS_path)) {
			switch (getPBpath) {
			case DAC0_AIO:
				if ((g_DAC0_AIO_OTP >> 4) == 0)
					g_PBDVC_comp_gain = (g_DAC0_AIO_OTP&AUDIO_OUT_LR_OTP_VALUE_MASK);
				else
					g_PBDVC_comp_gain = 0 - (g_DAC0_AIO_OTP&AUDIO_OUT_LR_OTP_VALUE_MASK);

				break;

			case DAC0_HP:
				if ((g_DAC0_HP_OTP >> 4) == 0)
					g_PBDVC_comp_gain = (g_DAC0_HP_OTP&AUDIO_OUT_LR_OTP_VALUE_MASK);
				else
					g_PBDVC_comp_gain = 0 - (g_DAC0_HP_OTP&AUDIO_OUT_LR_OTP_VALUE_MASK);

				break;

			case DAC1_AIO:
				if ((g_DAC1_AIO_OTP >> 4) == 0)
					g_PBDVC_comp_gain = (g_DAC1_AIO_OTP&AUDIO_OUT_LR_OTP_VALUE_MASK);
				else
					g_PBDVC_comp_gain = 0 - (g_DAC1_AIO_OTP&AUDIO_OUT_LR_OTP_VALUE_MASK);

				break;

			case DAC1_HP:
				if ((g_DAC1_HP_OTP >> 4) == 0)
					g_PBDVC_comp_gain = (g_DAC1_HP_OTP&AUDIO_OUT_LR_OTP_VALUE_MASK);
				else
					g_PBDVC_comp_gain = 0 - (g_DAC1_HP_OTP&AUDIO_OUT_LR_OTP_VALUE_MASK);

				break;

			default:
				g_PBDVC_comp_gain = 0;
				break;
			}
		} else
			g_PBDVC_comp_gain = 0;

		alog("[Audio][Debug]g_PBDVC_comp_gain= %d \n", g_PBDVC_comp_gain);


		if ((g_AIO_path == TS_path) || (g_AIO_path == PB_TS_path)) {
			switch (getTSpath) {
			case DAC0_AIO:
				if ((g_DAC0_AIO_OTP >> 4) == 0)
					g_TSDVC_comp_gain = (g_DAC0_AIO_OTP&AUDIO_OUT_LR_OTP_VALUE_MASK);
				else
					g_TSDVC_comp_gain = 0 - (g_DAC0_AIO_OTP&AUDIO_OUT_LR_OTP_VALUE_MASK);

				break;

			case DAC0_HP:
				if ((g_DAC0_HP_OTP >> 4) == 0)
					g_TSDVC_comp_gain = (g_DAC0_HP_OTP&AUDIO_OUT_LR_OTP_VALUE_MASK);
				else
					g_TSDVC_comp_gain = 0 - (g_DAC0_HP_OTP&AUDIO_OUT_LR_OTP_VALUE_MASK);

				break;

			case DAC1_AIO:
				if ((g_DAC1_AIO_OTP >> 4) == 0)
					g_TSDVC_comp_gain = (g_DAC1_AIO_OTP&AUDIO_OUT_LR_OTP_VALUE_MASK);
				else
					g_TSDVC_comp_gain = 0 - (g_DAC1_AIO_OTP&AUDIO_OUT_LR_OTP_VALUE_MASK);

				break;

			case DAC1_HP:
				if ((g_DAC1_HP_OTP >> 4) == 0)
					g_TSDVC_comp_gain = (g_DAC1_HP_OTP&AUDIO_OUT_LR_OTP_VALUE_MASK);
				else
					g_TSDVC_comp_gain = 0 - (g_DAC1_HP_OTP&AUDIO_OUT_LR_OTP_VALUE_MASK);

				break;

			default:
				g_TSDVC_comp_gain = 0;
				break;
			}
		} else
			g_TSDVC_comp_gain = 0;

		alog("[Audio][Debug]g_TSDVC_comp_gain= %d \n", g_TSDVC_comp_gain);
	} else
		ResetAudioOutputCompensate();
}

void Audio_EnableCompensateAudioOutput(uint32_t isEnable, APP_AUDIO_PATH path)
{
	alog("[%s] Not Support.........\n", __FUNCTION__);

#if 0
	if (1 == isEnable) {
		g_AIO_path = path;
		//alog("[Audio][Debug]EnableCompensate, path= %d, g_AIO_path= %d \n", path, g_AIO_path);
		g_ENABLE_AUDIOOUTPUTCOMPENSATE = 1;
	} else {
		g_AIO_path = 0;
		//alog("[Audio][Debug]DisableCompensate, path= %d, g_AIO_path= %d \n", path, g_AIO_path);
		g_ENABLE_AUDIOOUTPUTCOMPENSATE = 0;
	}
#endif
}
// For Sirius Audio output L/R mismatch compensation [End]

unsigned int Audio_HwpReadRegister(unsigned int addr)
{
	unsigned int ret = 0;
	audio_ack_fail_info_RBUS ack_fail_info;
	int read_timeout = 10;

	ack_fail_info.regValue = AIO_ReadRegister(AUDIO_ack_fail_info_reg);
	ack_fail_info.ack_fail_rw = 1; //write 1 to clear this register
	AIO_WriteRegister(AUDIO_ack_fail_info_reg, ack_fail_info.regValue);
	audio_hw_usleep(2); //wait flag already clear time t1, N >= 2t1
	ret = AIO_ReadRegister(addr);
	while(1)
	{
		//audio_hw_usleep(2);// wait auto-ack Timeout t2(1us), N >= 2t2
		ack_fail_info.regValue = AIO_ReadRegister(AUDIO_ack_fail_info_reg);
		if (ack_fail_info.ack_fail_addr == 0 || read_timeout <= 0)
		{
			if (read_timeout <= 9 || ack_fail_info.ack_fail_addr != 0)
				alog_err("audio_debug Leave, read_timeout = %d, addr = %x\n", read_timeout, addr);
			
			break;
		}

		if (ack_fail_info.ack_fail_addr != 0)
		{
			ack_fail_info.ack_fail_rw = 1;  //write 1 to clear this register
			AIO_WriteRegister(AUDIO_ack_fail_info_reg, ack_fail_info.regValue);
			audio_hw_usleep(2); //wait flag already clear time t1, N >= 2t1
			ret = AIO_ReadRegister(addr);
			alog_err("audio_debug rbus Timeout\n");
		}
		
		read_timeout --;
	}
	
	return ret;
}

void Audio_HwpWriteRegister(unsigned int addr, unsigned int value)
{
	audio_ack_fail_info_RBUS ack_fail_info;
	int read_timeout = 10;

	ack_fail_info.regValue = AIO_ReadRegister(AUDIO_ack_fail_info_reg);
	ack_fail_info.ack_fail_rw = 1; //write 1 to clear this register
	AIO_WriteRegister(AUDIO_ack_fail_info_reg, ack_fail_info.regValue);
	audio_hw_usleep(2); //wait flag already clear time t1, N >= 2t1
	AIO_WriteRegister(addr, value);
	//usleep(40);	//Due to some HWP reg control has double buffer, in line with samples, so keep at leaset 20us(48kHz) delay after write reg for safety
	while(1)
	{
		//audio_hw_usleep(2);// wait auto-ack Timeout t2(1us), N >= 2t2
		ack_fail_info.regValue = AIO_ReadRegister(AUDIO_ack_fail_info_reg);
		if (ack_fail_info.ack_fail_addr == 0 || read_timeout <= 0)
		{
			if (read_timeout <= 9 || ack_fail_info.ack_fail_addr != 0)
			{
				alog_err("audio_debug Leave, write_timeout = %d, addr = %x\n", read_timeout, addr);
			}
			
			//Due to some HWP reg control has double buffer, in line with samples, so keep at leaset 20us(48kHz) delay after write reg for safety
			audio_hw_usleep(40); //t2 + t3 = 40us
			break;
		}

		if (ack_fail_info.ack_fail_addr != 0)
		{
			ack_fail_info.ack_fail_rw = 1;  //write 1 to clear this register
			AIO_WriteRegister(AUDIO_ack_fail_info_reg, ack_fail_info.regValue);
			audio_hw_usleep(2); //wait flag already clear time t1, N >= 2t1
			AIO_WriteRegister(addr, value);
			alog_err("audio_debug rbus Timeout");
		}
		
		read_timeout --;
	}
}

int32_t Audio_HwpSetEQ_Smooth_H0(APP_EQ_ALL_BAND band_id, APP_EQ_COEF eq_coef)
{
	int32_t ret=0;
	return ret;
}

int32_t Audio_HwpSetEQ_Smooth_H0_Run(APP_EQ_ALL_BAND band_id, APP_EQ_COEF eq_coef)
{

	return AIO_OK;

}

int32_t Audio_HwpSetEQParameter(APP_EQ_ALL_BAND band_id, APP_EQ_COEF eq_coef)
{
	return AIO_OK;
}


int32_t Audio_HwpSetLoud_Smooth_H0(APP_LOUD_ALL_BAND band_id, APP_EQ_COEF loud_coef)
{

	return AIO_OK;
}
int32_t Audio_HwpSetLoud_Smooth_H0_Run(APP_LOUD_ALL_BAND band_id, APP_EQ_COEF loud_coef)
{
	return AIO_OK;
}

int32_t Audio_HwpSetLoudParameter(APP_LOUD_ALL_BAND band_id, APP_EQ_COEF loud_coef)
{	
	
	return AIO_OK;
}

int32_t Audio_HwpWriteEQControlEnable(APP_EQ_ALL_BAND band_id, bool eq_enable)
{	

	return AIO_OK;
}

int32_t Audio_HwpWriteLoudControlEnable(APP_LOUD_ALL_BAND band_id, bool loud_enable)
{

	return AIO_OK;
}

int32_t Audio_HwpReadEQControlEnable(uint32_t *p_eq_ctrl_reg)
{

	return AIO_OK;
}

int32_t Audio_HwpReadLoudControlEnable(uint32_t *p_loud_ctrl_reg)
{

	return AIO_OK;
}

int32_t HwpSetDRC_HighBand(bool para, APP_DRC_CFG *p_cfg_drc)
{
#ifdef SUPPORT_TWO_BAND_DRC
	audio_drc_control_1_ch01_RBUS DRC_CTRL1_REG;
	audio_drc_control_2_ch01_RBUS DRC_CTRL2_REG;
	audio_drc_control_3_ch01_RBUS DRC_CTRL3_REG;
	audio_drc_control_4_ch01_RBUS DRC_CTRL4_REG;
	audio_drc_control_5_ch01_RBUS DRC_CTRL5_REG;
	audio_drc_control_6_ch01_RBUS DRC_CTRL6_REG;
	audio_drc_control_7_ch01_RBUS DRC_CTRL7_REG;
	APP_DRC_CFG                   *t_cfg_drc;

	if (p_cfg_drc == NULL) {
		//apply default setting
		t_cfg_drc = &m_cfg_drc;
	} else {
		t_cfg_drc = p_cfg_drc;
	}

	// Please refer "drc_lpf_coef" enum
	DRC_CTRL1_REG.regValue = AIO_ReadRegister(AUDIO_DRC_Control_1_ch01_reg);
	DRC_CTRL1_REG.drc_lpf_coef = APP_AVC_LPF_COEF_6;    // default 2^6 conf parameter
	AIO_WriteRegister(AUDIO_DRC_Control_1_ch01_reg, DRC_CTRL1_REG.regValue);


	if (para == AIO_ENABLE) {
		DRC_CTRL1_REG.regValue = AIO_ReadRegister(AUDIO_DRC_Control_1_ch01_reg);
		DRC_CTRL1_REG.drc_noise_gain = t_cfg_drc->drc_noise_gain;
		DRC_CTRL1_REG.drc_noise_thre = t_cfg_drc->drc_noise_thre;
		DRC_CTRL1_REG.drc_fast_up_db = t_cfg_drc->drc_fast_up_db;
		DRC_CTRL1_REG.drc_slow_up_db = t_cfg_drc->drc_slow_up_db;
		DRC_CTRL1_REG.drc_down_db = t_cfg_drc->drc_down_db;
		DRC_CTRL1_REG.drc_vol_detect = t_cfg_drc->drc_vol_detect;
		DRC_CTRL1_REG.drc_vol_amplif_enable = t_cfg_drc->drc_vol_amplif_enable;
		AIO_WriteRegister(AUDIO_DRC_Control_1_ch01_reg, DRC_CTRL1_REG.regValue);

		DRC_CTRL2_REG.regValue = AIO_ReadRegister(AUDIO_DRC_Control_2_ch01_reg);
		DRC_CTRL2_REG.drc_cnt_thre =  t_cfg_drc->drc_mon_win / 2;
		DRC_CTRL2_REG.drc_mon_win = t_cfg_drc->drc_mon_win;
		DRC_CTRL2_REG.drc_zc_win = t_cfg_drc->drc_mon_win;
		AIO_WriteRegister(AUDIO_DRC_Control_2_ch01_reg, DRC_CTRL2_REG.regValue);

		DRC_CTRL3_REG.regValue = AIO_ReadRegister(AUDIO_DRC_Control_3_ch01_reg);
		DRC_CTRL3_REG.drc_noise_thre_hi = t_cfg_drc->drc_noise_thre_hi;
		AIO_WriteRegister(AUDIO_DRC_Control_3_ch01_reg, DRC_CTRL3_REG.regValue);

		DRC_CTRL4_REG.regValue = AIO_ReadRegister(AUDIO_DRC_Control_4_ch01_reg);
		DRC_CTRL4_REG.drc_level_max = t_cfg_drc->drc_level_max;
		DRC_CTRL4_REG.drc_level_min = t_cfg_drc->drc_level_min;
		AIO_WriteRegister(AUDIO_DRC_Control_4_ch01_reg, DRC_CTRL4_REG.regValue);

		DRC_CTRL5_REG.regValue = AIO_ReadRegister(AUDIO_DRC_Control_5_ch01_reg);
		DRC_CTRL5_REG.drc_small_up_db = t_cfg_drc->drc_small_up_db;
		DRC_CTRL5_REG.drc_small_down_db = t_cfg_drc->drc_small_down_db;
		DRC_CTRL5_REG.drc_small_cnt_thre = t_cfg_drc->drc_mon_win / 2;
		AIO_WriteRegister(AUDIO_DRC_Control_5_ch01_reg, DRC_CTRL5_REG.regValue);

		DRC_CTRL6_REG.regValue = AIO_ReadRegister(AUDIO_DRC_Control_6_ch01_reg);
		DRC_CTRL6_REG.drc_small_level_max = t_cfg_drc->drc_small_level_max;
		DRC_CTRL6_REG.drc_small_level_min = t_cfg_drc->drc_small_level_min;
		AIO_WriteRegister(AUDIO_DRC_Control_6_ch01_reg, DRC_CTRL6_REG.regValue);

		DRC_CTRL7_REG.regValue = AIO_ReadRegister(AUDIO_DRC_Control_7_ch01_reg);
		DRC_CTRL7_REG.drc_small_level = t_cfg_drc->drc_small_level;
		AIO_WriteRegister(AUDIO_DRC_Control_7_ch01_reg, DRC_CTRL7_REG.regValue);

	}
#endif
	return AIO_OK;
}

int32_t HwpSetDRC_LowBand(bool para, APP_DRC_LPF_CFG *p_cfg_drc_lpf)
{
#ifdef SUPPORT_TWO_BAND_DRC
	audio_drc_lpf_control_1_ch01_RBUS DRC_LPF_CTRL1_REG;
	audio_drc_lpf_control_2_ch01_RBUS DRC_LPF_CTRL2_REG;
	audio_drc_lpf_control_3_ch01_RBUS DRC_LPF_CTRL3_REG;
	audio_drc_lpf_control_4_ch01_RBUS DRC_LPF_CTRL4_REG;
	audio_drc_lpf_control_5_ch01_RBUS DRC_LPF_CTRL5_REG;
	audio_drc_lpf_control_6_ch01_RBUS DRC_LPF_CTRL6_REG;
	audio_drc_lpf_control_7_ch01_RBUS DRC_LPF_CTRL7_REG;
	APP_DRC_LPF_CFG                   *t_cfg_drc_lpf;

	if (p_cfg_drc_lpf == NULL) {
		//apply default setting
		t_cfg_drc_lpf = &m_cfg_drc_lpf;
	} else {
		t_cfg_drc_lpf = p_cfg_drc_lpf;
	}

	// Please refer "drc_lpf_coef" enum
	DRC_LPF_CTRL1_REG.regValue = AIO_ReadRegister(AUDIO_DRC_Lpf_Control_1_ch01_reg);
	DRC_LPF_CTRL1_REG.drc_lpf_lpf_coef = APP_AVC_LPF_COEF_6;    // default 2^6 conf parameter
	AIO_WriteRegister(AUDIO_DRC_Lpf_Control_1_ch01_reg, DRC_LPF_CTRL1_REG.regValue);


	if (para == AIO_ENABLE) {
		DRC_LPF_CTRL1_REG.regValue = AIO_ReadRegister(AUDIO_DRC_Lpf_Control_1_ch01_reg);
		DRC_LPF_CTRL1_REG.drc_lpf_noise_gain = t_cfg_drc_lpf->drc_lpf_noise_gain;
		DRC_LPF_CTRL1_REG.drc_lpf_noise_thre = t_cfg_drc_lpf->drc_lpf_noise_thre;
		DRC_LPF_CTRL1_REG.drc_lpf_fast_up_db = t_cfg_drc_lpf->drc_lpf_fast_up_db;
		DRC_LPF_CTRL1_REG.drc_lpf_slow_up_db = t_cfg_drc_lpf->drc_lpf_slow_up_db;
		DRC_LPF_CTRL1_REG.drc_lpf_down_db = t_cfg_drc_lpf->drc_lpf_down_db;
		DRC_LPF_CTRL1_REG.drc_lpf_vol_detect = t_cfg_drc_lpf->drc_lpf_vol_detect;
		DRC_LPF_CTRL1_REG.drc_lpf_vol_amplif_enable = t_cfg_drc_lpf->drc_lpf_vol_amplif_enable;
		AIO_WriteRegister(AUDIO_DRC_Lpf_Control_1_ch01_reg, DRC_LPF_CTRL1_REG.regValue);

		DRC_LPF_CTRL2_REG.regValue = AIO_ReadRegister(AUDIO_DRC_lpf_Control_2_ch01_reg);
		DRC_LPF_CTRL2_REG.drc_lpf_cnt_thre =  t_cfg_drc_lpf->drc_lpf_mon_win / 2;
		DRC_LPF_CTRL2_REG.drc_lpf_mon_win = t_cfg_drc_lpf->drc_lpf_mon_win;
		DRC_LPF_CTRL2_REG.drc_lpf_zc_win = t_cfg_drc_lpf->drc_lpf_mon_win;
		AIO_WriteRegister(AUDIO_DRC_lpf_Control_2_ch01_reg, DRC_LPF_CTRL2_REG.regValue);

		DRC_LPF_CTRL3_REG.regValue = AIO_ReadRegister(AUDIO_DRC_lpf_Control_3_ch01_reg);
		DRC_LPF_CTRL3_REG.drc_lpf_noise_thre_hi = t_cfg_drc_lpf->drc_lpf_noise_thre_hi;
		AIO_WriteRegister(AUDIO_DRC_lpf_Control_3_ch01_reg, DRC_LPF_CTRL3_REG.regValue);

		DRC_LPF_CTRL4_REG.regValue = AIO_ReadRegister(AUDIO_DRC_lpf_Control_4_ch01_reg);
		DRC_LPF_CTRL4_REG.drc_lpf_level_max = t_cfg_drc_lpf->drc_lpf_level_max;
		DRC_LPF_CTRL4_REG.drc_lpf_level_min = t_cfg_drc_lpf->drc_lpf_level_min;
		AIO_WriteRegister(AUDIO_DRC_lpf_Control_4_ch01_reg, DRC_LPF_CTRL4_REG.regValue);

		DRC_LPF_CTRL5_REG.regValue = AIO_ReadRegister(AUDIO_DRC_lpf_Control_5_ch01_reg);
		DRC_LPF_CTRL5_REG.drc_lpf_small_up_db = t_cfg_drc_lpf->drc_lpf_small_up_db;
		DRC_LPF_CTRL5_REG.drc_lpf_small_down_db = t_cfg_drc_lpf->drc_lpf_small_down_db;
		DRC_LPF_CTRL5_REG.drc_lpf_small_cnt_thre = t_cfg_drc_lpf->drc_lpf_mon_win / 2;
		AIO_WriteRegister(AUDIO_DRC_lpf_Control_5_ch01_reg, DRC_LPF_CTRL5_REG.regValue);

		DRC_LPF_CTRL6_REG.regValue = AIO_ReadRegister(AUDIO_DRC_lpf_Control_6_ch01_reg);
		DRC_LPF_CTRL6_REG.drc_lpf_small_level_max = t_cfg_drc_lpf->drc_lpf_small_level_max;
		DRC_LPF_CTRL6_REG.drc_lpf_small_level_min = t_cfg_drc_lpf->drc_lpf_small_level_min;
		AIO_WriteRegister(AUDIO_DRC_lpf_Control_6_ch01_reg, DRC_LPF_CTRL6_REG.regValue);

		DRC_LPF_CTRL7_REG.regValue = AIO_ReadRegister(AUDIO_DRC_lpf_Control_7_ch01_reg);
		DRC_LPF_CTRL7_REG.drc_lpf_small_level = t_cfg_drc_lpf->drc_lpf_small_level;
		AIO_WriteRegister(AUDIO_DRC_lpf_Control_7_ch01_reg, DRC_LPF_CTRL7_REG.regValue);

	}
#endif
	return AIO_OK;
}

int32_t Audio_HwpEnableTwoBandDRC(bool enable, APP_DRC_LPF_CFG *p_cfg_drc_lpf,
				 APP_DRC_CFG *p_cfg_drc)
{
#ifdef SUPPORT_TWO_BAND_DRC
	audio_hw_process_control_RBUS hw_process_control;

	down(&audio_sem_app);
	//disable two_band DRC before configure it
	hw_process_control.regValue = Audio_HwpReadRegister(AUDIO_HW_Process_Control_reg);
	hw_process_control.drc_lpf_enable = AIO_DISABLE;
	hw_process_control.drc_enable = AIO_DISABLE;
	Audio_HwpWriteRegister(AUDIO_HW_Process_Control_reg, hw_process_control.regValue);

	// modify two_band DRC parameters by user
	HwpSetDRC_LowBand(enable, p_cfg_drc_lpf); // lowband parameter
	HwpSetDRC_HighBand(enable, p_cfg_drc); // highband parameter

	// enable two_band DRC function again.
	hw_process_control.regValue = Audio_HwpReadRegister(AUDIO_HW_Process_Control_reg);
	hw_process_control.drc_lpf_enable = AIO_ENABLE;
	hw_process_control.drc_enable = AIO_ENABLE;
	Audio_HwpWriteRegister(AUDIO_HW_Process_Control_reg, hw_process_control.regValue);

	up(&audio_sem_app);
#endif
	return AIO_OK;
}

void CleanEQLoudnessOverflow(void)
{	

}

int32_t Audio_HwpGetMaxEQBandNum(int32_t *eq_band)
{
	*eq_band = MAX_EQ_BAND_NUM;
	return AIO_OK;
}

int32_t Audio_HwpSetCH23AVC2(bool para, APP_AVC2_CFG *p_cfg_avc2)
{
#if 0	
    //hw_process_control_RBUS       HWP_CTRL_REG;
    audio_avc2_control_1_ch23_RBUS AVC2_CTRL0_REG;
    audio_avc2_control_2_ch23_RBUS AVC2_CTRL1_REG;
    audio_avc2_control_3_ch23_RBUS AVC2_CTRL2_REG;
    audio_avc2_control_4_ch23_RBUS AVC2_CTRL3_REG;

    APP_AVC2_CFG                   *t_cfg_avc2;
    audio_hw_process_control_RBUS  hw_process_control;

    if (p_cfg_avc2 == NULL) {
        t_cfg_avc2 = &m_cfg_avc2;
    }
    else {
        t_cfg_avc2 = p_cfg_avc2;
    }

    /*
     * disable H/W-AVC before configure it.
     * This register is a double buffer register,we should check it after setting,
     * but this version of code, we just assume that H/W reaction is always fast enough.
     */
    hw_process_control.regValue = Audio_HwpReadRegister(AUDIO_HW_Process_Control_reg);
    hw_process_control.avc2_enable = AIO_DISABLE;
    Audio_HwpWriteRegister(AUDIO_HW_Process_Control_reg, hw_process_control.regValue);

    AVC2_CTRL0_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_1_ch23_reg);
    AIO_WriteRegister(AUDIO_AVC2_Control_1_ch23_reg, AVC2_CTRL0_REG.regValue);

    if (para == AIO_ENABLE) {
        // configure H/W-AVC2
        AVC2_CTRL0_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_1_ch23_reg);
        AVC2_CTRL0_REG.avc2_noise_gain_lsrs = t_cfg_avc2->avc2_noise_gain;
        AVC2_CTRL0_REG.avc2_noise_thre_lsrs = t_cfg_avc2->avc2_noise_thre;
        AVC2_CTRL0_REG.fast_up_db_lsrs = t_cfg_avc2->fast_up_db;
        AVC2_CTRL0_REG.slow_up_db_lsrs = t_cfg_avc2->slow_up_db;
        AVC2_CTRL0_REG.down_db_lsrs = t_cfg_avc2->down_db;
        AVC2_CTRL0_REG.avc2_vol_detect_lsrs = t_cfg_avc2->avc2_vol_detect;
        AVC2_CTRL0_REG.fast_recov_ctrl_lsrs = t_cfg_avc2->avc2_vol_amplif_enable;
        AVC2_CTRL0_REG.avc2_lpf_coef_lsrs = t_cfg_avc2->avc2_lpf_coef;
        AIO_WriteRegister(AUDIO_AVC2_Control_1_ch23_reg, AVC2_CTRL0_REG.regValue);

        AVC2_CTRL1_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_2_ch23_reg);
        AVC2_CTRL1_REG.avc2_cnt_thre_lsrs =  t_cfg_avc2->avc2_mon_win / 2;
        AVC2_CTRL1_REG.avc2_zc_win_lsrs = 0x3ff;//t_cfg_avc2->avc2_zc_win;
        AVC2_CTRL1_REG.avc2_mon_win_lsrs = t_cfg_avc2->avc2_mon_win;
        AIO_WriteRegister(AUDIO_AVC2_Control_2_ch23_reg, AVC2_CTRL1_REG.regValue);

        AVC2_CTRL2_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_3_ch23_reg);
        AVC2_CTRL2_REG.avc2_noise_thre_hi_lsrs = t_cfg_avc2->avc2_noise_thre_hi;
        AIO_WriteRegister(AUDIO_AVC2_Control_3_ch23_reg, AVC2_CTRL2_REG.regValue);

        AVC2_CTRL3_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_4_ch23_reg);
        AVC2_CTRL3_REG.avc2_level_max_lsrs = t_cfg_avc2->avc2_level_max;
        AVC2_CTRL3_REG.avc2_level_min_lsrs = t_cfg_avc2->avc2_level_min;
        AIO_WriteRegister(AUDIO_AVC2_Control_4_ch23_reg, AVC2_CTRL3_REG.regValue);

        // enable H/W-AVC again.
        //HWP_CTRL_REG.regValue = AIO_ReadRegister(AUDIO_HW_Process_Control_reg);
        //HWP_CTRL_REG.auto_volenable = AIO_ENABLE;
        //AIO_WriteRegister(AUDIO_HW_Process_Control_reg, HWP_CTRL_REG.regValue);
        hw_process_control.regValue = Audio_HwpReadRegister(AUDIO_HW_Process_Control_reg);
        hw_process_control.avc2_enable = AIO_ENABLE;    // Clayton 2013/1/28
        Audio_HwpWriteRegister(AUDIO_HW_Process_Control_reg, hw_process_control.regValue);

    }
#endif
    return AIO_OK;
}



int32_t Audio_HwpSetCH45AVC2(bool para, APP_AVC2_CFG *p_cfg_avc2)
{
#if 0	
    //hw_process_control_RBUS       HWP_CTRL_REG;
    audio_avc2_control_1_ch45_RBUS AVC2_CTRL0_REG;
    audio_avc2_control_2_ch45_RBUS AVC2_CTRL1_REG;
    audio_avc2_control_3_ch45_RBUS AVC2_CTRL2_REG;
    audio_avc2_control_4_ch45_RBUS AVC2_CTRL3_REG;

    APP_AVC2_CFG                   *t_cfg_avc2;
    audio_hw_process_control_RBUS  hw_process_control;

    if (p_cfg_avc2 == NULL) {
        t_cfg_avc2 = &m_cfg_avc2;
    }
    else {
        t_cfg_avc2 = p_cfg_avc2;
    }

    /*
     * disable H/W-AVC before configure it.
     * This register is a double buffer register,we should check it after setting,
     * but this version of code, we just assume that H/W reaction is always fast enough.
     */
    hw_process_control.regValue = Audio_HwpReadRegister(AUDIO_HW_Process_Control_reg);
    hw_process_control.avc2_enable = AIO_DISABLE;
    Audio_HwpWriteRegister(AUDIO_HW_Process_Control_reg, hw_process_control.regValue);

    AVC2_CTRL0_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_1_ch45_reg);
    AIO_WriteRegister(AUDIO_AVC2_Control_1_ch45_reg, AVC2_CTRL0_REG.regValue);

    if (para == AIO_ENABLE) {
        // configure H/W-AVC2
        AVC2_CTRL0_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_1_ch45_reg);
        AVC2_CTRL0_REG.avc2_noise_gain_csw = t_cfg_avc2->avc2_noise_gain;
        AVC2_CTRL0_REG.avc2_noise_thre_csw = t_cfg_avc2->avc2_noise_thre;
        AVC2_CTRL0_REG.fast_up_db_csw = t_cfg_avc2->fast_up_db;
        AVC2_CTRL0_REG.slow_up_db_csw = t_cfg_avc2->slow_up_db;
        AVC2_CTRL0_REG.down_db_csw = t_cfg_avc2->down_db;
        AVC2_CTRL0_REG.avc2_vol_detect_csw = t_cfg_avc2->avc2_vol_detect;
        AVC2_CTRL0_REG.fast_recov_ctrl_csw = t_cfg_avc2->avc2_vol_amplif_enable;
        AVC2_CTRL0_REG.avc2_lpf_coef_csw = t_cfg_avc2->avc2_lpf_coef;
        AIO_WriteRegister(AUDIO_AVC2_Control_1_ch45_reg, AVC2_CTRL0_REG.regValue);

        AVC2_CTRL1_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_2_ch45_reg);
        AVC2_CTRL1_REG.avc2_cnt_thre_csw =  t_cfg_avc2->avc2_mon_win / 2;
        AVC2_CTRL1_REG.avc2_zc_win_csw = 0x3ff;//t_cfg_avc2->avc2_zc_win;
        AVC2_CTRL1_REG.avc2_mon_win_csw = t_cfg_avc2->avc2_mon_win;
        AIO_WriteRegister(AUDIO_AVC2_Control_2_ch45_reg, AVC2_CTRL1_REG.regValue);

        AVC2_CTRL2_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_3_ch45_reg);
        AVC2_CTRL2_REG.avc2_noise_thre_hi_csw = t_cfg_avc2->avc2_noise_thre_hi;
        AIO_WriteRegister(AUDIO_AVC2_Control_3_ch45_reg, AVC2_CTRL2_REG.regValue);

        AVC2_CTRL3_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_4_ch45_reg);
        AVC2_CTRL3_REG.avc2_level_max_csw = t_cfg_avc2->avc2_level_max;
        AVC2_CTRL3_REG.avc2_level_min_csw = t_cfg_avc2->avc2_level_min;
        AIO_WriteRegister(AUDIO_AVC2_Control_4_ch45_reg, AVC2_CTRL3_REG.regValue);

        // enable H/W-AVC again.
        //HWP_CTRL_REG.regValue = AIO_ReadRegister(AUDIO_HW_Process_Control_reg);
        //HWP_CTRL_REG.auto_volenable = AIO_ENABLE;
        //AIO_WriteRegister(AUDIO_HW_Process_Control_reg, HWP_CTRL_REG.regValue);
        hw_process_control.regValue = Audio_HwpReadRegister(AUDIO_HW_Process_Control_reg);
        hw_process_control.avc2_enable = AIO_ENABLE;    // Clayton 2013/1/28
        Audio_HwpWriteRegister(AUDIO_HW_Process_Control_reg, hw_process_control.regValue);

    }
#endif
    return AIO_OK;
}


int32_t Audio_HwpSetCH67AVC2(bool para, APP_AVC2_CFG *p_cfg_avc2)
{
#if 0	
    //hw_process_control_RBUS       HWP_CTRL_REG;
    audio_avc2_control_1_ch67_RBUS AVC2_CTRL0_REG;
    audio_avc2_control_2_ch67_RBUS AVC2_CTRL1_REG;
    audio_avc2_control_3_ch67_RBUS AVC2_CTRL2_REG;
    audio_avc2_control_4_ch67_RBUS AVC2_CTRL3_REG;

    APP_AVC2_CFG                   *t_cfg_avc2;
    audio_hw_process_control_RBUS  hw_process_control;

    if (p_cfg_avc2 == NULL) {
        t_cfg_avc2 = &m_cfg_avc2;
    }
    else {
        t_cfg_avc2 = p_cfg_avc2;
    }

    /*
     * disable H/W-AVC before configure it.
     * This register is a double buffer register,we should check it after setting,
     * but this version of code, we just assume that H/W reaction is always fast enough.
     */
    hw_process_control.regValue = Audio_HwpReadRegister(AUDIO_HW_Process_Control_reg);
    hw_process_control.avc2_enable = AIO_DISABLE;
    Audio_HwpWriteRegister(AUDIO_HW_Process_Control_reg, hw_process_control.regValue);

    AVC2_CTRL0_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_1_ch67_reg);
    AIO_WriteRegister(AUDIO_AVC2_Control_1_ch67_reg, AVC2_CTRL0_REG.regValue);

    if (para == AIO_ENABLE) {
        // configure H/W-AVC2
        AVC2_CTRL0_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_1_ch67_reg);
        AVC2_CTRL0_REG.avc2_noise_gain_ch67 = t_cfg_avc2->avc2_noise_gain;
        AVC2_CTRL0_REG.avc2_noise_thre_ch67 = t_cfg_avc2->avc2_noise_thre;
        AVC2_CTRL0_REG.fast_up_db_ch67 = t_cfg_avc2->fast_up_db;
        AVC2_CTRL0_REG.slow_up_db_ch67 = t_cfg_avc2->slow_up_db;
        AVC2_CTRL0_REG.down_db_ch67 = t_cfg_avc2->down_db;
        AVC2_CTRL0_REG.avc2_vol_detect_ch67 = t_cfg_avc2->avc2_vol_detect;
        AVC2_CTRL0_REG.fast_recov_ctrl_ch67 = t_cfg_avc2->avc2_vol_amplif_enable;
        AVC2_CTRL0_REG.avc2_lpf_coef_ch67 = t_cfg_avc2->avc2_lpf_coef;
        AIO_WriteRegister(AUDIO_AVC2_Control_1_ch67_reg, AVC2_CTRL0_REG.regValue);

        AVC2_CTRL1_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_2_ch67_reg);
        AVC2_CTRL1_REG.avc2_cnt_thre_ch67 =  t_cfg_avc2->avc2_mon_win / 2;
        AVC2_CTRL1_REG.avc2_zc_win_ch67 = 0x3ff;//t_cfg_avc2->avc2_zc_win;
        AVC2_CTRL1_REG.avc2_mon_win_ch67 = t_cfg_avc2->avc2_mon_win;
        AIO_WriteRegister(AUDIO_AVC2_Control_2_ch67_reg, AVC2_CTRL1_REG.regValue);

        AVC2_CTRL2_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_3_ch67_reg);
        AVC2_CTRL2_REG.avc2_noise_thre_hi_ch67 = t_cfg_avc2->avc2_noise_thre_hi;
        AIO_WriteRegister(AUDIO_AVC2_Control_3_ch67_reg, AVC2_CTRL2_REG.regValue);

        AVC2_CTRL3_REG.regValue = AIO_ReadRegister(AUDIO_AVC2_Control_4_ch67_reg);
        AVC2_CTRL3_REG.avc2_level_max_ch67 = t_cfg_avc2->avc2_level_max;
        AVC2_CTRL3_REG.avc2_level_min_ch67 = t_cfg_avc2->avc2_level_min;
        AIO_WriteRegister(AUDIO_AVC2_Control_4_ch67_reg, AVC2_CTRL3_REG.regValue);

        // enable H/W-AVC again.
        //HWP_CTRL_REG.regValue = AIO_ReadRegister(AUDIO_HW_Process_Control_reg);
        //HWP_CTRL_REG.auto_volenable = AIO_ENABLE;
        //AIO_WriteRegister(AUDIO_HW_Process_Control_reg, HWP_CTRL_REG.regValue);
        hw_process_control.regValue = Audio_HwpReadRegister(AUDIO_HW_Process_Control_reg);
        hw_process_control.avc2_enable = AIO_ENABLE;    // Clayton 2013/1/28
        Audio_HwpWriteRegister(AUDIO_HW_Process_Control_reg, hw_process_control.regValue);

    }
#endif
    return AIO_OK;
}

