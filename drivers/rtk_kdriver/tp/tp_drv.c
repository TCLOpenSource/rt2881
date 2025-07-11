/******************************************************************************
 *
 *   Copyright(c) 2014 Realtek Semiconductor Corp. All rights reserved.
 *
 *   @author yping@realtek.com
 *
 *****************************************************************************/
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/of.h>
#include <mach/rtk_platform.h>

#include <base_types.h>
#include <rtk_kdriver/rtk_crt.h>
#include <rbus/sys_reg_reg.h>

#include "tp_soc.h"
#include <tp/tp_dbg.h>
#include <tp/tp_drv_global.h>
#include <tp/tp_drv_verify.h>
#include <tp/tp_drv.h>
#include <tp/tp_reg_ctrl.h>
#include <tp/tp_drv_api.h>

#ifdef CONFIG_RTK_TP_WITH_MIO
#include <rbus/iso_spi_synchronizer_reg.h>
#endif

#ifdef SUPPORT_TP_CP_SAVING_MODE
extern bool mcp_enable_cp_power_saving(bool on);
#endif

/*-----------------------------------------------------------------------------------
 * File Operations
 *------------------------------------------------------------------------------------*/

#ifdef CONFIG_RTK_KDRV_MIO
extern int get_mio_enable_status(void);
#endif

int tp_open(struct inode *inode, struct file *file)
{
        file->private_data = (void*) pTp_drv;

        return (file->private_data) ? 0 : -ENOMEM;
}


int tp_release(struct inode *inode, struct file *file)
{
        return 0;
}

long tp_ioctl(
        struct file*            file,
        unsigned int            cmd,
        unsigned long           arg
)
{
        return TPVerify_ioctl(file, cmd, arg);
}

#ifdef CONFIG_COMPAT
long compat_tp_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
        void __user *compat_arg = compat_ptr(arg);
        return tp_ioctl(filp, cmd, (unsigned long)compat_arg);
}
#endif


static struct file_operations tp_fops = {
        .unlocked_ioctl = tp_ioctl,
#ifdef CONFIG_COMPAT
        .compat_ioctl   = compat_tp_ioctl,
#endif

        .open           = tp_open,
        .release        = tp_release,
};

/***************************************************************************
         ------------------- Power Management ----------------
****************************************************************************/
#ifdef CONFIG_PM
static TP_REG_INFO tp_reg_data;

TPK_TP_CLK_STU_T check_tp_clk_status(void)
{
        TP_DBG_SIMPLE( "[%s] clk:0x%x, srst:0x%x\n", __func__, SYS_REG_SYS_CLKEN1_get_clken_tp(rtd_inl(SYS_REG_SYS_CLKEN1_reg)), SYS_REG_SYS_SRST1_get_rstn_tp(rtd_inl(SYS_REG_SYS_SRST1_reg)));

        if ( SYS_REG_SYS_CLKEN1_get_clken_tp(rtd_inl(SYS_REG_SYS_CLKEN1_reg)) &&
             SYS_REG_SYS_SRST1_get_rstn_tp(rtd_inl(SYS_REG_SYS_SRST1_reg)) )
                return TP_CLK_ON;
        else
                return TP_CLK_OFF;

}

