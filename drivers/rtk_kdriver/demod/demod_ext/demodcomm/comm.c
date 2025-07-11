#include <rtk_kdriver/i2c-rtk-api.h>
#include "tv_osal.h"
#include "rtk_tv_fe_types.h"
#include "comm.h"

#define MAX_TX_SIZE         (255)
#define MAX_RX_SIZE         (255)

#define S_OK 1
 
#define DECODE_RET(x)       ((x==S_OK) ? TUNER_CTRL_OK : TUNER_CTRL_FAIL )

extern int i2c_master_recv_ex_flag(unsigned char bus_id, unsigned char addr,
        unsigned char *p_sub_addr, unsigned char sub_addr_len,
        unsigned char *p_read_buff, unsigned int read_len, __u16 flags);


extern int i2c_master_send_ex_flag(unsigned char bus_id, unsigned char addr,
        unsigned char *write_buff, unsigned int write_len, __u16 flags);


void Comm_Constructors_Ext(COMM* pCOMMKernel,unsigned char Id)
{
	pCOMMKernel->m_max_tx_size = 0;
	pCOMMKernel->m_max_rx_size = 0;

    pCOMMKernel->m_Id            = Id;    
    pCOMMKernel->m_max_tx_size   = MAX_TX_SIZE;
    pCOMMKernel->m_max_rx_size   = MAX_RX_SIZE;  

    pCOMMKernel->Init = Comm_Init_Ext;
    pCOMMKernel->SendReadCommand = Comm_SendReadCommand_Ext;
    pCOMMKernel->SendWriteCommand =  Comm_SendWriteCommand_Ext;
    pCOMMKernel->SetTargetName =  Comm_SetTargetName_Ext;
	pCOMMKernel->Destory = Comm_Destructors_Ext;
}

void Comm_Destructors_Ext(COMM* pCOMMKernel)
{
}

int Comm_Init_Ext(COMM* pCOMMKernel){

// ^^^^^^^^^^^^^^^^^^^  
    return TUNER_CTRL_OK; 
#if 0
    I2C_Init();
    
    I2C_SetSpeed(pCOMMKernel->m_Id, 100);
    return TUNER_CTRL_OK; 
#endif
}

int Comm_SendReadCommand_Ext(
	COMM*            pCOMMKernel,
	unsigned char           DeviceAddr, 
	unsigned char*          pRegisterAddr,
	unsigned char           RegisterAddrSize,
	unsigned char*          pReadingBytes,
	unsigned char           ByteNum,
	U32BITS           Flags)
{

#if 0
// ^^^^^^^^^^^^^^^^^^^  
	return TUNER_CTRL_OK;
#else
	int ret;
	//COMM_DBG("i2c read\n");
	ret = i2c_master_recv_ex_flag(pCOMMKernel->m_Id, DeviceAddr>>1,pRegisterAddr,RegisterAddrSize,pReadingBytes, ByteNum, Flags);

#ifdef SHOW_RW_MSG
    int j;
    COMM_WARNING("RD[%02x] ",DeviceAddr);
    
    for(j=0; j<RegisterAddrSize; j++) {        
        COMM_WARNING("<%02x> ",pRegisterAddr[j]);        
    }            
    
    for(j=0; j<ByteNum; j++) {
        COMM_WARNING("%02x ",pReadingBytes[j]);
    }
            
    COMM_WARNING("\n");         
#endif            

#ifdef SHOW_ERR_MSG                     
    
    if (ret!=S_OK)
    {
        int i;
        
        COMM_WARNING("\n\n#################################################\n");
        COMM_WARNING("%s : i2c %d : FETAL : send random read command to target(%02x) fail\n",__FUNCTION__, pCOMMKernel->m_Id, DeviceAddr);
        COMM_WARNING("RR [%02x] ",DeviceAddr);
        
        for(i=0; i<RegisterAddrSize; i++) {        
            COMM_WARNING("<%02x> ",pRegisterAddr[i]);        
        }            
        
        for(i=0; i<ByteNum; i++) {
            COMM_WARNING("%02x ",pReadingBytes[i]);
        }
            
        COMM_WARNING("\n\n#################################################\n"); 
    }        
    
#endif                      
    return TUNER_CTRL_OK;
#endif
    
}

int Comm_SendWriteCommand_Ext(
	COMM*            pCOMMKernel,
    unsigned char           DeviceAddr,        
    unsigned char*          pWritingBytes,
    unsigned int           ByteNum,
    U32BITS           Flags)
{
#if 0
// ^^^^^^^^^^^^^^^^^^^  
	return TUNER_CTRL_OK;
#else
	//COMM_DBG("rtk_demod i2c write m_Id=0x%x DeviceAddr=0x%x ByteNum=%d\n", pCOMMKernel->m_Id, DeviceAddr,ByteNum);

	if(i2c_master_send_ex_flag(pCOMMKernel->m_Id, DeviceAddr>>1, pWritingBytes, ByteNum, Flags) < 0)
	{
		return TUNER_CTRL_FAIL;
	}
	else
	{
		return TUNER_CTRL_OK;
    }
	//ret = i2c_master_send_ex(pCOMMKernel->m_Id, DeviceAddr>>1, pWritingBytes, ByteNum);

#ifdef SHOW_RW_MSG    
    COMM_WARNING("WR[%02x] ",DeviceAddr);
    
    for( j=0; j<ByteNum; j++) 
    { 
        COMM_WARNING("%02x ",pWritingBytes[j]);        
        tv_osal_msleep(5);
    }               
    COMM_WARNING("\n");         
#endif            
    
#ifdef SHOW_ERR_MSG                  
        COMM_WARNING("\n\n#################################################\n");
        COMM_WARNING("%s : i2c %d : FETAL : send write command to target(%02x) fail\n",__FUNCTION__, pCOMMKernel->m_Id, DeviceAddr);
        COMM_WARNING("Write[%02x] ",DeviceAddr);
        for(int i=0;i<ByteNum; i++)                    
            COMM_WARNING("%02x ",pWritingBytes[i]);                             
        COMM_WARNING("\n#################################################\n\n");                
#endif        
#endif
}          
    
   
int Comm_SetTargetName_Ext(
	COMM*            pCOMMKernel,
	unsigned char           DeviceAddr,        
	const char*             Name)
{
// ^^^^^^^^^^^^^^^^^^^  
	return TUNER_CTRL_OK;
#if 0
    if (I2C_Set_Target_Name_EX(pCOMMKernel->m_Id, DeviceAddr>>1,Name)==S_OK)
        return TUNER_CTRL_OK;
    else
        return TUNER_CTRL_FAIL;
#endif
}

