#ifndef BUILD_QUICK_SHOW
#include <linux/module.h>
#include <linux/export.h>
#else   /* !BUILD_QUICK_SHOW */
#include <no_os/export.h>
#endif  /* !BUILD_QUICK_SHOW */

#include <rtk_kdriver/io.h>
#include <rtk_kdriver/quick_show/quick_show.h>
#include <scaler/scalerCommon.h>


bool in_quick_show = false;


static inline bool _is_QS_magic_valid(unsigned int val)
{
    return (val & QS_STATUS_magic_mask) == (QS_STATUS_magic_valid_data << QS_STATUS_magic_shift);
}


static inline bool _is_QS_active(unsigned int val)
{
    return _is_QS_magic_valid(val) && (val & QS_STATUS_active_mask);
}


bool is_QS_active(void)
{
    return _is_QS_active(rtd_inl(QS_STATUS_reg));
}
EXPORT_SYMBOL(is_QS_active);


bool is_QS_hdmi_enable(void)
{
    unsigned int val;

    val = rtd_inl(QS_STATUS_reg);
    return _is_QS_active(val) && (val & QS_STATUS_hdmi_mask);
}
EXPORT_SYMBOL(is_QS_hdmi_enable);


bool is_QS_dp_enable(void)
{
    unsigned int val;

    val = rtd_inl(QS_STATUS_reg);
    return _is_QS_active(val) && (val & QS_STATUS_dp_mask);
}
EXPORT_SYMBOL(is_QS_dp_enable);


bool is_QS_panel_enable(void)
{
    unsigned int val;

    val = rtd_inl(QS_STATUS_reg);
    return _is_QS_active(val) && (val & QS_STATUS_panel_mask);
}
EXPORT_SYMBOL(is_QS_panel_enable);


bool is_QS_scaler_enable(void)
{
    unsigned int val;

    val = rtd_inl(QS_STATUS_reg);
    return _is_QS_active(val) && (val & QS_STATUS_scaler_mask);
}
EXPORT_SYMBOL(is_QS_scaler_enable);


bool is_QS_i2c_enable(void)
{
    unsigned int val;

    val = rtd_inl(QS_STATUS_reg);
    return _is_QS_active(val) && (val & QS_STATUS_i2c_mask);
}
EXPORT_SYMBOL(is_QS_i2c_enable);


bool is_QS_pq_enable(void)
{
    unsigned int val;

    val = rtd_inl(QS_STATUS_reg);
    return _is_QS_active(val) && (val & QS_STATUS_pq_mask);
}
EXPORT_SYMBOL(is_QS_pq_enable);


bool is_QS_acpu_enable(void)
{
    unsigned int val;

    val = rtd_inl(QS_STATUS_reg);
    return _is_QS_magic_valid(val) && (val & QS_STATUS_acpu_mask);
}
EXPORT_SYMBOL(is_QS_acpu_enable);


bool is_QS_vcpu_enable(void)
{
    unsigned int val;

    val = rtd_inl(QS_STATUS_reg);
    return _is_QS_magic_valid(val) && (val & QS_STATUS_vcpu_mask);
}
EXPORT_SYMBOL(is_QS_vcpu_enable);


bool is_QS_amp_enable(void)
{
    unsigned int val;

    val = rtd_inl(QS_STATUS_reg);
    return _is_QS_active(val) && (val & QS_STATUS_amp_mask);
}
EXPORT_SYMBOL(is_QS_amp_enable);


bool is_QS_hp_enable(void)
{
    unsigned int val;

    val = rtd_inl(QS_STATUS_reg);
    return _is_QS_active(val) && (val & QS_STATUS_hp_mask);
}
EXPORT_SYMBOL(is_QS_hp_enable);


bool is_QS_typec_enable(void)
{
    unsigned int val;

    val = rtd_inl(QS_STATUS_reg);
    return _is_QS_active(val) && (val & QS_STATUS_typec_mask);
}
EXPORT_SYMBOL(is_QS_typec_enable);

bool is_QS_eco_mode(void)
{
#ifndef BUILD_QUICK_SHOW
    unsigned int val;

    val = rtd_inl(QS_STATUS_reg);
    return _is_QS_active(val) && (rtd_inl(QS_STATUS_2_reg) & QS_STATUS_2_ECO_MODE_mask);
#else
    extern bool qs_eco_mode;
    return qs_eco_mode;
#endif
}
EXPORT_SYMBOL(is_QS_eco_mode);


