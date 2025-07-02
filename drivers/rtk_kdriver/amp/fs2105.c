#ifndef BUILD_QUICK_SHOW
#include <linux/i2c.h>
#include <linux/delay.h>
#else
#include <timer.h>
#endif
#include "fs2105.h"
#include "rtk_amp_interface.h"

#define END_FLAG 	(0xff)
#define AMP_DEBUG 	(0)




void FS2105P_func(int amp_i2c_id, unsigned short addr)
{
    unsigned char data[3];
    int index=0;

    AMP_ERR("\nFS2105P Initial Start\n");

    data[0] = 0x10;
    data[1] = 0x00;
    data[2] = 0x02;
    if(i2c_master_send_ex_flag(amp_i2c_id, addr, data, 3 ,I2C_M_NORMAL_SPEED) < 0){
        AMP_ERR("FS2105P err @ _line : %d \n", __LINE__);
    } ///* OSC On & HiZ */I2C_M_HIGH_SPEED
    mdelay(10);/* Wait DAC not in running */

    for(index = 0; index < (sizeof(FS2105P_InitTbl)/sizeof(FS2105_REG)); index ++)
    {
        data[0] = FS2105P_InitTbl[index].bAddr;
        data[1] = FS2105P_InitTbl[index].bArray[0];
        data[2] = FS2105P_InitTbl[index].bArray[1];
        if (data[0] == 0xff) {
            mdelay(20);
        } else if(i2c_master_send_ex_flag(amp_i2c_id, addr , data, 3 ,I2C_M_FAST_SPEED) < 0){
            AMP_ERR("program FS2105P failed\n");
        }
    }

    mdelay(20);
    AMP_ERR("\nFS2105P Initial End\n");
}

void FS2105_amp_reset(void)
{

    AMP_WARN("FS2105_amp_reset\n");
    rtk_amp_pin_set("PIN_AMP_RESET", 0);
    mdelay(20);
    rtk_amp_pin_set("PIN_AMP_RESET", 1);
    mdelay(1);
}

int FS2105_mute_set(int on_off, int amp_i2c_id,unsigned short slave_addr)
{
    unsigned char data[3] ={0};

    data[0] = 0x11;
    if(on_off){
        AMP_WARN("FS2105 unmute amp\n");
        data[1] = 0x03;    //--unmute amp
        data[2] = 0x00;
    }
    else{
        AMP_WARN("FS2105 mute amp\n");
        data[1] = 0x02;    //--mute amp
        data[2] = 0x00;
    }
    return i2c_master_send_ex(amp_i2c_id, slave_addr, data, 2);

}



int FS2105_param_set(unsigned char *data_wr, int amp_i2c_id, unsigned short slave_addr)
{

    unsigned char send_data[3] ={0};

    send_data[0] = data_wr[0];
    send_data[1] = data_wr[1];
    send_data[2] = data_wr[2];

    if ( i2c_master_send_ex(amp_i2c_id, slave_addr, &send_data[0], 3) < 0 ){
        AMP_ERR("I2C Write Reg:0x%x Data:0x%x 0x%xfailed\n",send_data[0],send_data[1],send_data[2]);
        return (-1);
    }
    else{
        AMP_WARN("Addr:0x%x Reg:0x%x Data:0x%x 0x%x\n", slave_addr,send_data[0],send_data[1],send_data[1],send_data[2]);
    }

    return 0;
}


int FS2105_param_get(unsigned char *reg, int amp_i2c_id, unsigned short slave_addr)
{
    int ret = 0;

    int result = 0;
    unsigned char addr[1] ={0};
    unsigned char data[2] ={0};

    //Read
    addr[0] = reg[0];
    if ( i2c_master_send_ex(amp_i2c_id, slave_addr, addr, FS2105_ADDR_SIZE) < 0 ){
        AMP_ERR("I2C Write Address Reg:0x%x failed\n",addr[0]);
        return (-1);
    }

    result = i2c_master_recv_ex(amp_i2c_id,slave_addr,NULL,0,data,FS2105_DATA_SIZE);

    if(result<0) {
        AMP_ERR("Read AMP REG:0x%x failed !! \n",addr[0]);
        return (-1);
    }

    AMP_WARN("Addr:0x%x Reg:0x%x Data:0x%x \n", slave_addr,addr[0],data[0]);

    return ret;
}

int FS2105_dump_all(const char *buf, int amp_i2c_id,unsigned short slave_addr)
{

    int ret = 0;
    int result = 0;
    int i = 0;
    unsigned char addr[1] ={0};
    unsigned char data[2] ={0};
    //Read Register
    AMP_WARN("== Dump  Reg  ==\n");
    for (i=0; i<(sizeof(FS2105_InitTbl)/sizeof(FS2105_REG)) ;++i){
        addr[0] = FS2105_InitTbl[i].bAddr;
        if ( i2c_master_send_ex(amp_i2c_id, slave_addr, &addr[0], FS2105_ADDR_SIZE) < 0 ){
            AMP_ERR("I2C Write Reg:0x%x failed\n",addr[0]);
            break;
        }

        result = i2c_master_recv_ex(amp_i2c_id,slave_addr,NULL,0,data,FS2105_DATA_SIZE);
        AMP_WARN(":Reg:0x%x Data:0x%x -0x%x\n", FS2105_InitTbl[i].bAddr,data[0], data[1]);
        if(result<0) {
            AMP_WARN("Read AMP REG:0x%x failed !! \n",addr[0]);
            break;
        }
    }

    return ret;
}