int save_tp_reigster_value_ex(void)
{
        REGADDR tp_tf_cntl_addr;
        REGADDR tp_tf_frmcfg_addr;
        REGADDR tp_tf_int_addr;
        REGADDR tp_tf_int_en_addr;

        REGADDR tp_pcr_ctl_addr;

        REGADDR tp_tp_des_cntl_addr;

        REGADDR tp_m2m_ring_limit_addr;
        REGADDR tp_m2m_ring_base_addr;
        REGADDR tp_m2m_ring_rp_addr;
        REGADDR tp_m2m_ring_wp_addr;
        REGADDR tp_m2m_ring_ctrl_addr;

#ifdef TP_SUPPORT_CI
        REGADDR tp_p_m2m_ring_limit_addr;
        REGADDR tp_p_m2m_ring_base_addr;
        REGADDR tp_p_m2m_ring_rp_addr;
        REGADDR tp_p_m2m_ring_wp_addr;
        REGADDR tp_p_m2m_ring_ctrl_addr;

        REGADDR tp_p_tf_cntl_addr;
        REGADDR tp_p_tf_frmcfg_addr;
        REGADDR tp_p_tf_int_addr;
        REGADDR tp_p_tf_int_en_addr;
#endif

        tp_tp_pid_ctrl_RBUS        tp_pid_ctrl_reg;
        tp_tp_sec_ctrl_RBUS        tp_sec_ctrl_reg;
//        tp_tp_key_ctrl_RBUS        tp_key_ctrl_reg;
        tp_tp_ring_ctrl_RBUS       tp_ring_ctrl_reg;
#ifdef TP_SUPPORT_CI
        tp_tp_p_ring_ctrl_RBUS     tp_p_ring_ctrl_reg;
#endif

        TPK_TP_ENGINE_T tp_id;

        UINT8 pid_idx;
        UINT8 sec_idx;
//        UINT8 key_idx;
        UINT8 buf_idx;

        for (tp_id = 0; tp_id < MAX_TP_COUNT; tp_id++) {

                switch (tp_id) {
                        case TP_TP0: {
                                /* tp */
                                tp_tf_cntl_addr          = TP_TP_TF0_CNTL_reg;
                                tp_tf_frmcfg_addr        = TP_TP_TF0_FRMCFG_reg;
                                tp_tf_int_addr           = TP_TP_TF0_INT_reg;
                                tp_tf_int_en_addr        = TP_TP_TF0_INT_EN_reg;
                                /* pcr */
                                tp_pcr_ctl_addr          = TP_TP0_PCR_CTL_reg;
                                /* descrambler */
                                tp_tp_des_cntl_addr      = TP_TP_TP0_DES_CNTL_reg;
                                /* tp mtp */
                                tp_m2m_ring_limit_addr   = TP_TP0_M2M_RING_LIMIT_reg;
                                tp_m2m_ring_base_addr    = TP_TP0_M2M_RING_BASE_reg;
                                tp_m2m_ring_rp_addr      = TP_TP0_M2M_RING_RP_reg;
                                tp_m2m_ring_wp_addr      = TP_TP0_M2M_RING_WP_reg;
                                tp_m2m_ring_ctrl_addr    = TP_TP0_M2M_RING_CTRL_reg;
#ifdef TP_SUPPORT_CI
                                /* tpp mtp */
                                tp_p_m2m_ring_limit_addr = TP_TP0_P_M2M_RING_LIMIT_reg;
                                tp_p_m2m_ring_base_addr  = TP_TP0_P_M2M_RING_BASE_reg;
                                tp_p_m2m_ring_rp_addr    = TP_TP0_P_M2M_RING_RP_reg;
                                tp_p_m2m_ring_wp_addr    = TP_TP0_P_M2M_RING_WP_reg;
                                tp_p_m2m_ring_ctrl_addr  = TP_TP0_P_M2M_RING_CTRL_reg;
                                /* tpp */
                                tp_p_tf_cntl_addr        = TP_TP_TF0_P_CNTL_reg;
                                tp_p_tf_frmcfg_addr      = TP_TP_TF0_P_FRMCFG_reg;
                                tp_p_tf_int_addr         = TP_TP_TF0_P_INT_reg;
                                tp_p_tf_int_en_addr      = TP_TP_TF0_P_INT_EN_reg;
#endif
                                break;
                        }
                        case TP_TP1: {
                                /* tp */
                                tp_tf_cntl_addr          = TP_TP_TF1_CNTL_reg;
                                tp_tf_frmcfg_addr        = TP_TP_TF1_FRMCFG_reg;
                                tp_tf_int_addr           = TP_TP_TF1_INT_reg;
                                tp_tf_int_en_addr        = TP_TP_TF1_INT_EN_reg;
                                /* pcr */
                                tp_pcr_ctl_addr          = TP_TP1_PCR_CTL_reg;
                                /* descrambler */
                                tp_tp_des_cntl_addr      = TP_TP_TP1_DES_CNTL_reg;
                                /* tp mtp */
                                tp_m2m_ring_limit_addr   = TP_TP1_M2M_RING_LIMIT_reg;
                                tp_m2m_ring_base_addr    = TP_TP1_M2M_RING_BASE_reg;
                                tp_m2m_ring_rp_addr      = TP_TP1_M2M_RING_RP_reg;
                                tp_m2m_ring_wp_addr      = TP_TP1_M2M_RING_WP_reg;
                                tp_m2m_ring_ctrl_addr    = TP_TP1_M2M_RING_CTRL_reg;
#ifdef TP_SUPPORT_CI
                                /* tpp mtp */
                                tp_p_m2m_ring_limit_addr = TP_TP1_P_M2M_RING_LIMIT_reg;
                                tp_p_m2m_ring_base_addr  = TP_TP1_P_M2M_RING_BASE_reg;
                                tp_p_m2m_ring_rp_addr    = TP_TP1_P_M2M_RING_RP_reg;
                                tp_p_m2m_ring_wp_addr    = TP_TP1_P_M2M_RING_WP_reg;
                                tp_p_m2m_ring_ctrl_addr  = TP_TP1_P_M2M_RING_CTRL_reg;
                                /* tpp */
                                tp_p_tf_cntl_addr        = TP_TP_TF1_P_CNTL_reg;
                                tp_p_tf_frmcfg_addr      = TP_TP_TF1_P_FRMCFG_reg;
                                tp_p_tf_int_addr         = TP_TP_TF1_P_INT_reg;
                                tp_p_tf_int_en_addr      = TP_TP_TF1_P_INT_EN_reg;
#endif

                                break;
                        }
                        case TP_TP2: {
                                /* tp */
                                tp_tf_cntl_addr          = TP_TP_TF2_CNTL_reg;
                                tp_tf_frmcfg_addr        = TP_TP_TF2_FRMCFG_reg;
                                tp_tf_int_addr           = TP_TP_TF2_INT_reg;
                                tp_tf_int_en_addr        = TP_TP_TF2_INT_EN_reg;
                                /* pcr */
                                tp_pcr_ctl_addr          = TP_TP2_PCR_CTL_reg;
                                /* descrambler */
                                tp_tp_des_cntl_addr      = TP_TP_TP2_DES_CNTL_reg;
                                /* tp mtp */
                                tp_m2m_ring_limit_addr   = TP_TP2_M2M_RING_LIMIT_reg;
                                tp_m2m_ring_base_addr    = TP_TP2_M2M_RING_BASE_reg;
                                tp_m2m_ring_rp_addr      = TP_TP2_M2M_RING_RP_reg;
                                tp_m2m_ring_wp_addr      = TP_TP2_M2M_RING_WP_reg;
                                tp_m2m_ring_ctrl_addr    = TP_TP2_M2M_RING_CTRL_reg;
#ifdef TP_SUPPORT_CI
                                /* tpp mtp */
                                tp_p_m2m_ring_limit_addr = TP_TP2_P_M2M_RING_LIMIT_reg;
                                tp_p_m2m_ring_base_addr  = TP_TP2_P_M2M_RING_BASE_reg;
                                tp_p_m2m_ring_rp_addr    = TP_TP2_P_M2M_RING_RP_reg;
                                tp_p_m2m_ring_wp_addr    = TP_TP2_P_M2M_RING_WP_reg;
                                tp_p_m2m_ring_ctrl_addr  = TP_TP2_P_M2M_RING_CTRL_reg;
                                /* tpp */
                                tp_p_tf_cntl_addr        = TP_TP_TF2_P_CNTL_reg;
                                tp_p_tf_frmcfg_addr      = TP_TP_TF2_P_FRMCFG_reg;
                                tp_p_tf_int_addr         = TP_TP_TF2_P_INT_reg;
                                tp_p_tf_int_en_addr      = TP_TP_TF2_P_INT_EN_reg;
#endif
                                break;
                        }
#ifdef TP_SUPPORT_TP3
                        case TP_TP3: {
                                /* tp */
                                tp_tf_cntl_addr          = TP_TP_TF3_CNTL_reg;
                                tp_tf_frmcfg_addr        = TP_TP_TF3_FRMCFG_reg;
                                tp_tf_int_addr           = TP_TP_TF3_INT_reg;
                                tp_tf_int_en_addr        = TP_TP_TF3_INT_EN_reg;
                                /* pcr */
                                tp_pcr_ctl_addr          = TP_TP3_PCR_CTL_reg;
                                /* TP3 do not support descrambler */
                                /* tp mtp */
                                tp_m2m_ring_limit_addr   = TP_TP3_M2M_RING_LIMIT_reg;
                                tp_m2m_ring_base_addr    = TP_TP3_M2M_RING_BASE_reg;
                                tp_m2m_ring_rp_addr      = TP_TP3_M2M_RING_RP_reg;
                                tp_m2m_ring_wp_addr      = TP_TP3_M2M_RING_WP_reg;
                                tp_m2m_ring_ctrl_addr    = TP_TP3_M2M_RING_CTRL_reg;
                                /* there is no tpp3 mtp */
                                /* three is no tpp */
                                break;
                        }
#endif
                        default:
                                break;
                }

                /* tp */
                tp_reg_data.tp_tf_cntl_reg[tp_id].regValue   = READ_REG32(tp_tf_cntl_addr);
                tp_reg_data.tp_tf_frmcfg_reg[tp_id].regValue = READ_REG32(tp_tf_frmcfg_addr);
                tp_reg_data.tp_tf_int_reg[tp_id].regValue    = READ_REG32(tp_tf_int_addr);
                tp_reg_data.tp_tf_int_en_reg[tp_id].regValue = READ_REG32(tp_tf_int_en_addr);

                /* pcr */
                tp_reg_data.tp_pcr_ctl_reg[tp_id].regValue = READ_REG32(tp_pcr_ctl_addr);

                /* tp mtp */
                tp_reg_data.tp_m2m_ring_limit_reg[tp_id].regValue = READ_REG32(tp_m2m_ring_limit_addr);
                tp_reg_data.tp_m2m_ring_base_reg[tp_id].regValue  = READ_REG32(tp_m2m_ring_base_addr);
                tp_reg_data.tp_m2m_ring_rp_reg[tp_id].regValue    = READ_REG32(tp_m2m_ring_rp_addr);
                tp_reg_data.tp_m2m_ring_wp_reg[tp_id].regValue    = READ_REG32(tp_m2m_ring_wp_addr);
                tp_reg_data.tp_m2m_ring_ctrl_reg[tp_id].regValue  = READ_REG32(tp_m2m_ring_ctrl_addr);


                if ( tp_id != TP_TP3 ) {
                        /* des & tdes */
                        tp_reg_data.tp_tp_des_cntl_reg[tp_id].regValue = READ_REG32(tp_tp_des_cntl_addr);
#ifdef TP_SUPPORT_CI
                        /* tpp mtp */
                        tp_reg_data.tp_p_m2m_ring_limit_reg[tp_id].regValue = READ_REG32(tp_p_m2m_ring_limit_addr);
                        tp_reg_data.tp_p_m2m_ring_base_reg[tp_id].regValue  = READ_REG32(tp_p_m2m_ring_base_addr);
                        tp_reg_data.tp_p_m2m_ring_rp_reg[tp_id].regValue    = READ_REG32(tp_p_m2m_ring_rp_addr);
                        tp_reg_data.tp_p_m2m_ring_wp_reg[tp_id].regValue    = READ_REG32(tp_p_m2m_ring_wp_addr);
                        tp_reg_data.tp_p_m2m_ring_ctrl_reg[tp_id].regValue  = READ_REG32(tp_p_m2m_ring_ctrl_addr);

                        /* tpp */
                        tp_reg_data.tp_tf_p_cntl_reg[tp_id].regValue   = READ_REG32(tp_p_tf_cntl_addr);
                        tp_reg_data.tp_tf_p_frmcfg_reg[tp_id].regValue = READ_REG32(tp_p_tf_frmcfg_addr);
                        tp_reg_data.tp_tf_p_int_reg[tp_id].regValue    = READ_REG32(tp_p_tf_int_addr);
                        tp_reg_data.tp_tf_p_int_en_reg[tp_id].regValue = READ_REG32(tp_p_tf_int_en_addr);
                        tp_reg_data.tp_out_int_reg.regValue       = READ_REG32(TP_TP_OUT_INT_reg);
#endif

                }
        }

        /* tpout */
        tp_reg_data.tp_tf_out_frmcfg_reg.regValue = READ_REG32(TP_TP_TF_OUT_FRMCFG_reg);
#ifdef TP_SUPPORT_CI
        tp_reg_data.tp_out_ctrl_reg.regValue      = READ_REG32(TP_TP_OUT_CTRL_reg);
        tp_reg_data.tp_out_int_reg.regValue       = READ_REG32(TP_TP_OUT_INT_reg);
#endif

        /* tp input control */
        tp_reg_data.tp_in_pol_reg.regValue        = READ_REG32(TP_TP_IN_POL_reg);
#ifdef TP_SUPPORT_CI
        /* sync replace */
        tp_reg_data.tp_sync_rplace_reg.regValue   = READ_REG32(TP_TP_SYNC_RPLACE_reg);
#endif

        /* pid filter */
        tp_reg_data.tp_pid_part_reg.regValue      = READ_REG32(TP_TP_PID_PART_reg);
        for (pid_idx = 0; pid_idx < MAX_PID_FILTER_COUNT; pid_idx++) {
                tp_pid_ctrl_reg.regValue = 0;
                tp_pid_ctrl_reg.idx      = pid_idx;
                WRITE_REG32(TP_TP_PID_CTRL_reg, tp_pid_ctrl_reg.regValue);

                tp_reg_data.tp_pid_data_reg[pid_idx].regValue  = READ_REG32(TP_TP_PID_DATA_reg);
                tp_reg_data.tp_pid_data2_reg[pid_idx].regValue = READ_REG32(TP_TP_PID_DATA2_reg);
                tp_reg_data.tp_pid_data3_reg[pid_idx].regValue = READ_REG32(TP_TP_PID_DATA3_reg);
        }
        /* section filter */
        for (sec_idx = 0; sec_idx < MAX_SEC_COUNT; sec_idx++) {
                tp_sec_ctrl_reg.regValue = 0;
                tp_sec_ctrl_reg.idx      = sec_idx;
                WRITE_REG32(TP_TP_SEC_CTRL_reg, tp_sec_ctrl_reg.regValue);

                tp_reg_data.tp_sec_data0_reg[sec_idx].regValue = READ_REG32(TP_TP_SEC_DATA0_reg);
                tp_reg_data.tp_sec_data1_reg[sec_idx].regValue = READ_REG32(TP_TP_SEC_DATA1_reg);
                tp_reg_data.tp_sec_data2_reg[sec_idx].regValue = READ_REG32(TP_TP_SEC_DATA2_reg);
                tp_reg_data.tp_sec_data3_reg[sec_idx].regValue = READ_REG32(TP_TP_SEC_DATA3_reg);
                tp_reg_data.tp_sec_data4_reg[sec_idx].regValue = READ_REG32(TP_TP_SEC_DATA4_reg);
                tp_reg_data.tp_sec_data5_reg[sec_idx].regValue = READ_REG32(TP_TP_SEC_DATA5_reg);
                tp_reg_data.tp_sec_data6_reg[sec_idx].regValue = READ_REG32(TP_TP_SEC_DATA6_reg);
                tp_reg_data.tp_sec_data7_reg[sec_idx].regValue = READ_REG32(TP_TP_SEC_DATA7_reg);
                tp_reg_data.tp_sec_data8_reg[sec_idx].regValue = READ_REG32(TP_TP_SEC_DATA8_reg);
                tp_reg_data.tp_sec_data9_reg[sec_idx].regValue = READ_REG32(TP_TP_SEC_DATA9_reg);
        }

#if 0
        /* des & tdes */
        for (key_idx = 0; key_idx < MAX_KEY_COUNT; key_idx++) {
                tp_key_ctrl_reg.regValue = 0;
                tp_key_ctrl_reg.idx      = key_idx;
                WRITE_REG32(TP_TP_KEY_CTRL_reg, tp_key_ctrl_reg.regValue);

                tp_reg_data.tp_key_info_reg[key_idx].regValue  = READ_REG32(TP_TP_KEY_INFO_0_reg);
                tp_reg_data.tp_key_info2_reg[key_idx].regValue = READ_REG32(TP_TP_KEY_INFO_1_reg);
        }
#endif

        /* crc */
        tp_reg_data.tp_crc_init_reg.regValue = READ_REG32(TP_TP_CRC_INIT_reg);

        /* tp ddr ring buffer */
        tp_reg_data.tp_threshold_reg.regValue = READ_REG32(TP_TP_THRESHOLD_reg);
        tp_reg_data.tp_fullness_reg.regValue  = READ_REG32(TP_TP_FULLNESS_reg);
        for (buf_idx = 0; buf_idx < MAX_TP_BUFFER_COUNT; buf_idx++) {
                tp_ring_ctrl_reg.regValue = 0;
                tp_ring_ctrl_reg.idx      = buf_idx;
                WRITE_REG32(TP_TP_RING_CTRL_reg, tp_ring_ctrl_reg.regValue);

                tp_reg_data.tp_ring_limit_reg[buf_idx].regValue = READ_REG32(TP_TP_RING_LIMIT_reg);
                tp_reg_data.tp_ring_base_reg[buf_idx].regValue  = READ_REG32(TP_TP_RING_BASE_reg);
                tp_reg_data.tp_ring_rp_reg[buf_idx].regValue    = READ_REG32(TP_TP_RING_RP_reg);
                tp_reg_data.tp_ring_wp_reg[buf_idx].regValue    = READ_REG32(TP_TP_RING_WP_reg);
        }
        tp_reg_data.tp_ring_avail_int_reg.regValue     = READ_REG32(TP_TP_RING_AVAIL_INT_0_reg);
        tp_reg_data.tp_ring_avail_int2_reg.regValue    = READ_REG32(TP_TP_RING_AVAIL_INT_1_reg);
        tp_reg_data.tp_ring_full_int_reg.regValue      = READ_REG32(TP_TP_RING_FULL_INT_0_reg);
        tp_reg_data.tp_ring_full_int2_reg.regValue     = READ_REG32(TP_TP_RING_FULL_INT_1_reg);
        tp_reg_data.tp_ring_avail_int_en_reg.regValue  = READ_REG32(TP_TP_RING_AVAIL_INT_EN_0_reg);
        tp_reg_data.tp_ring_avail_int_en2_reg.regValue = READ_REG32(TP_TP_RING_AVAIL_INT_EN_1_reg);
        tp_reg_data.tp_ring_full_int_en_reg.regValue   = READ_REG32(TP_TP_RING_FULL_INT_EN_0_reg);
        tp_reg_data.tp_ring_full_int_en2_reg.regValue  = READ_REG32(TP_TP_RING_FULL_INT_EN_1_reg);

#ifdef TP_SUPPORT_CI
        /* tp_p ring buffer */
        tp_reg_data.tp_p_threshold_reg.regValue = READ_REG32(TP_TP_P_THRESHOLD_reg);
        tp_reg_data.tp_p_fullness_reg.regValue  = READ_REG32(TP_TP_P_FULLNESS_reg);

        for (buf_idx = 0; buf_idx < MAX_TPP_BUFFER_COUNT; buf_idx++) {
                tp_p_ring_ctrl_reg.regValue = 0;
                tp_p_ring_ctrl_reg.idx      = buf_idx;
                WRITE_REG32(TP_TP_P_RING_CTRL_reg, tp_p_ring_ctrl_reg.regValue);

                /* clean tp_p ring buff data*/
                tp_reg_data.tp_p_ring_limit_reg[buf_idx].regValue = READ_REG32(TP_TP_P_RING_LIMIT_reg);
                tp_reg_data.tp_p_ring_base_reg[buf_idx].regValue  = READ_REG32(TP_TP_P_RING_BASE_reg);
                tp_reg_data.tp_p_ring_rp_reg[buf_idx].regValue    = READ_REG32(TP_TP_P_RING_BASE_reg);
                tp_reg_data.tp_p_ring_wp_reg[buf_idx].regValue    = READ_REG32(TP_TP_P_RING_BASE_reg);
        }

        tp_reg_data.tp_p_ring_avail_int_reg.regValue    = READ_REG32(TP_TP_P_RING_AVAIL_INT_reg);
        tp_reg_data.tp_p_ring_full_int_reg.regValue     = READ_REG32(TP_TP_P_RING_FULL_INT_reg);
        tp_reg_data.tp_p_ring_avail_int_en_reg.regValue = READ_REG32(TP_TP_P_RING_AVAIL_INT_EN_reg);
        tp_reg_data.tp_p_ring_full_int_en_reg.regValue  = READ_REG32(TP_TP_P_RING_FULL_INT_EN_reg);
#endif
        /* tp_p dmy_a & dmy_b */
        tp_reg_data.tp_dmy_a_reg.regValue = READ_REG32(TP_TP_DMY_A_reg);
        tp_reg_data.tp_dmy_b_reg.regValue = READ_REG32(TP_TP_DMY_B_reg);
#ifdef TP_SUPPORT_TP3
        /* tp_p sync replace */
        tp_reg_data.tp_p_sync_rplace_reg.regValue = READ_REG32(TP_TP_P_SYNC_RPLACE_reg);
#endif
        /* frame rate control */
        tp_reg_data.prefix_fr_clk_rate_reg.regValue  = READ_REG32(TP_PREFIX_FR_CLK_RATE_reg);
        tp_reg_data.prefix_ctrl_reg.regValue         = READ_REG32(TP_PREFIX_CTRL_reg);
        tp_reg_data.prefix_data1_reg.regValue        = READ_REG32(TP_PREFIX_DATA1_reg);
        tp_reg_data.prefix_data2_reg.regValue        = READ_REG32(TP_PREFIX_DATA2_reg);
        tp_reg_data.fr_ctrl_reg.regValue             = READ_REG32(TP_FR_CTRL_reg);

        /* start code search */
        tp_reg_data.start_code_tp0_reg.regValue      = READ_REG32(TP_START_CODE_TP0_reg);
        tp_reg_data.start_code_tp1_reg.regValue      = READ_REG32(TP_START_CODE_TP1_reg);
        tp_reg_data.start_code_ctrl_tp0_reg.regValue = READ_REG32(TP_START_CODE_CTRL_TP0_reg);
        tp_reg_data.start_code_ctrl_tp1_reg.regValue = READ_REG32(TP_START_CODE_CTRL_TP1_reg);

        /* Others */
        tp_reg_data.tp0_err_cnt_en.regValue = READ_REG32(TP_TP0_ERR_CNT_EN_reg);
        tp_reg_data.tp1_err_cnt_en.regValue = READ_REG32(TP_TP1_ERR_CNT_EN_reg);

        /* TPO CLK */
        tp_reg_data.tpout_clk_reg  = READ_REG32(TP_TP_OUT_CLOCK_reg);

        tp_reg_data.tpout_pinshare_buf_th = READ_REG32(TP_TP_OUT_SHARE_reg);

        /* ATSC 30 */
        tp_reg_data.tp_atsc_src_cntl.regValue     = READ_REG32(TP_TP_ATSC_SRC_CNTL_reg);
        tp_reg_data.tp_atsc_config1.regValue      = READ_REG32(TP_TP_ATSC_CONFIG1_reg);
        tp_reg_data.tp_atsc_config2.regValue      = READ_REG32(TP_TP_ATSC_CONFIG2_reg);
        tp_reg_data.tp_atsc_config3.regValue      = READ_REG32(TP_TP_ATSC_CONFIG3_reg);
        tp_reg_data.tp_atsc_plp_filter1.regValue  = READ_REG32(TP_TP_ATSC_PLP_FILTER1_reg);
        tp_reg_data.tp_atsc_plp_filter2.regValue  = READ_REG32(TP_TP_ATSC_PLP_FILTER2_reg);
        tp_reg_data.tp_atsc_d_limit.regValue      = READ_REG32(TP_TP_ATSC_D_LIMIT_reg);
        tp_reg_data.tp_atsc_d_base.regValue       = READ_REG32(TP_TP_ATSC_D_BASE_reg);
        tp_reg_data.tp_atsc_d_rp.regValue         = READ_REG32(TP_TP_ATSC_D_RP_reg);
        tp_reg_data.tp_atsc_d_wp.regValue         = READ_REG32(TP_TP_ATSC_D_WP_reg);
        tp_reg_data.tp_atsc_i_limit.regValue      = READ_REG32(TP_TP_ATSC_I_LIMIT_reg);
        tp_reg_data.tp_atsc_i_base.regValue       = READ_REG32(TP_TP_ATSC_I_BASE_reg);
        tp_reg_data.tp_atsc_i_rp.regValue         = READ_REG32(TP_TP_ATSC_I_RP_reg);
        tp_reg_data.tp_atsc_i_wp.regValue         = READ_REG32(TP_TP_ATSC_I_WP_reg);
#ifdef CONFIG_RTK_TP_WITH_MIO
        if(get_mio_enable_status()){
        /* mio TP */
        tp_reg_data.mio_tp_ctrl.regValue         = READ_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_CTRL_reg);
        tp_reg_data.mio_tp_t2p_cfg.regValue      = READ_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_T2P_CFG_reg);
        tp_reg_data.mio_tp_t2p_ctrl.regValue     = READ_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_T2P_CTRL_reg);
        tp_reg_data.mio_tp_p2t_cfg.regValue      = READ_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_P2T_CFG_reg);
        tp_reg_data.mio_tp_p2t_ctrl.regValue     = READ_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_P2T_CTRL_reg);
        tp_reg_data.mio_tp_dbg_ctrl.regValue     = READ_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_DBG_CTRL_reg);
        tp_reg_data.mio_tp_dbg.regValue          = READ_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_DBG_reg);
        tp_reg_data.mio_tp_dft_t2p.regValue      = READ_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_DFT_T2P_reg);
        tp_reg_data.mio_tp_dft_p2t.regValue      = READ_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_DFT_P2T_reg);
        tp_reg_data.mio_tp_dmya.regValue         = READ_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_DMYA_reg);
        tp_reg_data.mio_tp_dmyb.regValue         = READ_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_DMYB_reg);
        }
