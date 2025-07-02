//#include <rbus/dfe_p0_reg.h>

#ifndef uint32_t
typedef unsigned int __UINT32_T;
#define uint32_t __UINT32_T
#endif

//#define HDMI20_PORT  HDMI_PORT2

//dfe_20
#define HDMI_DFE0_BASE           (0xb800DE00)
#define HDMI_DFE1_BASE           (0xb803BE00)
#define HDMI_DFE2_BASE           (0xb803CE00)


struct hdmi_dfe_reg_st {
uint32_t en_l0;	//de00
uint32_t init0_l0;  //de04
uint32_t init1_l0;  //de08
uint32_t dummy1;  //de0c
uint32_t en_l1;    //de10
uint32_t init0_l1;  //de14
uint32_t init1_l1;  //de18
uint32_t dummy2;  //de1c
uint32_t en_l2;    //de20
uint32_t init0_l2;  //de24
uint32_t init1_l2;  //de28
uint32_t dummy3;  //de2c
uint32_t mode;  //de30
uint32_t gain;  //de34
uint32_t limit0;  //de38
uint32_t limit1;  //de3c
uint32_t limit2;  //de40
uint32_t readback;  //de44
uint32_t flag;  //de48
uint32_t debug;  //de4c
};


extern const struct hdmi_dfe_reg_st *dfe[3];


#ifdef CONFIG_ARM64
#define DFE_REG_DFE_EN_L0_reg                    ((unsigned long)(&dfe[nport]->en_l0))              // 0xB800DE00    
#define DFE_REG_DFE_INIT0_L0_reg               ((unsigned int)(unsigned long)(&dfe[nport]->init0_l0)) // 0xB800DE04  
#define DFE_REG_DFE_INIT1_L0_reg               ((unsigned int)(unsigned long)(&dfe[nport]->init1_l0)) // 0xB800DE08 
#define DFE_REG_DFE_EN_L1_reg                    ((unsigned int)(unsigned long)(&dfe[nport]->en_l1))  // 0xB800DE10      
#define DFE_REG_DFE_INIT0_L1_reg               ((unsigned int)(unsigned long)(&dfe[nport]->init0_l1)) // 0xB800DE14 
#define DFE_REG_DFE_INIT1_L1_reg               ((unsigned int)(unsigned long)(&dfe[nport]->init1_l1))// 0xB800DE18  
#define DFE_REG_DFE_EN_L2_reg                    ((unsigned int)(unsigned long)(&dfe[nport]->en_l2 )) // 0xB800DE20
#define DFE_REG_DFE_INIT0_L2_reg               ((unsigned int)(unsigned long)(&dfe[nport]->init0_l2))// 0xB800DE24 
#define DFE_REG_DFE_INIT1_L2_reg               ((unsigned int)(unsigned long)(&dfe[nport]->init1_l2))// 0xB800DE28  


#define DFE_REG_DFE_MODE_reg                     ((unsigned int)(unsigned long)(&dfe[nport]->mode)) // 0xB800DE30         
#define DFE_REG_DFE_GAIN_reg                      ((unsigned int)(unsigned long)(&dfe[nport]->gain    )) // 0xB800DE34   
#define DFE_REG_DFE_LIMIT0_reg                   ((unsigned int)(unsigned long)(&dfe[nport]->limit0  )) // 0xB800DE38 
#define DFE_REG_DFE_LIMIT1_reg                   ((unsigned int)(unsigned long)(&dfe[nport]->limit1  ))  // 0xB800DE3C
#define DFE_REG_DFE_LIMIT2_reg                   ((unsigned int)(unsigned long)(&dfe[nport]->limit2  )) // 0xB800DE40
#define DFE_REG_DFE_READBACK_reg              ((unsigned int)(unsigned long)(&dfe[nport]->readback))// 0xB800DE44  
#define DFE_REG_DFE_FLAG_reg                       ((unsigned int)(unsigned long)(&dfe[nport]->flag    ))// 0xB800DE48  
#define DFE_REG_DFE_DEBUG_reg                    ((unsigned int)(unsigned long)(&dfe[nport]->debug   ))// 0xB800DE4C 

#else
#define DFE_REG_DFE_EN_L0_reg                   ((unsigned int)(&dfe[nport]->en_l0))            // 0xB800DE00
#define DFE_REG_DFE_INIT0_L0_reg               ((unsigned int)(&dfe[nport]->init0_l0))              // 0xB800DE04 
#define DFE_REG_DFE_INIT1_L0_reg               ((unsigned int)(&dfe[nport]->init1_l0))              // 0xB800DE08
#define DFE_REG_DFE_EN_L1_reg                    ((unsigned int)(&dfe[nport]->en_l1))              // 0xB800DE10
#define DFE_REG_DFE_INIT0_L1_reg               ((unsigned int)(&dfe[nport]->init0_l1))              // 0xB800DE14
#define DFE_REG_DFE_INIT1_L1_reg               ((unsigned int)(&dfe[nport]->init1_l1))              // 0xB800DE18
#define DFE_REG_DFE_EN_L2_reg                    ((unsigned int)(&dfe[nport]->en_l2 ))              // 0xB800DE20
#define DFE_REG_DFE_INIT0_L2_reg               ((unsigned int)(&dfe[nport]->init0_l2))              // 0xB800DE24
#define DFE_REG_DFE_INIT1_L2_reg               ((unsigned int)(&dfe[nport]->init1_l2))              // 0xB800DE28