int get_QS_portnum(void)
{
    unsigned int val;

    val = rtd_inl(QS_STATUS_reg);
    if (_is_QS_active(val))
        return (val & QS_STATUS_portnum_mask) >> QS_STATUS_portnum_shift;
    else
        return -ENODEV;
}
EXPORT_SYMBOL(get_QS_portnum);


int get_QS_volume(void)
{
    unsigned int val;

    val = rtd_inl(QS_STATUS_reg);
    if (_is_QS_active(val))
        return (val & QS_STATUS_volume_mask) >> QS_STATUS_volume_shift;
    else
        return -ENODEV;
}
EXPORT_SYMBOL(get_QS_volume);


int get_QS_audio_balance(void)
{
    unsigned int val;
    int balance = 0;

    if (is_QS_active()) {
        val = rtd_inl(QS_STATUS_2_reg);
        balance = ((val & QS_STATUS_2_balance_mask) >> QS_STATUS_2_balance_shift) * db_per_balance;

        if (val & QS_STATUS_2_balance_lr_mask) // left
            balance = -balance;
    }

    return balance;
}
EXPORT_SYMBOL(get_QS_audio_balance);


int get_QS_mute(void)
{
    unsigned int val;

    val = rtd_inl(QS_STATUS_reg);
    if (_is_QS_active(val))
        return (val & QS_STATUS_mute_mask) >> QS_STATUS_mute_shift;
    else
        return -ENODEV;
}
EXPORT_SYMBOL(get_QS_mute);


bool is_QS_game_mode(void)
{
#ifndef BUILD_QUICK_SHOW
    unsigned int val;

    val = rtd_inl(QS_STATUS_reg);
    return _is_QS_active(val) && (rtd_inl(QS_STATUS_2_reg) & QS_STATUS_2_PIC_MODE_GAME_mask);
#else
    extern bool qs_game_mode;
    return qs_game_mode;
#endif
}
EXPORT_SYMBOL(is_QS_game_mode);


PC_MODE_INFO get_QS_pc_mode(void)
{
#ifndef BUILD_QUICK_SHOW
    unsigned int val;
    PC_MODE_INFO qs_pc_mode = PC_MODE_UNKNOWN;

    val = rtd_inl(QS_STATUS_reg);
    if (_is_QS_active(val))
        qs_pc_mode = (rtd_inl(QS_STATUS_2_reg) & QS_STATUS_2_PIC_MODE_PC_mask)>>QS_STATUS_2_PIC_MODE_PC_shift;

    return qs_pc_mode;
#else
    extern PC_MODE_INFO qs_pc_mode;
    return qs_pc_mode;
#endif
}
EXPORT_SYMBOL(get_QS_pc_mode);

LOW_LATENCY_MODE_INFO get_QS_low_latency_mode(void)
{
#ifndef BUILD_QUICK_SHOW
    unsigned int val;
    LOW_LATENCY_MODE_INFO qs_low_latency_mode = LOW_LATENCY_MODE_DISABLE;

    val = rtd_inl(QS_STATUS_reg);
    if (_is_QS_active(val))
        qs_low_latency_mode = (rtd_inl(QS_STATUS_2_reg) & QS_STATUS_2_LOW_LATENCY_MODE_mask)>>QS_STATUS_2_LOW_LATENCY_MODE_shift;

    return qs_low_latency_mode;
#else
    extern LOW_LATENCY_MODE_INFO qs_low_latency_mode;
    return qs_low_latency_mode;
#endif
}
EXPORT_SYMBOL(get_QS_low_latency_mode);

bool is_QS_freesync_enable(void)
{
#ifndef BUILD_QUICK_SHOW
    unsigned int val;

    val = rtd_inl(QS_STATUS_reg);
    return _is_QS_active(val) && (rtd_inl(QS_STATUS_2_reg) & QS_STATUS_2_FREESYNC_EN_mask);
#else
    extern bool qs_freesync_en;
    return qs_freesync_en;
#endif
}
EXPORT_SYMBOL(is_QS_freesync_enable);


void set_QS_inactive(void)
{
    rtd_clearbits(QS_STATUS_reg, QS_STATUS_active_mask);
}
EXPORT_SYMBOL(set_QS_inactive);


MODULE_LICENSE("GPL");