#endif
        return TPK_SUCCESS;
}

int restore_tp_reigster_value_ex(void)
{
        REGADDR tp_tf_cntl_addr;
        REGADDR tp_tf_frmcfg_addr;
        REGADDR tp_tf_cnt_addr;
        REGADDR tp_tf_drp_cnt_addr;
        REGADDR tp_tf_err_cnt_addr;
        REGADDR tp_tf_int_addr;
        REGADDR tp_tf_int_en_addr;

        REGADDR tp_pcr_ctl_addr;

        REGADDR tp_tp_des_cntl_addr;

        REGADDR tp_m2m_ring_limit_addr;
        REGADDR tp_m2m_ring_base_addr;
        REGADDR tp_m2m_ring_rp_addr;
        REGADDR tp_m2m_ring_wp_addr;
        REGADDR tp_m2m_ring_ctrl_addr;
#ifdef TP_SUPPORT_CI
        REGADDR tp_p_m2m_ring_limit_addr;
        REGADDR tp_p_m2m_ring_base_addr;
        REGADDR tp_p_m2m_ring_rp_addr;
        REGADDR tp_p_m2m_ring_wp_addr;
        REGADDR tp_p_m2m_ring_ctrl_addr;

        REGADDR tp_p_tf_cntl_addr;
        REGADDR tp_p_tf_frmcfg_addr;
        REGADDR tp_p_tf_int_addr;
        REGADDR tp_p_tf_int_en_addr;

        REGADDR tp_p_tf_cnt_addr;
        REGADDR tp_p_tf_drp_cnt_addr;
#endif

        tp_tp_pid_ctrl_RBUS        tp_pid_ctrl_reg;
        tp_tp_sec_ctrl_RBUS        tp_sec_ctrl_reg;
//        tp_tp_key_ctrl_RBUS        tp_key_ctrl_reg;
        tp_tp_ring_ctrl_RBUS       tp_ring_ctrl_reg;
#ifdef TP_SUPPORT_CI
        tp_tp_p_ring_ctrl_RBUS     tp_p_ring_ctrl_reg;
#endif
        tp_tp_tf_out_frmcfg_RBUS   tp_tf_out_frmcfg_reg;

        TPK_TP_ENGINE_T tp_id;
        UINT8 pid_idx;
        UINT8 sec_idx;
//        UINT8 key_idx;
        UINT8 buf_idx;

        WRITE_REG32(TP_TP_OUT_SHARE_reg, tp_reg_data.tpout_pinshare_buf_th);

        /* TPO CLK */
        WRITE_REG32(TP_TP_OUT_CLOCK_reg, READ_REG32(TP_TP_OUT_CLOCK_reg) |
                    (tp_reg_data.tpout_clk_reg & (TP_TP_OUT_CLOCK_tpo_clk_pre_mask | TP_TP_OUT_CLOCK_tpo_clk_post_mask)));

        /* enable tpout clock */
        CRT_CLK_OnOff(TPOUT, CLK_ON, NULL);

        /* mio TP */
#ifdef CONFIG_RTK_TP_WITH_MIO
        if(get_mio_enable_status()){
        WRITE_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_CTRL_reg, tp_reg_data.mio_tp_ctrl.regValue);
        WRITE_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_T2P_CFG_reg, tp_reg_data.mio_tp_t2p_cfg.regValue);
        WRITE_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_T2P_CTRL_reg, tp_reg_data.mio_tp_t2p_ctrl.regValue);
        WRITE_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_P2T_CFG_reg, tp_reg_data.mio_tp_p2t_cfg.regValue);
        WRITE_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_P2T_CTRL_reg, tp_reg_data.mio_tp_p2t_ctrl.regValue);
        WRITE_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_DBG_CTRL_reg, tp_reg_data.mio_tp_dbg_ctrl.regValue);
        WRITE_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_DBG_reg, tp_reg_data.mio_tp_dbg.regValue);
        WRITE_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_DFT_T2P_reg, tp_reg_data.mio_tp_dft_t2p.regValue);
        WRITE_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_DFT_P2T_reg, tp_reg_data.mio_tp_dft_p2t.regValue);
        WRITE_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_DMYA_reg, tp_reg_data.mio_tp_dmya.regValue);
        WRITE_REG32(ISO_SPI_SYNCHRONIZER_SD_TP_DMYB_reg, tp_reg_data.mio_tp_dmyb.regValue);
        }
