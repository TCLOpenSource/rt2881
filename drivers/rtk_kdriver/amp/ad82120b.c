#ifndef BUILD_QUICK_SHOW
#include <linux/i2c.h>
#include <linux/delay.h>
#else
#include <timer.h>
#endif
#include "rtk_amp_interface.h"
#include "ad82120b.h"
#include <rtk_kdriver/rtk_gpio.h>


void ad82120b_func(int amp_i2c_id,unsigned short addr)
{
    unsigned char data[2];
    unsigned char data_len=1;
    unsigned char index=0;

    int retry_cnt = 5;
    AMP_ERR("\nAD82120B Initial Start\n");

    // software reset amp
    data[0] = 0x1a;
    data[1] = 0x00;
    for (index = 0; index <= retry_cnt; index++) {
        if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1, I2C_M_FAST_SPEED) < 0) { //--reset amp
            AMP_ERR("%s line: %d, program AD82120B failed (retry=%d)\n", __func__, __LINE__, index);
            mdelay(5);
            if(index == retry_cnt) return;
        }
        else {
            break;
        }
    }
	
    mdelay(5);

    data[0] = 0x1a;
    data[1] = 0x28;
    if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1, I2C_M_FAST_SPEED) < 0)//--Normal operation
            AMP_ERR("%s line: %d, program AD82120B failed\n", __func__, __LINE__);

    mdelay(20);
    data[0] = 0x02;
    data[1] = 0x7f;
    if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1, I2C_M_FAST_SPEED) < 0) //--mute amp
        AMP_ERR("%s line: %d, program AD82120B failed\n", __func__, __LINE__);

    for(index = 0; index < (sizeof(AD82120B_InitTbl)/sizeof(AD82120B_REG)); index ++)
    {
        data[0] = AD82120B_InitTbl[index].bAddr;
        data_len = 1;
        data[1] = AD82120B_InitTbl[index].bArray;
        if(data[0] == 0x02)
            continue;
        if(data[0] >= 0x71 && data[0]<= 0x7C)
            continue;

#ifdef AD82120B_DEBUG
        AMP_WARN("===write addr:0x%x len:%d===\n",data[0],data_len);
        AMP_WARN("%x ", data[1]);
#endif
        if (i2c_master_send_ex_flag(amp_i2c_id, addr , data, data_len+1 ,I2C_M_FAST_SPEED) < 0)
            AMP_ERR("program AD82120B failed\n");
    }

    data[0] = 0x02;
    data[1] = 0x7f;
    if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1 ,I2C_M_FAST_SPEED) < 0) //--mute amp
        AMP_ERR("%s line: %d, program AD82120 failed\n", __func__, __LINE__);
    udelay(100);

    data[0] = 0x02;
    data[1] = 0x00;
    if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1 ,I2C_M_FAST_SPEED) < 0) //--unmute amp
        AMP_ERR("%s line: %d, program AD82120 failed\n", __func__, __LINE__);
    AMP_ERR("\nAD82120B Initial End\n");
}

void ad82120bm_func(int amp_i2c_id,unsigned short addr)
{
    unsigned char data[2];
    unsigned char data_len=1;
    unsigned char index=0;

    int retry_cnt = 5;
    AMP_ERR("\nAD82120B_Mono Initial Start\n");

    // software reset amp
    data[0] = 0x1a;
    data[1] = 0x00;
    for (index = 0; index <= retry_cnt; index++) {
        if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1, I2C_M_FAST_SPEED) < 0) { //--reset amp
            AMP_ERR("%s line: %d, program AD82120Bm failed (retry=%d)\n", __func__, __LINE__, index);
            mdelay(5);
            if(index == retry_cnt) return;
        }
        else {
            break;
        }
    }
    mdelay(5);

    data[0] = 0x1a;
    data[1] = 0x68;
    if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1, I2C_M_FAST_SPEED) < 0) //--Normal operation, MONO
            AMP_ERR("%s line: %d, program AD82120Bm failed\n", __func__, __LINE__);
        mdelay(20);

    data[0] = 0x02;
    data[1] = 0x7f;
    if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1, I2C_M_FAST_SPEED) < 0) //--mute amp
        AMP_ERR("%s line: %d, program AD82120Bm failed\n", __func__, __LINE__);

    for(index = 0; index < (sizeof(AD82120BM_InitTbl)/sizeof(AD82120B_REG)); index ++)
    {
        data[0] = AD82120BM_InitTbl[index].bAddr;
        data_len = 1;
        data[1] = AD82120BM_InitTbl[index].bArray;
        if(data[0] == 0x02)
            continue;
        if(data[0] >= 0x71 && data[0]<= 0x7C)
            continue;
		//set mono
		if(data[0] == 0x1A){
			data[1] =((AD82120BM_InitTbl[index].bArray)| 0x40);
			if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1, I2C_M_FAST_SPEED) < 0){ //--mute amp
        	AMP_ERR("%s line: %d, program AD82120Bm failed\n", __func__, __LINE__);
			}
			continue;
		}

		if(data[0] == 0x5b){
			data[1] =0x30;
			if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1, I2C_M_FAST_SPEED) < 0){ //--mute amp
        	AMP_ERR("%s line: %d, program AD82120Bm failed\n", __func__, __LINE__);
			}
			continue;
		}

		if(data[0] == 0x5c){
			data[1] =0x06;
			if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1, I2C_M_FAST_SPEED) < 0){ //--mute amp
        	AMP_ERR("%s line: %d, program AD82120Bm failed\n", __func__, __LINE__);
			}
			continue;
		}

        if (i2c_master_send_ex_flag(amp_i2c_id, addr , data, data_len+1 ,I2C_M_FAST_SPEED) < 0)
            AMP_ERR("program AD82120Bm failed\n");
    }

    data[0] = 0x02;
    data[1] = 0x7f;
    if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1 ,I2C_M_FAST_SPEED) < 0) //--mute amp
        AMP_ERR("%s line: %d, program AD82120Bm failed\n", __func__, __LINE__);
    udelay(100);

    data[0] = 0x02;
    data[1] = 0x00;
    if (i2c_master_send_ex_flag(amp_i2c_id, addr, data, data_len+1 ,I2C_M_FAST_SPEED) < 0) //--unmute amp
        AMP_ERR("%s line: %d, program AD82120Bm failed\n", __func__, __LINE__);
    AMP_ERR("\nAD82120B_Mono Initial End\n");
}

