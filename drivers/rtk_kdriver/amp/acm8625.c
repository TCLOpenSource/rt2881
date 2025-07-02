#ifndef BUILD_QUICK_SHOW
#include <linux/i2c.h>
#include <linux/delay.h>
#else
#include <timer.h>
#endif
#include "acm8625.h"
#include "rtk_amp_interface.h"

#define END_FLAG 	(0xff)
#define AMP_DEBUG 	(0)


void acm8625_func(int amp_i2c_id, unsigned short addr)
{
    unsigned char data[2];
    unsigned char data_len=0;
    int index=0;


    AMP_ERR("\nACM8625 Initial Start\n");
    mdelay(1);
    for(index = 0; index < (sizeof(ACM8625_Initial_InitTbl)/sizeof(ACM8625_REG)); index ++)
    {
        data[0] = ACM8625_Initial_InitTbl[index].bAddr;
        data_len = 1;
        data[1] = ACM8625_Initial_InitTbl[index].bArray;
#ifdef AMP_DEBUG
        AMP_WARN("===write addr:0x%x len:%d===\n",data[0],data_len);
        AMP_WARN("%x ", data[1]);
#endif
        if (i2c_master_send_ex_flag(amp_i2c_id, addr , data, data_len+1 ,I2C_M_FAST_SPEED) < 0)
            AMP_ERR("program ACM8625 failed\n");
    }
    mdelay(10);


    for(index = 0; index < (sizeof(ACM8625_Reg_InitTbl)/sizeof(ACM8625_REG)); index ++)
    {
        data[0] = ACM8625_Reg_InitTbl[index].bAddr;
        data_len = 1;
        data[1] = ACM8625_Reg_InitTbl[index].bArray;
#ifdef AMP_DEBUG
        AMP_WARN("===write addr:0x%x len:%d===\n",data[0],data_len);
        AMP_WARN("%x ", data[1]);
#endif
        if (i2c_master_send_ex_flag(amp_i2c_id, addr , data, data_len+1 ,I2C_M_FAST_SPEED) < 0)
            AMP_ERR("program ACM8625 failed\n");
    }

    AMP_ERR("\nACM8625 Initial End\n");
}



void acm8625_amp_reset(void)
{

    AMP_WARN("FS2105_amp_reset\n");
    rtk_amp_pin_set("PIN_AMP_RESET", 0);
    mdelay(10);
    rtk_amp_pin_set("PIN_AMP_RESET", 1);
    mdelay(5);
}

int acm8625_mute_set(int on_off, int amp_i2c_id,unsigned short slave_addr)
{
    unsigned char data[2] ={0};

    data[0] = 0x00;
    data[1] = 0x00;
    if(i2c_master_send_ex(amp_i2c_id, slave_addr, data, 2) < 0){
        AMP_ERR("ACM8625 err @ _line : %d \n", __LINE__);
    }
    
    data[0] = 0x04;
    if(on_off){
        AMP_WARN("FS2105 mute amp\n");
        data[1] = 0x0f;    //--mute amp  mute both left & right channels
    }
    else{
        AMP_WARN("FS2105 unmute amp\n");
        data[1] = 0x03;    //--unmute amp
    }
    return i2c_master_send_ex(amp_i2c_id, slave_addr, data, 2);

}



int acm8625_param_set(unsigned char *data_wr, int amp_i2c_id, unsigned short slave_addr)
{

    unsigned char send_data[2] ={0};

    send_data[0] = data_wr[0];
    send_data[1] = data_wr[1];

    if ( i2c_master_send_ex(amp_i2c_id, slave_addr, &send_data[0], 2) < 0 ){
        AMP_ERR("I2C Write Reg:0x%x Data:0x%x 0x%xfailed\n",send_data[0],send_data[1]);
        return (-1);
    }
    else{
        AMP_WARN("Addr:0x%x Reg:0x%x Data:0x%x 0x%x\n", slave_addr,send_data[0],send_data[1],send_data[1]);
    }

    return 0;
}


int acm8625_param_get(unsigned char *reg, int amp_i2c_id, unsigned short slave_addr)
{
    int ret = 0;

    int result = 0;
    unsigned char addr[1] ={0};
    unsigned char data[1] ={0};

    //Read
    addr[0] = reg[0];
    if ( i2c_master_send_ex(amp_i2c_id, slave_addr, addr, ACM8625_ADDR_SIZE) < 0 ){
        AMP_ERR("I2C Write Address Reg:0x%x failed\n",addr[0]);
        return (-1);
    }

    result = i2c_master_recv_ex(amp_i2c_id,slave_addr,NULL,0,data,ACM8625_DATA_SIZE);

    if(result<0) {
        AMP_ERR("Read AMP REG:0x%x failed !! \n",addr[0]);
        return (-1);
    }

    AMP_WARN("Addr:0x%x Reg:0x%x Data:0x%x \n", slave_addr,addr[0],data[0]);

    return ret;
}

int acm8625_dump_all(const char *buf, int amp_i2c_id,unsigned short slave_addr)
{

    int ret = 0;
    int result = 0;
    int i = 0;
    unsigned char addr[1] ={0};
    unsigned char data[2] ={0};
    //Read Register
    AMP_WARN("== Dump  Reg  ==\n");
    for (i=0; i<(sizeof(ACM8625_Initial_InitTbl)/sizeof(ACM8625_REG)) ;++i){
        addr[0] = ACM8625_Initial_InitTbl[i].bAddr;
        if ( i2c_master_send_ex(amp_i2c_id, slave_addr, &addr[0], ACM8625_ADDR_SIZE) < 0 ){
            AMP_ERR("I2C Write Reg:0x%x failed\n",addr[0]);
            break;
        }

        result = i2c_master_recv_ex(amp_i2c_id,slave_addr,NULL,0,data,ACM8625_DATA_SIZE);
        AMP_WARN(":Reg:0x%x Data:0x%x -0x%x\n", ACM8625_Initial_InitTbl[i].bAddr,data[0], data[1]);
        if(result<0) {
            AMP_WARN("Read AMP REG:0x%x failed !! \n",addr[0]);
            break;
        }
    }

    return ret;
}