#endif
        /* ATSC 30 */
        WRITE_REG32(TP_TP_ATSC_D_LIMIT_reg, tp_reg_data.tp_atsc_d_limit.regValue);
        WRITE_REG32(TP_TP_ATSC_D_BASE_reg, tp_reg_data.tp_atsc_d_base.regValue);
        WRITE_REG32(TP_TP_ATSC_D_RP_reg, tp_reg_data.tp_atsc_d_rp.regValue);
        WRITE_REG32(TP_TP_ATSC_D_WP_reg, tp_reg_data.tp_atsc_d_wp.regValue);
        WRITE_REG32(TP_TP_ATSC_I_LIMIT_reg, tp_reg_data.tp_atsc_i_limit.regValue);
        WRITE_REG32(TP_TP_ATSC_I_BASE_reg, tp_reg_data.tp_atsc_i_base.regValue);
        WRITE_REG32(TP_TP_ATSC_I_RP_reg, tp_reg_data.tp_atsc_i_rp.regValue);
        WRITE_REG32(TP_TP_ATSC_I_WP_reg, tp_reg_data.tp_atsc_i_wp.regValue);

        WRITE_REG32(TP_TP_ATSC_CONFIG1_reg, tp_reg_data.tp_atsc_config1.regValue);
        WRITE_REG32(TP_TP_ATSC_CONFIG2_reg, tp_reg_data.tp_atsc_config2.regValue);
        WRITE_REG32(TP_TP_ATSC_CONFIG3_reg, tp_reg_data.tp_atsc_config3.regValue);
        WRITE_REG32(TP_TP_ATSC_PLP_FILTER1_reg, tp_reg_data.tp_atsc_plp_filter1.regValue);
        WRITE_REG32(TP_TP_ATSC_PLP_FILTER2_reg, tp_reg_data.tp_atsc_plp_filter2.regValue);

        WRITE_REG32(TP_TP_ATSC_SRC_CNTL_reg, tp_reg_data.tp_atsc_src_cntl.regValue);

        /* tp ddr ring buffer */
        WRITE_REG32(TP_TP_THRESHOLD_reg, tp_reg_data.tp_threshold_reg.regValue);
        WRITE_REG32(TP_TP_FULLNESS_reg, tp_reg_data.tp_fullness_reg.regValue);

        for (buf_idx = 0; buf_idx < MAX_TP_BUFFER_COUNT; buf_idx++) {
                WRITE_REG32(TP_TP_RING_LIMIT_reg, tp_reg_data.tp_ring_limit_reg[buf_idx].regValue);
                WRITE_REG32(TP_TP_RING_BASE_reg, tp_reg_data.tp_ring_base_reg[buf_idx].regValue);
                WRITE_REG32(TP_TP_RING_RP_reg, tp_reg_data.tp_ring_rp_reg[buf_idx].regValue);
                WRITE_REG32(TP_TP_RING_WP_reg, tp_reg_data.tp_ring_wp_reg[buf_idx].regValue);

                tp_ring_ctrl_reg.regValue = 0;
                tp_ring_ctrl_reg.r_w      = 1;
                tp_ring_ctrl_reg.idx      = buf_idx;
                WRITE_REG32(TP_TP_RING_CTRL_reg, tp_ring_ctrl_reg.regValue);
        }

        tp_reg_data.tp_ring_avail_int_reg.write_data     = 1;
        tp_reg_data.tp_ring_avail_int2_reg.write_data    = 1;
        tp_reg_data.tp_ring_full_int_reg.write_data      = 1;
        tp_reg_data.tp_ring_full_int2_reg.write_data     = 1;
        tp_reg_data.tp_ring_avail_int_en_reg.write_data  = 1;
        tp_reg_data.tp_ring_avail_int_en2_reg.write_data = 1;
        tp_reg_data.tp_ring_full_int_en_reg.write_data   = 1;
        tp_reg_data.tp_ring_full_int_en2_reg.write_data  = 1;
        WRITE_REG32(TP_TP_RING_AVAIL_INT_0_reg, tp_reg_data.tp_ring_avail_int_reg.regValue);
        WRITE_REG32(TP_TP_RING_AVAIL_INT_1_reg, tp_reg_data.tp_ring_avail_int2_reg.regValue);
        WRITE_REG32(TP_TP_RING_FULL_INT_0_reg, tp_reg_data.tp_ring_full_int_reg.regValue);
        WRITE_REG32(TP_TP_RING_FULL_INT_1_reg, tp_reg_data.tp_ring_full_int2_reg.regValue);
        WRITE_REG32(TP_TP_RING_AVAIL_INT_EN_0_reg, tp_reg_data.tp_ring_avail_int_en_reg.regValue);
        WRITE_REG32(TP_TP_RING_AVAIL_INT_EN_1_reg, tp_reg_data.tp_ring_avail_int_en2_reg.regValue);
        WRITE_REG32(TP_TP_RING_FULL_INT_EN_0_reg, tp_reg_data.tp_ring_full_int_en_reg.regValue);
        WRITE_REG32(TP_TP_RING_FULL_INT_EN_1_reg, tp_reg_data.tp_ring_full_int_en2_reg.regValue);