void FS2105_ram_init(int amp_i2c_id, unsigned short addr)
{
    unsigned char data[5];
    int index = 0;

    /*1. switch to HIZ mode then delay 30ms*/
    data[0] = 0x11;
    data[1] = 0x01;
    data[2] = 0x00;
    if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, 3 ,I2C_M_FAST_SPEED)< 0) {
        AMP_ERR("%s line: %d, program FS2105 failed \n", __func__, __LINE__);
        return;
    }
    mdelay(30);

    /*2. access key unlock*/
    data[0] = 0x0B;
    data[1] = 0x00;
    data[2] = 0x91;
    if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, 3 ,I2C_M_FAST_SPEED)< 0) {
        AMP_ERR("%s line: %d, program FS2105 failed \n", __func__, __LINE__);
        return;
    }
    /*3. set ram addr 0x0000, begin write gain mixer*/
    data[0] = 0x46;
    data[1] = 0x40;
    data[2] = 0x00;
    if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, 3 ,I2C_M_FAST_SPEED)< 0) {
        AMP_ERR("%s line: %d, program FS2105 failed \n", __func__, __LINE__);
        return;
    }

    for (index = 0; index < (sizeof(FS2105_Gain_Mixer_Tbl)/4); index ++) {
        data[0] = 0x42;
        data[1] = FS2105_Gain_Mixer_Tbl[index][0];
        data[2] = FS2105_Gain_Mixer_Tbl[index][1];
        data[3] = FS2105_Gain_Mixer_Tbl[index][2];
        data[4] = FS2105_Gain_Mixer_Tbl[index][3];
        if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, 5 ,I2C_M_FAST_SPEED)< 0) {
            AMP_ERR("%s line: %d, program FS2105 failed \n", __func__, __LINE__);
            return;
        }
    }

    /*4. set ram addr 0x0083, begin write EQ MBDRC PostDrc clipper */
    data[0] = 0x46;
    data[1] = 0x40;
    data[2] = 0x83;
    if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, 3 ,I2C_M_FAST_SPEED)< 0) {
        AMP_ERR("%s line: %d, program FS2105 failed \n", __func__, __LINE__);
        return;
    }

    for (index = 0; index < (sizeof(FS2105_EQ_Tbl)/4); index ++) {
        data[0] = 0x42;
        data[1] = FS2105_EQ_Tbl[index][0];
        data[2] = FS2105_EQ_Tbl[index][1];
        data[3] = FS2105_EQ_Tbl[index][2];
        data[4] = FS2105_EQ_Tbl[index][3];
        if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, 5 ,I2C_M_FAST_SPEED)< 0) {
            AMP_ERR("%s line: %d, program FS2105 failed \n", __func__, __LINE__);
            return;
        }
    }
    /*5. recovery */
    data[0] = 0x46;
    data[1] = 0x00;
    data[2] = 0x00;
    if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, 3 ,I2C_M_FAST_SPEED)< 0) {
        AMP_ERR("%s line: %d, program FS2105 failed \n", __func__, __LINE__);
        return;
    }

    data[0] = 0x0B;
    data[1] = 0x00;
    data[2] = 0x00;
    if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, 3 ,I2C_M_FAST_SPEED)< 0) {
        AMP_ERR("%s line: %d, program FS2105 failed \n", __func__, __LINE__);
        return;
    }


    /*6. switch to play mode */
    data[0] = 0x11;
    data[1] = 0x03;
    data[2] = 0x00;
    if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, 3 ,I2C_M_FAST_SPEED)< 0) {
        AMP_ERR("%s line: %d, program FS2105 failed \n", __func__, __LINE__);
        return;
    }
    mdelay(30);

}

void FS2105_reg_init(int amp_i2c_id, unsigned short addr)
{
    unsigned char data[3];
    int index = 0;

    for(index = 0; index < (sizeof(FS2105_InitTbl)/sizeof(FS2105_REG)); index ++)
    {
        data[0] = FS2105_InitTbl[index].bAddr;
        data[1] = FS2105_InitTbl[index].bArray[0];
        data[2] = FS2105_InitTbl[index].bArray[1];
        if (i2c_master_send_ex_flag(amp_i2c_id, addr , data, 3 ,I2C_M_FAST_SPEED) < 0)
            AMP_ERR("program FS2105 failed\n");
    }
}

void FS2105_func(int amp_i2c_id, unsigned short addr)
{
    AMP_ERR("\nFS2105 Initial Begin-\n");
    FS2105_reg_init(amp_i2c_id, addr);
    FS2105_ram_init(amp_i2c_id, addr);
    AMP_ERR("\nFS2105 Initial End-\n");
}