int ad82120b_dump_all(const char *buf, int amp_i2c_id,unsigned short slave_addr)
{

    int ret = 0;
    int result = 0;
    int i = 0;
    unsigned char addr[1] ={0};
    unsigned char data[2] ={0};
    //Read Register
    AMP_WARN("== Dump 00~83 Reg ==\n");
    for (i=0x00; i<(sizeof(AD82120B_InitTbl)/sizeof(AD82120B_REG)) ;++i){
        addr[0] = AD82120B_InitTbl[i].bAddr;
        if ( i2c_master_send_ex(amp_i2c_id, slave_addr, &addr[0], 1) < 0 ){
            AMP_ERR("I2C Write Reg:0x%x failed\n",addr[0]);
            break;
        }

        result = i2c_master_recv_ex(amp_i2c_id,slave_addr,NULL,0,data,1);
        AMP_WARN("Reg:0x%x Data:0x%x\n", addr[0],data[0]);
        if(result<0) {
            AMP_ERR("Read AMP REG:0x%x failed !! \n",addr[0]);
            break;
       }
    }

    return ret;
}




int ad82120b_mute_set(int on_off, int amp_i2c_id, unsigned short slave_addr)
{
    unsigned char data[2] ={0};

    data[0] = 0x02;
    if(on_off){
        AMP_WARN("AD82120B unmute amp\n");
        data[1] = 0x00;    //--unmute amp
    }
    else{
        AMP_WARN("AD82120B mute amp\n");
        data[1] = 0x7f;    //--mute amp
    }
    return i2c_master_send_ex(amp_i2c_id, slave_addr, data, 2); 
}
int ad82120b_param_get(unsigned char *reg, int amp_i2c_id, unsigned short slave_addr)
{

    int ret = 0;
    int result = 0;
    unsigned char addr[1] ={0};
    unsigned char data[1] ={0};

    //Read
    addr[0] = reg[0];
    if ( i2c_master_send_ex(amp_i2c_id, slave_addr, &addr[0], 1) < 0 ){
        AMP_ERR("I2C Write Address Reg:0x%x failed\n",addr[0]);
        return (-1);
    }

    result = i2c_master_recv_ex(amp_i2c_id,slave_addr,NULL,0,data,AD82120B_DATA_SIZE);

    if(result<0) {
        AMP_ERR("Read AMP REG:0x%x failed !! \n",*reg);
        return (-1);
    }

    AMP_WARN("Addr:0x%x Reg:0x%x Data:0x%x \n", slave_addr,addr[0],data[0]);

    return ret;
}
int ad82120b_param_set(unsigned char *data_wr, int amp_i2c_id, unsigned short slave_addr)
{
    unsigned char send_data[2] ={0};

    send_data[0] = data_wr[0];
    send_data[1] = data_wr[1];

    if ( i2c_master_send_ex(amp_i2c_id, slave_addr, &send_data[0], 2) < 0 ){
        AMP_ERR("I2C Write Reg:0x%x Data:0x%x failed\n",send_data[0],send_data[1]);
        return (-1);
    }
    else{
        AMP_WARN("Addr:0x%x Reg:0x%x Data:0x%x \n", slave_addr,send_data[0],send_data[1]);
    }

    return 0;

}