#ifdef TP_SUPPORT_CI
        /* tp_p ddr ring buffer */
        WRITE_REG32(TP_TP_P_THRESHOLD_reg, tp_reg_data.tp_p_threshold_reg.regValue);
        WRITE_REG32(TP_TP_P_FULLNESS_reg, tp_reg_data.tp_p_fullness_reg.regValue);
        for (buf_idx = 0; buf_idx < MAX_TPP_BUFFER_COUNT; buf_idx++) {
                WRITE_REG32(TP_TP_P_RING_LIMIT_reg, tp_reg_data.tp_p_ring_limit_reg[buf_idx].regValue);
                WRITE_REG32(TP_TP_P_RING_BASE_reg, tp_reg_data.tp_p_ring_base_reg[buf_idx].regValue);
                WRITE_REG32(TP_TP_P_RING_RP_reg, tp_reg_data.tp_p_ring_rp_reg[buf_idx].regValue);
                WRITE_REG32(TP_TP_P_RING_WP_reg, tp_reg_data.tp_p_ring_wp_reg[buf_idx].regValue);

                tp_p_ring_ctrl_reg.regValue = 0;
                tp_p_ring_ctrl_reg.r_w      = 1;
                tp_p_ring_ctrl_reg.idx      = buf_idx;
                WRITE_REG32(TP_TP_P_RING_CTRL_reg, tp_p_ring_ctrl_reg.regValue);
        }

        tp_reg_data.tp_p_ring_avail_int_en_reg.write_data = 1;
        tp_reg_data.tp_p_ring_full_int_en_reg.write_data  = 1;
        tp_reg_data.tp_p_ring_avail_int_en_reg.write_data = 1;
        tp_reg_data.tp_p_ring_full_int_en_reg.write_data  = 1;
        WRITE_REG32(TP_TP_P_RING_AVAIL_INT_reg, tp_reg_data.tp_p_ring_avail_int_reg.regValue);
        WRITE_REG32(TP_TP_P_RING_FULL_INT_reg, tp_reg_data.tp_p_ring_full_int_reg.regValue);
        WRITE_REG32(TP_TP_P_RING_AVAIL_INT_EN_reg, tp_reg_data.tp_p_ring_avail_int_en_reg.regValue);
        WRITE_REG32(TP_TP_P_RING_FULL_INT_EN_reg, tp_reg_data.tp_p_ring_full_int_en_reg.regValue);
#endif

        /* pid filter */
        WRITE_REG32(TP_TP_PID_PART_reg, tp_reg_data.tp_pid_part_reg.regValue);
        for (pid_idx = 0; pid_idx < MAX_PID_FILTER_COUNT; pid_idx++) {
                WRITE_REG32(TP_TP_PID_DATA_reg, tp_reg_data.tp_pid_data_reg[pid_idx].regValue);
                WRITE_REG32(TP_TP_PID_DATA2_reg, tp_reg_data.tp_pid_data2_reg[pid_idx].regValue);
                WRITE_REG32(TP_TP_PID_DATA3_reg, tp_reg_data.tp_pid_data3_reg[pid_idx].regValue);

                tp_pid_ctrl_reg.regValue = 0;
                tp_pid_ctrl_reg.r_w      = 1;
                tp_pid_ctrl_reg.idx      = pid_idx;
                WRITE_REG32(TP_TP_PID_CTRL_reg, tp_pid_ctrl_reg.regValue);
        }
        /* section filter */
        for (sec_idx = 0; sec_idx < MAX_SEC_COUNT; sec_idx++) {
                WRITE_REG32(TP_TP_SEC_DATA0_reg, tp_reg_data.tp_sec_data0_reg[sec_idx].regValue);
                WRITE_REG32(TP_TP_SEC_DATA1_reg, tp_reg_data.tp_sec_data1_reg[sec_idx].regValue);
                WRITE_REG32(TP_TP_SEC_DATA2_reg, tp_reg_data.tp_sec_data2_reg[sec_idx].regValue);
                WRITE_REG32(TP_TP_SEC_DATA3_reg, tp_reg_data.tp_sec_data3_reg[sec_idx].regValue);
                WRITE_REG32(TP_TP_SEC_DATA4_reg, tp_reg_data.tp_sec_data4_reg[sec_idx].regValue);
                WRITE_REG32(TP_TP_SEC_DATA5_reg, tp_reg_data.tp_sec_data5_reg[sec_idx].regValue);
                WRITE_REG32(TP_TP_SEC_DATA6_reg, tp_reg_data.tp_sec_data6_reg[sec_idx].regValue);
                WRITE_REG32(TP_TP_SEC_DATA7_reg, tp_reg_data.tp_sec_data7_reg[sec_idx].regValue);
                WRITE_REG32(TP_TP_SEC_DATA8_reg, tp_reg_data.tp_sec_data8_reg[sec_idx].regValue);
                WRITE_REG32(TP_TP_SEC_DATA9_reg, tp_reg_data.tp_sec_data9_reg[sec_idx].regValue);

                tp_sec_ctrl_reg.regValue = 0;
                tp_sec_ctrl_reg.r_w      = 1;
                tp_sec_ctrl_reg.idx      = sec_idx;
                WRITE_REG32(TP_TP_SEC_CTRL_reg, tp_sec_ctrl_reg.regValue);
        }