#define DFE_REG_DFE_MODE_reg                     ((unsigned int)(&dfe[nport]->mode))              // 0xB800DE30 
#define DFE_REG_DFE_GAIN_reg                      ((unsigned int)(&dfe[nport]->gain    ))              // 0xB800DE34
#define DFE_REG_DFE_LIMIT0_reg                   ((unsigned int)(&dfe[nport]->limit0  ))              // 0xB800DE38 
#define DFE_REG_DFE_LIMIT1_reg                   ((unsigned int)(&dfe[nport]->limit1  ))              // 0xB800DE3C 
#define DFE_REG_DFE_LIMIT2_reg                   ((unsigned int)(&dfe[nport]->limit2  ))              // 0xB800DE40
#define DFE_REG_DFE_READBACK_reg              ((unsigned int)(&dfe[nport]->readback))              // 0xB800DE44
#define DFE_REG_DFE_FLAG_reg                       ((unsigned int)(&dfe[nport]->flag    ))              // 0xB800DE48
#define DFE_REG_DFE_DEBUG_reg                    ((unsigned int)(&dfe[nport]->debug   ))              // 0xB800DE4C
#endif




#define LOAD_IN_INIT_LE		(_BIT31)
#define LOAD_IN_INIT_VTH	(_BIT30)
#define LOAD_IN_INIT_SERVO	(_BIT29)
#define LOAD_IN_INIT_TAP4	(_BIT28)
#define LOAD_IN_INIT_TAP3	(_BIT27)
#define LOAD_IN_INIT_TAP2	(_BIT26)
#define LOAD_IN_INIT_TAP1	(_BIT25)
#define LOAD_IN_INIT_TAP0	(_BIT24)
#define LOAD_IN_INIT_ALL	(LOAD_IN_INIT_LE|LOAD_IN_INIT_VTH|LOAD_IN_INIT_SERVO|LOAD_IN_INIT_TAP4|LOAD_IN_INIT_TAP3|LOAD_IN_INIT_TAP2|LOAD_IN_INIT_TAP1|LOAD_IN_INIT_TAP0)


#define REG_dfe_adapt_en_lane0_TAP0                                         _BIT16
#define REG_dfe_adapt_en_lane0_TAP1                                         _BIT17
#define REG_dfe_adapt_en_lane0_TAP2                                         _BIT18
#define REG_dfe_adapt_en_lane0_TAP3                                         _BIT19
#define REG_dfe_adapt_en_lane0_TAP4                                         _BIT20
#define REG_dfe_adapt_en_lane0_SERVO                                       _BIT21
#define REG_dfe_adapt_en_lane0_Vth                                            _BIT22
#define REG_dfe_adapt_en_lane0_LE                                              _BIT23
#define REG_dfe_adapt_en_lane1_TAP0                                         _BIT16
#define REG_dfe_adapt_en_lane1_TAP1                                         _BIT17
#define REG_dfe_adapt_en_lane1_TAP2                                         _BIT18
#define REG_dfe_adapt_en_lane1_TAP3                                         _BIT19
#define REG_dfe_adapt_en_lane1_TAP4                                         _BIT20
#define REG_dfe_adapt_en_lane1_SERVO                                       _BIT21
#define REG_dfe_adapt_en_lane1_Vth                                            _BIT22
#define REG_dfe_adapt_en_lane1_LE                                              _BIT23
#define REG_dfe_adapt_en_lane2_TAP0                                         _BIT16
#define REG_dfe_adapt_en_lane2_TAP1                                         _BIT17
#define REG_dfe_adapt_en_lane2_TAP2                                         _BIT18
#define REG_dfe_adapt_en_lane2_TAP3                                         _BIT19
#define REG_dfe_adapt_en_lane2_TAP4                                         _BIT20
#define REG_dfe_adapt_en_lane2_SERVO                                       _BIT21
#define REG_dfe_adapt_en_lane2_Vth                                            _BIT22
#define REG_dfe_adapt_en_lane2_LE                                              _BIT23
#define REG_dfe_adapt_en_lane3_TAP0                                         _BIT16
#define REG_dfe_adapt_en_lane3_TAP1                                         _BIT17
#define REG_dfe_adapt_en_lane3_TAP2                                         _BIT18
#define REG_dfe_adapt_en_lane3_TAP3                                         _BIT19
#define REG_dfe_adapt_en_lane3_TAP4                                         _BIT20
#define REG_dfe_adapt_en_lane3_SERVO                                       _BIT21
#define REG_dfe_adapt_en_lane3_Vth                                            _BIT22
#define REG_dfe_adapt_en_lane3_LE                                              _BIT23


#define LIMIT_TAP0_MAX				0
#define LIMIT_TAP0_MIN				1
#define LIMIT_TAP1_MAX				2
#define LIMIT_TAP1_MIN				3
#define LIMIT_LE_MAX				4
#define LIMIT_LE_MIN				5

#define COEF_VTH				0
#define COEF_TAP0				1
#define COEF_TAP1				2
#define COEF_TAP2				3
#define COEF_TAP3				4
#define COEF_TAP4				5
#define COEF_SERVO				6
#define COEF_LE					7


#define get_TAP2_coef_sign(data)                   	(0x20&(data))
#define get_TAP2_coef(data)                          	(0x1F&(data))
#define get_TAP3_coef_sign(data)                    	(0x20&(data))
#define get_TAP3_coef(data)                          	(0x1F&(data))
#define get_TAP4_coef_sign(data)                     (0x20&(data))
#define get_TAP4_coef(data)                          	(0x1F&(data))

