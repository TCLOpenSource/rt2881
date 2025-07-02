#include <linux/kernel.h>
#include <linux/string.h>

#include <rtk_kdriver/i2c_h5_customized.h>
#include <tvscalercontrol/host/host_scaler_i2c_only.h>
#include <scaler/scalerCommon.h>
#include <rtd_log/rtd_module_log.h>

#if defined(CONFIG_H5CX_SUPPORT)

#define i2c_rlink_Host_vsc_Block_size 4
#define i2c_rlink_Cmd_size_less8k 2

static int scaler_i2c_calc_checksum(unsigned char *data, unsigned short len, unsigned char *checksum)
{
    unsigned int sum = 0;
    unsigned int i = 0;

    if ((data == NULL) || (len == 0) || (NULL == checksum))
    return -1;

    for (; i<len; i++)
    sum += data[i];

    *checksum = (unsigned char)((256-(sum%256)) %256);
    return 0;
}

char vbe_rlink_set_dlg_mode(unsigned int dlgMode)
{
    unsigned char i2c_cmd_data[i2c_rlink_Cmd_size_less8k + i2c_rlink_Host_vsc_Block_size + 1];
    unsigned char checksum;
    int ret_val = 0;

    memset(i2c_cmd_data, 0, sizeof(i2c_cmd_data));

    // cmd
    i2c_cmd_data[0] = I2C_ONLY_SCALER;
    i2c_cmd_data[1] = SCALER_RLK_I2C_ONLY_HOST_DLG_MODE;

    // data
    i2c_cmd_data[2] = (dlgMode & 0xFF000000) >> 24;
    i2c_cmd_data[3] = (dlgMode & 0x00FF0000) >> 16;
    i2c_cmd_data[4] = (dlgMode & 0x0000FF00) >> 8;
    i2c_cmd_data[5] = (dlgMode & 0x000000FF);

    // checksum
    scaler_i2c_calc_checksum(&i2c_cmd_data[i2c_rlink_Cmd_size_less8k], i2c_rlink_Host_vsc_Block_size, &checksum);
    i2c_cmd_data[i2c_rlink_Cmd_size_less8k + i2c_rlink_Host_vsc_Block_size] = checksum;

    ret_val = i2c_master_send_ex_flag(H5X_I2C_PORT, H5X_I2C_ADDR, i2c_cmd_data, (unsigned short)sizeof(i2c_cmd_data), H5X_I2C_SPEED_FLAG);
    if (ret_val < 0)
    {
        // TODO. error handle
        rtd_pr_vbe_emerg("%s send data failed, ret_val=%d, size=%d \n", __func__, ret_val, sizeof(i2c_cmd_data));
    }
    else
    {
        rtd_pr_vbe_emerg("%s, i2c_src[0:6]=%d,%d,%d,%d,%d,%d\n",
            __func__, i2c_cmd_data[0], i2c_cmd_data[1], i2c_cmd_data[2], i2c_cmd_data[3], i2c_cmd_data[4], i2c_cmd_data[5], 
            i2c_cmd_data[6]);
    }

    return ret_val;

}

char vbe_rlink_set_vrr_info(unsigned int vrr)
{
    unsigned char i2c_cmd_data[i2c_rlink_Cmd_size_less8k + i2c_rlink_Host_vsc_Block_size + 1];
    unsigned char checksum;
    int ret_val = 0;

    memset(i2c_cmd_data, 0, sizeof(i2c_cmd_data));

    // cmd
    i2c_cmd_data[0] = I2C_ONLY_SCALER;
    i2c_cmd_data[1] = SCALER_RLK_I2C_ONLY_HOST_VRR;

    // data
    i2c_cmd_data[2] = (vrr & 0xFF000000) >> 24;
    i2c_cmd_data[3] = (vrr & 0x00FF0000) >> 16;
    i2c_cmd_data[4] = (vrr & 0x0000FF00) >> 8;
    i2c_cmd_data[5] = (vrr & 0x000000FF);

    // checksum
    scaler_i2c_calc_checksum(&i2c_cmd_data[i2c_rlink_Cmd_size_less8k], i2c_rlink_Host_vsc_Block_size, &checksum);
    i2c_cmd_data[i2c_rlink_Cmd_size_less8k + i2c_rlink_Host_vsc_Block_size] = checksum;

    ret_val = i2c_master_send_ex_flag(H5X_I2C_PORT, H5X_I2C_ADDR, i2c_cmd_data, (unsigned short)sizeof(i2c_cmd_data), H5X_I2C_SPEED_FLAG);
    if (ret_val < 0)
    {
        // TODO. error handle
        rtd_pr_vbe_emerg("%s send data failed, ret_val=%d, size=%d \n", __func__, ret_val, sizeof(i2c_cmd_data));
    }
    else
    {
        rtd_pr_vbe_emerg("%s, i2c_src[0:6]=%d,%d,%d,%d,%d,%d\n",
            __func__, i2c_cmd_data[0], i2c_cmd_data[1], i2c_cmd_data[2], i2c_cmd_data[3], i2c_cmd_data[4], i2c_cmd_data[5], 
            i2c_cmd_data[6]);
    }

    return ret_val;

}

char vbe_rlink_set_freesync_info(unsigned int freesync)
{
    unsigned char i2c_cmd_data[i2c_rlink_Cmd_size_less8k + i2c_rlink_Host_vsc_Block_size + 1];
    unsigned char checksum;
    int ret_val = 0;

    memset(i2c_cmd_data, 0, sizeof(i2c_cmd_data));

    // cmd
    i2c_cmd_data[0] = I2C_ONLY_SCALER;
    i2c_cmd_data[1] = SCALER_RLK_I2C_ONLY_HOST_FREESYNC;

    // data
    i2c_cmd_data[2] = (freesync & 0xFF000000) >> 24;
    i2c_cmd_data[3] = (freesync & 0x00FF0000) >> 16;
    i2c_cmd_data[4] = (freesync & 0x0000FF00) >> 8;
    i2c_cmd_data[5] = (freesync & 0x000000FF);

    // checksum
    scaler_i2c_calc_checksum(&i2c_cmd_data[i2c_rlink_Cmd_size_less8k], i2c_rlink_Host_vsc_Block_size, &checksum);
    i2c_cmd_data[i2c_rlink_Cmd_size_less8k + i2c_rlink_Host_vsc_Block_size] = checksum;

    ret_val = i2c_master_send_ex_flag(H5X_I2C_PORT, H5X_I2C_ADDR, i2c_cmd_data, (unsigned short)sizeof(i2c_cmd_data), H5X_I2C_SPEED_FLAG);
    if (ret_val < 0)
    {
        // TODO. error handle
        rtd_pr_vbe_emerg("%s send data failed, ret_val=%d, size=%d \n", __func__, ret_val, sizeof(i2c_cmd_data));
    }
    else
    {
        rtd_pr_vbe_emerg("%s, i2c_src[0:6]=%d,%d,%d,%d,%d,%d\n",
            __func__, i2c_cmd_data[0], i2c_cmd_data[1], i2c_cmd_data[2], i2c_cmd_data[3], i2c_cmd_data[4], i2c_cmd_data[5], 
            i2c_cmd_data[6]);
    }

    return ret_val;

}

#endif