#if 0
        /* des & tdes */
        for (key_idx = 0; key_idx < MAX_KEY_COUNT; key_idx++) {
                WRITE_REG32(TP_TP_KEY_INFO_0_reg, tp_reg_data.tp_key_info_reg[key_idx].regValue);
                WRITE_REG32(TP_TP_KEY_INFO_1_reg, tp_reg_data.tp_key_info2_reg[key_idx].regValue);

                tp_key_ctrl_reg.regValue = 0;
                tp_key_ctrl_reg.r_w      = 1;
                tp_key_ctrl_reg.idx      = key_idx;
                WRITE_REG32(TP_TP_KEY_CTRL_reg, tp_key_ctrl_reg.regValue);
        }
#endif
        /* crc */
        WRITE_REG32(TP_TP_CRC_INIT_reg, tp_reg_data.tp_crc_init_reg.regValue);
#ifdef TP_SUPPORT_CI
        /* sync replace */
        WRITE_REG32(TP_TP_SYNC_RPLACE_reg, tp_reg_data.tp_sync_rplace_reg.regValue);

        /* tp_p sync replace */
        WRITE_REG32(TP_TP_P_SYNC_RPLACE_reg, tp_reg_data.tp_p_sync_rplace_reg.regValue);
#endif
        /* tp dmy_a & dmy_b */
        WRITE_REG32(TP_TP_DMY_A_reg, tp_reg_data.tp_dmy_a_reg.regValue);
        WRITE_REG32(TP_TP_DMY_B_reg, tp_reg_data.tp_dmy_b_reg.regValue);

        /* frame rate control */
        WRITE_REG32(TP_PREFIX_FR_CLK_RATE_reg, tp_reg_data.prefix_fr_clk_rate_reg.regValue);
        WRITE_REG32(TP_PREFIX_CTRL_reg, tp_reg_data.prefix_ctrl_reg.regValue);
        WRITE_REG32(TP_PREFIX_DATA1_reg, tp_reg_data.prefix_data1_reg.regValue);
        WRITE_REG32(TP_PREFIX_DATA2_reg, tp_reg_data.prefix_data2_reg.regValue);
        WRITE_REG32(TP_FR_CTRL_reg, tp_reg_data.fr_ctrl_reg.regValue);

        /* start code search */
        WRITE_REG32(TP_START_CODE_TP0_reg, tp_reg_data.start_code_tp0_reg.regValue);
        WRITE_REG32(TP_START_CODE_TP1_reg, tp_reg_data.start_code_tp1_reg.regValue);
        WRITE_REG32(TP_START_CODE_CTRL_TP0_reg, tp_reg_data.start_code_ctrl_tp0_reg.regValue);
        WRITE_REG32(TP_START_CODE_CTRL_TP1_reg, tp_reg_data.start_code_ctrl_tp1_reg.regValue);

        /* tpout */
        tp_tf_out_frmcfg_reg.regValue  = 0;
        tp_tf_out_frmcfg_reg.in_tp_rst = 1;
        WRITE_REG32(TP_TP_TF_OUT_FRMCFG_reg, tp_tf_out_frmcfg_reg.regValue);
        udelay(100);
        tp_tf_out_frmcfg_reg.in_tp_rst = 0;
        WRITE_REG32(TP_TP_TF_OUT_FRMCFG_reg, tp_tf_out_frmcfg_reg.regValue);
        udelay(100);

        WRITE_REG32(TP_TP_TF_OUT_FRMCFG_reg, tp_reg_data.tp_tf_out_frmcfg_reg.regValue);
#ifdef TP_SUPPORT_CI
        WRITE_REG32(TP_TP_OUT_CTRL_reg, tp_reg_data.tp_out_ctrl_reg.regValue);
        WRITE_REG32(TP_TP_OUT_INT_reg, tp_reg_data.tp_out_int_reg.regValue);
#endif
        /* tp input control */
        WRITE_REG32(TP_TP_IN_POL_reg, tp_reg_data.tp_in_pol_reg.regValue);

        /* Others */
        WRITE_REG32(TP_TP0_ERR_CNT_EN_reg, tp_reg_data.tp0_err_cnt_en.regValue);
        WRITE_REG32(TP_TP1_ERR_CNT_EN_reg, tp_reg_data.tp1_err_cnt_en.regValue);

        for (tp_id = 0; tp_id < MAX_TP_COUNT; tp_id++) {
                switch (tp_id) {
                        case TP_TP0:
                                /* tp */
                                tp_tf_cntl_addr          = TP_TP_TF0_CNTL_reg;
                                tp_tf_frmcfg_addr        = TP_TP_TF0_FRMCFG_reg;
                                tp_tf_cnt_addr           = TP_TP_TF0_CNT_reg;
                                tp_tf_drp_cnt_addr       = TP_TP_TF0_CNT_reg;
                                tp_tf_err_cnt_addr       = TP_TP_TF0_CNT_reg;
                                tp_tf_int_addr           = TP_TP_TF0_INT_reg;
                                tp_tf_int_en_addr        = TP_TP_TF0_INT_EN_reg;
                                /* pcr */
                                tp_pcr_ctl_addr          = TP_TP0_PCR_CTL_reg;
                                /* descrambler */
                                tp_tp_des_cntl_addr      = TP_TP_TP0_DES_CNTL_reg;
                                /* tp mtp */
                                tp_m2m_ring_limit_addr   = TP_TP0_M2M_RING_LIMIT_reg;
                                tp_m2m_ring_base_addr    = TP_TP0_M2M_RING_BASE_reg;
                                tp_m2m_ring_rp_addr      = TP_TP0_M2M_RING_RP_reg;
                                tp_m2m_ring_wp_addr      = TP_TP0_M2M_RING_WP_reg;
                                tp_m2m_ring_ctrl_addr    = TP_TP0_M2M_RING_CTRL_reg;
#ifdef TP_SUPPORT_CI
                                /* tpp mtp */
                                tp_p_m2m_ring_limit_addr = TP_TP0_P_M2M_RING_LIMIT_reg;
                                tp_p_m2m_ring_base_addr  = TP_TP0_P_M2M_RING_BASE_reg;
                                tp_p_m2m_ring_rp_addr    = TP_TP0_P_M2M_RING_RP_reg;
                                tp_p_m2m_ring_wp_addr    = TP_TP0_P_M2M_RING_WP_reg;
                                tp_p_m2m_ring_ctrl_addr  = TP_TP0_P_M2M_RING_CTRL_reg;
                                /* tpp */
                                tp_p_tf_cntl_addr        = TP_TP_TF0_P_CNTL_reg;
                                tp_p_tf_frmcfg_addr      = TP_TP_TF0_P_FRMCFG_reg;
                                tp_p_tf_int_addr         = TP_TP_TF0_P_INT_reg;
                                tp_p_tf_int_en_addr      = TP_TP_TF0_P_INT_EN_reg;
                                tp_p_tf_cnt_addr         = TP_TP_TF0_P_CNT_reg;
                                tp_p_tf_drp_cnt_addr     = TP_TP_TF0_P_DRP_CNT_reg;
#endif

                                break;

                        case TP_TP1:
                                /* tp */
                                tp_tf_cntl_addr          = TP_TP_TF1_CNTL_reg;
                                tp_tf_frmcfg_addr        = TP_TP_TF1_FRMCFG_reg;
                                tp_tf_cnt_addr           = TP_TP_TF1_CNT_reg;
                                tp_tf_drp_cnt_addr       = TP_TP_TF1_CNT_reg;
                                tp_tf_err_cnt_addr       = TP_TP_TF1_CNT_reg;
                                tp_tf_int_addr           = TP_TP_TF1_INT_reg;
                                tp_tf_int_en_addr        = TP_TP_TF1_INT_EN_reg;
                                /* pcr */
                                tp_pcr_ctl_addr          = TP_TP1_PCR_CTL_reg;
                                /* descrambler */
                                tp_tp_des_cntl_addr      = TP_TP_TP1_DES_CNTL_reg;
                                /* tp mtp */
                                tp_m2m_ring_limit_addr   = TP_TP1_M2M_RING_LIMIT_reg;
                                tp_m2m_ring_base_addr    = TP_TP1_M2M_RING_BASE_reg;
                                tp_m2m_ring_rp_addr      = TP_TP1_M2M_RING_RP_reg;
                                tp_m2m_ring_wp_addr      = TP_TP1_M2M_RING_WP_reg;
                                tp_m2m_ring_ctrl_addr    = TP_TP1_M2M_RING_CTRL_reg;
#ifdef TP_SUPPORT_CI
                                /* tpp mtp */
                                tp_p_m2m_ring_limit_addr = TP_TP1_P_M2M_RING_LIMIT_reg;
                                tp_p_m2m_ring_base_addr  = TP_TP1_P_M2M_RING_BASE_reg;
                                tp_p_m2m_ring_rp_addr    = TP_TP1_P_M2M_RING_RP_reg;
                                tp_p_m2m_ring_wp_addr    = TP_TP1_P_M2M_RING_WP_reg;
                                tp_p_m2m_ring_ctrl_addr  = TP_TP1_P_M2M_RING_CTRL_reg;
                                /* tpp */
                                tp_p_tf_cntl_addr        = TP_TP_TF1_P_CNTL_reg;
                                tp_p_tf_frmcfg_addr      = TP_TP_TF1_P_FRMCFG_reg;
                                tp_p_tf_int_addr         = TP_TP_TF1_P_INT_reg;
                                tp_p_tf_int_en_addr      = TP_TP_TF1_P_INT_EN_reg;
                                tp_p_tf_cnt_addr         = TP_TP_TF1_P_CNT_reg;
                                tp_p_tf_drp_cnt_addr     = TP_TP_TF1_P_DRP_CNT_reg;
#endif
                                break;

                        case TP_TP2:
                                /* tp */
                                tp_tf_cntl_addr          = TP_TP_TF2_CNTL_reg;
                                tp_tf_frmcfg_addr        = TP_TP_TF2_FRMCFG_reg;
                                tp_tf_cnt_addr           = TP_TP_TF2_CNT_reg;
                                tp_tf_drp_cnt_addr       = TP_TP_TF2_CNT_reg;
                                tp_tf_err_cnt_addr       = TP_TP_TF2_CNT_reg;
                                tp_tf_int_addr           = TP_TP_TF2_INT_reg;
                                tp_tf_int_en_addr        = TP_TP_TF2_INT_EN_reg;
                                /* pcr */
                                tp_pcr_ctl_addr          = TP_TP2_PCR_CTL_reg;
                                /* descrambler */
                                tp_tp_des_cntl_addr      = TP_TP_TP2_DES_CNTL_reg;
                                /* tp mtp */
                                tp_m2m_ring_limit_addr   = TP_TP2_M2M_RING_LIMIT_reg;
                                tp_m2m_ring_base_addr    = TP_TP2_M2M_RING_BASE_reg;
                                tp_m2m_ring_rp_addr      = TP_TP2_M2M_RING_RP_reg;
                                tp_m2m_ring_wp_addr      = TP_TP2_M2M_RING_WP_reg;
                                tp_m2m_ring_ctrl_addr    = TP_TP2_M2M_RING_CTRL_reg;
#ifdef TP_SUPPORT_CI
                                /* tpp mtp */
                                tp_p_m2m_ring_limit_addr = TP_TP2_P_M2M_RING_LIMIT_reg;
                                tp_p_m2m_ring_base_addr  = TP_TP2_P_M2M_RING_BASE_reg;
                                tp_p_m2m_ring_rp_addr    = TP_TP2_P_M2M_RING_RP_reg;
                                tp_p_m2m_ring_wp_addr    = TP_TP2_P_M2M_RING_WP_reg;
                                tp_p_m2m_ring_ctrl_addr  = TP_TP2_P_M2M_RING_CTRL_reg;
                                /* tpp */
                                tp_p_tf_cntl_addr        = TP_TP_TF2_P_CNTL_reg;
                                tp_p_tf_frmcfg_addr      = TP_TP_TF2_P_FRMCFG_reg;
                                tp_p_tf_int_addr         = TP_TP_TF2_P_INT_reg;
                                tp_p_tf_int_en_addr      = TP_TP_TF2_P_INT_EN_reg;
                                tp_p_tf_cnt_addr         = TP_TP_TF2_P_CNT_reg;
                                tp_p_tf_drp_cnt_addr     = TP_TP_TF2_P_DRP_CNT_reg;
#endif
                                break;
#ifdef TP_SUPPORT_TP3
                        case TP_TP3: {
                                /* tp */
                                tp_tf_cntl_addr          = TP_TP_TF3_CNTL_reg;
                                tp_tf_frmcfg_addr        = TP_TP_TF3_FRMCFG_reg;
                                tp_tf_cnt_addr           = TP_TP_TF3_CNT_reg;
                                tp_tf_drp_cnt_addr       = TP_TP_TF3_CNT_reg;
                                tp_tf_err_cnt_addr       = TP_TP_TF3_CNT_reg;
                                tp_tf_int_addr           = TP_TP_TF3_INT_reg;
                                tp_tf_int_en_addr        = TP_TP_TF3_INT_EN_reg;
                                /* pcr */
                                tp_pcr_ctl_addr          = TP_TP3_PCR_CTL_reg;
                                /* TP3 do not support descrambler */
                                /* tp mtp */
                                tp_m2m_ring_limit_addr   = TP_TP3_M2M_RING_LIMIT_reg;
                                tp_m2m_ring_base_addr    = TP_TP3_M2M_RING_BASE_reg;
                                tp_m2m_ring_rp_addr      = TP_TP3_M2M_RING_RP_reg;
                                tp_m2m_ring_wp_addr      = TP_TP3_M2M_RING_WP_reg;
                                tp_m2m_ring_ctrl_addr    = TP_TP3_M2M_RING_CTRL_reg;
                                /* there is no tpp3 mtp */
                                /* three is no tpp */
                                break;
                        }
#endif
                        default:
                                break;
                }

                /* tp mtp */
                tp_reg_data.tp_m2m_ring_ctrl_reg[tp_id].write_data       = 1;
                WRITE_REG32(tp_m2m_ring_limit_addr, tp_reg_data.tp_m2m_ring_limit_reg[tp_id].regValue);
                WRITE_REG32(tp_m2m_ring_base_addr, tp_reg_data.tp_m2m_ring_base_reg[tp_id].regValue);
                WRITE_REG32(tp_m2m_ring_rp_addr, tp_reg_data.tp_m2m_ring_rp_reg[tp_id].regValue);
                WRITE_REG32(tp_m2m_ring_wp_addr, tp_reg_data.tp_m2m_ring_wp_reg[tp_id].regValue);
                WRITE_REG32(tp_m2m_ring_ctrl_addr, tp_reg_data.tp_m2m_ring_ctrl_reg[tp_id].regValue);

                /* pcr */
                WRITE_REG32(tp_pcr_ctl_addr, tp_reg_data.tp_pcr_ctl_reg[tp_id].regValue);

                /* tp */
                tp_reg_data.tp_tf_cntl_reg[tp_id].write_data   = 1;
                tp_reg_data.tp_tf_int_reg[tp_id].write_data    = 1;
                tp_reg_data.tp_tf_int_en_reg[tp_id].write_data = 1;
                WRITE_REG32(tp_tf_cntl_addr, tp_reg_data.tp_tf_cntl_reg[tp_id].regValue);
                WRITE_REG32(tp_tf_frmcfg_addr, tp_reg_data.tp_tf_frmcfg_reg[tp_id].regValue);
                WRITE_REG32(tp_tf_cnt_addr, _ZERO);
                WRITE_REG32(tp_tf_drp_cnt_addr, _ZERO);
                WRITE_REG32(tp_tf_err_cnt_addr, _ZERO);
                WRITE_REG32(tp_tf_int_addr, tp_reg_data.tp_tf_int_reg[tp_id].regValue);
                WRITE_REG32(tp_tf_int_en_addr, tp_reg_data.tp_tf_int_en_reg[tp_id].regValue);

                if ( tp_id != TP_TP3 ) {
                        /* des & tdes */
                        WRITE_REG32(tp_tp_des_cntl_addr, tp_reg_data.tp_tp_des_cntl_reg[tp_id].regValue);
#ifdef TP_SUPPORT_CI
                        /* tpp mtp */
                        tp_reg_data.tp_p_m2m_ring_ctrl_reg[tp_id].write_data = 1;
                        WRITE_REG32(tp_p_m2m_ring_limit_addr, tp_reg_data.tp_p_m2m_ring_limit_reg[tp_id].regValue);
                        WRITE_REG32(tp_p_m2m_ring_base_addr, tp_reg_data.tp_p_m2m_ring_base_reg[tp_id].regValue);
                        WRITE_REG32(tp_p_m2m_ring_rp_addr, tp_reg_data.tp_p_m2m_ring_rp_reg[tp_id].regValue);
                        WRITE_REG32(tp_p_m2m_ring_wp_addr, tp_reg_data.tp_p_m2m_ring_wp_reg[tp_id].regValue);
                        WRITE_REG32(tp_p_m2m_ring_ctrl_addr, tp_reg_data.tp_p_m2m_ring_ctrl_reg[tp_id].regValue);
                        /* tp_p */
                        tp_reg_data.tp_tf_p_cntl_reg[tp_id].write_data   = 1;
                        tp_reg_data.tp_tf_p_int_reg[tp_id].write_data    = 1;
                        tp_reg_data.tp_tf_p_int_en_reg[tp_id].write_data = 1;
                        WRITE_REG32(tp_p_tf_cntl_addr, tp_reg_data.tp_tf_p_cntl_reg[tp_id].regValue);
                        WRITE_REG32(tp_p_tf_frmcfg_addr, tp_reg_data.tp_tf_p_frmcfg_reg[tp_id].regValue);
                        WRITE_REG32(tp_p_tf_int_addr, tp_reg_data.tp_tf_p_int_reg[tp_id].regValue);
                        WRITE_REG32(tp_p_tf_int_en_addr, tp_reg_data.tp_tf_p_int_en_reg[tp_id].regValue);

                        WRITE_REG32(tp_p_tf_cnt_addr, _ZERO);
                        WRITE_REG32(tp_p_tf_drp_cnt_addr, _ZERO);
#endif
                }

        }
#ifdef TP_TP_GCK
        WRITE_REG32(TP_TP_GCK_reg, 0x00000001);
#endif
        return TPK_SUCCESS;
}

static void tp_shutdown(struct platform_device *pdev)
{
        TP_DBG_SIMPLE( "do tp_shutdown\n");
}

static int tp_suspend(struct device *dev)
{
        INT32 ret = 0;

        TP_DBG_SIMPLE( "do tp_suspend\n");

        ret = save_tp_reigster_value_ex();
        if ( ret == TPK_SUCCESS ) {
                /* disable tp clock */
                CRT_CLK_OnOff(TP, CLK_OFF, NULL);
#ifdef CONFIG_RTK_TP_WITH_MIO
        if(get_mio_enable_status())
        {
            CRT_CLK_OnOff(TP_MIO, CLK_OFF, NULL);
        }

#endif
        }

        TP_DBG_SIMPLE( "do tp_suspend finish\n");
        return 0;
}

static int tp_resume(struct device *dev)
{
        TP_DBG_SIMPLE( "do tp_resume\n");

        /* enable tp clock and restore tp register value */
        if ( check_tp_clk_status() != TP_CLK_ON ) {
                CRT_CLK_OnOff(TP, CLK_ON, NULL);
#ifdef CONFIG_RTK_TP_WITH_MIO
                if(get_mio_enable_status())
                {
                    CRT_CLK_OnOff(TP_MIO, CLK_ON, NULL);
                }
#endif
        }
        restore_tp_reigster_value_ex();

#ifdef CONFIG_RTK_TP_WITH_MIO
        TP_MIO_init();//to avoid tpo clk always keep high problem
#endif

#ifdef SUPPORT_TP_CP_SAVING_MODE        
        mcp_enable_cp_power_saving(false);
#endif

        TP_DBG_SIMPLE( "do tp_resume finish\n");
        return 0;
}

#ifdef CONFIG_HIBERNATION
static int tp_poweroff(struct device *dev)
{
        /* TO BE IMPLEMENT */
        TP_DBG_SIMPLE( "do tp_poweroff\n");
        /* do nothing */
        TP_DBG_SIMPLE( "do tp_poweroff finish\n");
        return 0;
}

static int tp_restore(struct device *dev)
{
        TP_DBG_SIMPLE( "do tp_restore\n");

        /* enable tp clock and restore tp register value */
        if ( check_tp_clk_status() != TP_CLK_ON ) {
                CRT_CLK_OnOff(TP, CLK_ON, NULL);
#ifdef CONFIG_RTK_TP_WITH_MIO

                if(get_mio_enable_status())
                {
                    CRT_CLK_OnOff(TP_MIO, CLK_ON, NULL);
                }
#endif
        }
        restore_tp_reigster_value_ex();

#ifdef CONFIG_RTK_TP_WITH_MIO
        TP_MIO_init();//to avoid tpo clk always keep high problem
#endif

#ifdef SUPPORT_TP_CP_SAVING_MODE        
        mcp_enable_cp_power_saving(false);
#endif

        TP_DBG_SIMPLE( "do tp_restore finish\n");
        return 0;
}

static int tp_freeze(struct device *dev)
{
        INT32 ret;
        ret = 0;

        TP_DBG_SIMPLE( "do tp_freeze \n");

        RHAL_TPOUT_Enable(0, TPOUT_STREAM_STOP);        
        TP_DBG_SIMPLE( "tp_freeze ,disable TPO\n");
#ifdef CONFIG_RTK_TP_WITH_MIO        
        rtd_outl(ISO_SPI_SYNCHRONIZER_TP_DFT_T2P_reg, 0x0000FFFF);
#endif

        ret = save_tp_reigster_value_ex();
        if ( ret == TPK_SUCCESS ) {
                /* disable tp clock */
                CRT_CLK_OnOff(TP, CLK_OFF, NULL);
#ifdef CONFIG_RTK_TP_WITH_MIO
                if(get_mio_enable_status())
                {
                    CRT_CLK_OnOff(TP_MIO, CLK_OFF, NULL);
                }
#endif
                TP_DBG_SIMPLE( "disable tp clock finish\n");
        }

        TP_DBG_SIMPLE( "do tp_freeze finish \n");
        return 0;
}
static int tp_thaw(struct device *dev)
{
        TP_DBG_SIMPLE( "do tp_thaw\n");

        /* enable tp clock and restore tp register value */
        if ( check_tp_clk_status() != TP_CLK_ON ) {
                CRT_CLK_OnOff(TP, CLK_ON, NULL);
#ifdef CONFIG_RTK_TP_WITH_MIO
                if(get_mio_enable_status())
                {
                    CRT_CLK_OnOff(TP_MIO, CLK_ON, NULL);
                }
#endif
        }
        restore_tp_reigster_value_ex();

        TP_DBG_SIMPLE( "do tp_thaw finish\n");
        return 0;
}
#endif

static int tp_probe(struct platform_device *pdev)
{
        int irq = 0;

        irq = platform_get_irq(pdev, 0);
        if (irq < 0) {
                TP_ERROR("[tp_probe]: can't get irq. ret=%d\n", irq);
                /* ignore if fail */
        }
        pTp_drv->irq_number = irq;
        return 0;
}

static int tp_remove(struct platform_device *pdev)
{
        return 0;
}

static const struct of_device_id tp_of_match[] = {
        {
                .compatible = "realtek,tp",
        },
        {},
};

MODULE_DEVICE_TABLE(of, tp_of_match);

static const struct dev_pm_ops tp_pm_ops = {
        .suspend    = tp_suspend,
        .resume     = tp_resume,
#ifdef CONFIG_HIBERNATION
        .freeze     = tp_freeze,
        .thaw       = tp_thaw,
        .poweroff   = tp_poweroff,
        .restore    = tp_restore,
#endif
};


static struct platform_driver tp_platform_driver = {
        .probe  = tp_probe,
        .remove = tp_remove,
        .shutdown = tp_shutdown,
        .driver = {
                .name         = (char *)TP_DRIVER_NAME,
                .bus          = &platform_bus_type,
#ifdef CONFIG_PM
                .pm           = &tp_pm_ops,
                .of_match_table = of_match_ptr(tp_of_match),
#endif
        },
};
#endif /* CONFIG_PM */

/***************************************************************************
         ------------------- module Init ----------------
****************************************************************************/
static struct miscdevice tp_miscdev = {
        MISC_DYNAMIC_MINOR,
        TP_DRIVER_NAME,
        &tp_fops
};

int __init tp_module_init(void)
{
        int result;
        TP_DBG( "Initial TP module\n");
        if (get_product_type() == PRODUCT_TYPE_DIAS) {
                TP_DBG( "TP not support on dias platform\n");
                CRT_CLK_OnOff(TP, CLK_OFF, NULL);
                result = 0;
                return result;
        }

        TP_INFO("enable TP clock\n");
        /* enable TP clock & reset */
        CRT_CLK_OnOff(TP, CLK_ON, NULL);

#ifdef CONFIG_RTK_TP_WITH_MIO
        if(get_mio_enable_status())
        {
            CRT_CLK_OnOff(TP_MIO, CLK_ON, NULL);
        }
#endif

#ifdef TP_SUPPORT_CI
       TP_INFO("enable TPO clock\n");
        /* output_clk_div_en :1800_0114[4] (TPO clock enable) */
       CRT_CLK_OnOff(TPOUT, CLK_ON, NULL);
#endif

#ifdef TP_VERIFY_ENABLE
        if (TPVerify_CreateSectionBuffer() < 0) {
                return -1;
        }
#endif
        create_tp_controller();

        if (pTp_drv == NULL) {
                return -1;
        }

        if (misc_register(&tp_miscdev)) {
                release_tp_controller();
                pTp_drv = NULL;
                return -1;
        }

        result = platform_driver_register(&tp_platform_driver);
        if (result) {
                TP_ERROR("%s: can not register platform driver, ret=%d\n", __func__, result);
                return -1;
        }

#ifdef TP_SUPPORT_CI
        /* enable tp input rate cnt detect funtion */
        tp_input_rate_cnt_detect_thread_start_stop(1);
#endif

#ifdef TP_VERIFY_ENABLE
        tp_allocate_section_info_buffer();
#endif
        TP_DBG( "Initial TP module successed\n");
        return 0;
}

static void __exit tp_module_exit(void)
{

        TP_DBG( "[TP] Release TP module\n");

        platform_driver_unregister(&tp_platform_driver);

        release_tp_controller();
        misc_deregister(&tp_miscdev);
#ifdef TP_VERIFY_ENABLE
        tp_release_section_info_buffer();
        TPVerify_ReleaseSectionBuffer();
#endif
#ifdef TP_SUPPORT_CI
        /* disable tp input rate cnt detect funtion */
        tp_input_rate_cnt_detect_thread_start_stop(0);
#endif

        TP_DBG( "[TP] Release TP module successed\n");
}

module_init(tp_module_init);
module_exit(tp_module_exit);
MODULE_IMPORT_NS(VFS_internal_I_am_really_a_filesystem_and_am_NOT_a_driver);
MODULE_AUTHOR( "Andrew Feng, Realtek Semiconductor");
MODULE_LICENSE( "GPL");
