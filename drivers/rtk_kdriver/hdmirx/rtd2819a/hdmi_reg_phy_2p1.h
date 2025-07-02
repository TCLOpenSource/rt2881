//#include <rbus/hdmirx_2p1_phy_reg.h>
#include <rbus/efuse_reg.h>

//////////////////////
//Copy from Mac8p

//#include <rbus/hdmirx_2p0_phy_reg.h>  //From Mac7p need replace TBD
//#include <rbus/gdirx_hdmi20_phy_p0_reg.h>  //Mac8p
#include <rbus/hdmirx_phy_reg.h>  //Mac8p-> Merlin8-> Mac9p
//#include <rbus/gdirx_hdmi21_dp14_dfe_p0_reg.h>
//#include <rbus/gdirx_hdmi20_dp14_phy_p1_regNew.h>  //Mac8p
//#include <rbus/gdirx_hdmi20_dp14_phy_p1_regNew.h>  //Mac8p


//////////////////////
//Copy from Mark2
#define HDMI_APHY0_BASE	    (0xB80B4204)   //4204 ~ 42bc (P0)   
#define HDMI_APHY1_BASE	    (0xB80B4A04) 
#define HDMI_APHY2_BASE	    (0xB800A204)   
#define HDMI_APHY3_BASE	    (0xB800AA04) 

#define HDMI_DPHY0_BASE	    (0xB80B4300)   //430 ~ 45FC (P0) 
#define HDMI_DPHY1_BASE	    (0xB80B4B00)  
#define HDMI_DPHY2_BASE	    (0xB800A300)    
#define HDMI_DPHY3_BASE	    (0xB800AB00)   

#define HDMI_APHY0_RSTB_BASE	    (0xB80B42FC) 
#define HDMI_APHY1_RSTB_BASE	    (0xB80B4AFC) 
#define HDMI_APHY2_RSTB_BASE	    (0xB800A2FC) 
#define HDMI_APHY3_RSTB_BASE	    (0xB800AAFC) 


struct hdmi21_aphy_reg_st {
	uint32_t z0k;  //0x180B4204
	uint32_t z0pow;  //0x180B4208
	uint32_t rsvd0;  //0x180B420C
	uint32_t top_in_0;  //0x180B4210
	uint32_t top_in_1;  //0x180B4214	
	uint32_t top_in_2;  //0x180B4218	
	uint32_t rsvd1;  //0x180B421C	
	uint32_t top_out_0;  //0x180B4220
	uint32_t top_out_1; //0x180B4224
	uint32_t rsvd2;  //0x180B4228
	uint32_t rsvd3;  //0x180B422C
	uint32_t ck0; //0x180B4230
	uint32_t ck1; //0x180B4234
	uint32_t ck2; //0x180B4238
	uint32_t ck3; //0x180B423C
	uint32_t ck4; //0x180B4240
	uint32_t acdr_ck0;  //0x180B4244
	uint32_t acdr_ck1;  //0x180B4248
	uint32_t acdr_ck2;  //0x180B424C
	uint32_t b0;  //0x180B4250
	uint32_t b1;  //0x180B4254
	uint32_t b2;  //0x180B4258
	uint32_t b3;  //0x180B425C
	uint32_t b4;  //0x180B4260
	uint32_t acdr_b0;  //0x180B4264
	uint32_t acdr_b1;  //0x180B4268
	uint32_t acdr_b2;  //0x180B426C
	uint32_t g0;  //0x180B4270
	uint32_t g1;  //0x180B4274
	uint32_t g2;  //0x180B4278
	uint32_t g3;  //0x180B427C
	uint32_t g4;  //0x180B4280
	uint32_t acdr_g0;  //0x180B4284
	uint32_t acdr_g1;  //0x180B4288
	uint32_t acdr_g2;  //0x180B428C
	uint32_t r0;  //0x180B4290
	uint32_t r1;  //0x180B4294
	uint32_t r2;  //0x180B4298
	uint32_t r3;  //0x180B429C
	uint32_t r4;  //0x180B42A0
	uint32_t acdr_r0;  //0x180B42A4
	uint32_t acdr_r1;  //0x180B42A8
	uint32_t acdr_r2;  //0x180B42AC
	uint32_t rsvd4;  //0x180B42B0
	uint32_t ck_cmu_1;  //0x180B42B4;
	uint32_t ck_cmu_2;  //0x180B42B8
	uint32_t ck_cmu_3;  //0x180B42BC
};

extern const struct hdmi21_aphy_reg_st *hd21_aphy[4];


///////////////////////////////////////////////////
#ifdef CONFIG_ARM64

#define HDMIRX_PHY_z0k_reg     	 	(unsigned int)(unsigned long)(&hd21_aphy[nport]->z0k)//0x180B4204            
#define HDMIRX_PHY_z0pow_reg    	(unsigned int)(unsigned long)(&hd21_aphy[nport]->z0pow)//0x180B4208	        
#define HDMIRX_PHY_top_in_0_reg 	(unsigned int)(unsigned long)(&hd21_aphy[nport]->top_in_0)//0x180B4210       
#define HDMIRX_PHY_top_in_1_reg 	(unsigned int)(unsigned long)(&hd21_aphy[nport]->top_in_1)//0x180B4214	      
#define HDMIRX_PHY_top_in_2_reg 	(unsigned int)(unsigned long)(&hd21_aphy[nport]->top_in_2)//0x180B4218	      
#define HDMIRX_PHY_top_out_0_reg 	(unsigned int)(unsigned long)(&hd21_aphy[nport]->top_out_0)//0x180B4220      
#define HDMIRX_PHY_top_out_1_reg 	(unsigned int)(unsigned long)(&hd21_aphy[nport]->top_out_1)//0x180B4224       
#define HDMIRX_PHY_rsvd0_reg    		(unsigned int)(unsigned long)(&hd21_aphy[nport]->rsvd0)//0x180B4228          
#define HDMIRX_PHY_rsvd1_reg		(unsigned int)(unsigned long)(&hd21_aphy[nport]->rsvd1)//0x180B422C          
#define HDMIRX_PHY_ck0_reg			(unsigned int)(unsigned long)(&hd21_aphy[nport]->ck0)//0x180B4230             
#define HDMIRX_PHY_ck1_reg			(unsigned int)(unsigned long)(&hd21_aphy[nport]->ck1)//0x180B4234             
#define HDMIRX_PHY_ck2_reg 		(unsigned int)(unsigned long)(&hd21_aphy[nport]->ck2)//0x180B4238             
#define HDMIRX_PHY_ck3_reg 		(unsigned int)(unsigned long)(&hd21_aphy[nport]->ck3)//0x180B423C             
#define HDMIRX_PHY_ck4_reg			(unsigned int)(unsigned long)(&hd21_aphy[nport]->ck4)//0x180B4240             
#define HDMIRX_PHY_acdr_ck0_reg	 (unsigned int)(unsigned long)(&hd21_aphy[nport]->acdr_ck0)//0x180B4244       
#define HDMIRX_PHY_acdr_ck1_reg	 (unsigned int)(unsigned long)(&hd21_aphy[nport]->acdr_ck1)//0x180B4248       
#define HDMIRX_PHY_acdr_ck2_reg 	(unsigned int)(unsigned long)(&hd21_aphy[nport]->acdr_ck2)//0x180B424C       
#define HDMIRX_PHY_b0_reg       		(unsigned int)(unsigned long)(&hd21_aphy[nport]->b0)//0x180B4250             
#define HDMIRX_PHY_b1_reg       		(unsigned int)(unsigned long)(&hd21_aphy[nport]->b1)//0x180B4254             
#define HDMIRX_PHY_b2_reg       		(unsigned int)(unsigned long)(&hd21_aphy[nport]->b2)//0x180B4258             
#define HDMIRX_PHY_b3_reg       		(unsigned int)(unsigned long)(&hd21_aphy[nport]->b3)//0x180B425C             
#define HDMIRX_PHY_b4_reg      		 (unsigned int)(unsigned long)(&hd21_aphy[nport]->b4)//0x180B4260             
#define HDMIRX_PHY_acdr_b0_reg  	(unsigned int)(unsigned long)(&hd21_aphy[nport]->acdr_b0)//0x180B4264        
#define HDMIRX_PHY_acdr_b1_reg  	(unsigned int)(unsigned long)(&hd21_aphy[nport]->acdr_b1)//0x180B4268        
#define HDMIRX_PHY_acdr_b2_reg 	 (unsigned int)(unsigned long)(&hd21_aphy[nport]->acdr_b2)//0x180B426C        
#define HDMIRX_PHY_g0_reg       		(unsigned int)(unsigned long)(&hd21_aphy[nport]->g0)//0x180B4270             
#define HDMIRX_PHY_g1_reg       		(unsigned int)(unsigned long)(&hd21_aphy[nport]->g1)//0x180B4274             
#define HDMIRX_PHY_g2_reg      		(unsigned int)(unsigned long)(&hd21_aphy[nport]->g2)//0x180B4278             
#define HDMIRX_PHY_g3_reg       		(unsigned int)(unsigned long)(&hd21_aphy[nport]->g3)//0x180B427C             
#define HDMIRX_PHY_g4_reg			(unsigned int)(unsigned long)(&hd21_aphy[nport]->g4)//0x180B4280             
#define HDMIRX_PHY_acdr_g0_reg  	(unsigned int)(unsigned long)(&hd21_aphy[nport]->acdr_g0)//0x180B4284        
#define HDMIRX_PHY_acdr_g1_reg		(unsigned int)(unsigned long)(&hd21_aphy[nport]->acdr_g1)//0x180B4288        
#define HDMIRX_PHY_acdr_g2_reg		(unsigned int)(unsigned long)(&hd21_aphy[nport]->acdr_g2)//0x180B428C        
#define HDMIRX_PHY_r0_reg			(unsigned int)(unsigned long)(&hd21_aphy[nport]->r0)//0x180B4290             
#define HDMIRX_PHY_r1_reg			(unsigned int)(unsigned long)(&hd21_aphy[nport]->r1)//0x180B4294             
#define HDMIRX_PHY_r2_reg			(unsigned int)(unsigned long)(&hd21_aphy[nport]->r2)//0x180B4298             
#define HDMIRX_PHY_r3_reg			(unsigned int)(unsigned long)(&hd21_aphy[nport]->r3)//0x180B429C             
#define HDMIRX_PHY_r4_reg			(unsigned int)(unsigned long)(&hd21_aphy[nport]->r4)//0x180B42A0             
#define HDMIRX_PHY_acdr_r0_reg		(unsigned int)(unsigned long)(&hd21_aphy[nport]->acdr_r0)//0x180B42A4        
#define HDMIRX_PHY_acdr_r1_reg		(unsigned int)(unsigned long)(&hd21_aphy[nport]->acdr_r1)//0x180B42A8        
#define HDMIRX_PHY_acdr_r2_reg		(unsigned int)(unsigned long)(&hd21_aphy[nport]->acdr_r2)//0x180B42AC        
#define HDMIRX_PHY_rsvd2_reg		(unsigned int)(unsigned long)(&hd21_aphy[nport]->rsvd2)//0x180B42B0          
#define HDMIRX_PHY_ck_cmu_1_reg	(unsigned int)(unsigned long)(&hd21_aphy[nport]->ck_cmu_1)//0x180B42B4      
#define HDMIRX_PHY_ck_cmu_2_reg	(unsigned int)(unsigned long)(&hd21_aphy[nport]->ck_cmu_2)//0x180B42B8       
#define HDMIRX_PHY_ck_cmu_3_reg	(unsigned int)(unsigned long)(&hd21_aphy[nport]->ck_cmu_3)//0x180B42BC       

#else

#define HDMIRX_PHY_z0k_reg     	 	(unsigned int)(&hd21_aphy[nport]->z0k)//0x180B4204            
#define HDMIRX_PHY_z0pow_reg    	(unsigned int)(&hd21_aphy[nport]->z0pow)//0x180B4208	        
#define HDMIRX_PHY_top_in_0_reg 	(unsigned int)(&hd21_aphy[nport]->top_in_0)//0x180B4210       
#define HDMIRX_PHY_top_in_1_reg 	(unsigned int)(&hd21_aphy[nport]->top_in_1)//0x180B4214	      
#define HDMIRX_PHY_top_in_2_reg 	(unsigned int)(&hd21_aphy[nport]->top_in_2)//0x180B4218	      
#define HDMIRX_PHY_top_out_0_reg 	(unsigned int)(&hd21_aphy[nport]->top_out_0)//0x180B4220      
#define HDMIRX_PHY_top_out_1_reg 	(unsigned int)(&hd21_aphy[nport]->top_out_1)//0x180B4224       
#define HDMIRX_PHY_rsvd0_reg    		(unsigned int)(&hd21_aphy[nport]->rsvd0)//0x180B4228          
#define HDMIRX_PHY_rsvd1_reg		(unsigned int)(&hd21_aphy[nport]->rsvd1)//0x180B422C          
#define HDMIRX_PHY_ck0_reg			(unsigned int)(&hd21_aphy[nport]->ck0)//0x180B4230             
#define HDMIRX_PHY_ck1_reg			(unsigned int)(&hd21_aphy[nport]->ck1)//0x180B4234             
#define HDMIRX_PHY_ck2_reg 		(unsigned int)(&hd21_aphy[nport]->ck2)//0x180B4238             
#define HDMIRX_PHY_ck3_reg 		(unsigned int)(&hd21_aphy[nport]->ck3)//0x180B423C             
#define HDMIRX_PHY_ck4_reg			(unsigned int)(&hd21_aphy[nport]->ck4)//0x180B4240             
#define HDMIRX_PHY_acdr_ck0_reg	 (unsigned int)(&hd21_aphy[nport]->acdr_ck0)//0x180B4244       
#define HDMIRX_PHY_acdr_ck1_reg	 (unsigned int)(&hd21_aphy[nport]->acdr_ck1)//0x180B4248       
#define HDMIRX_PHY_acdr_ck2_reg 	(unsigned int)(&hd21_aphy[nport]->acdr_ck2)//0x180B424C       
#define HDMIRX_PHY_b0_reg       		(unsigned int)(&hd21_aphy[nport]->b0)//0x180B4250             
#define HDMIRX_PHY_b1_reg       		(unsigned int)(&hd21_aphy[nport]->b1)//0x180B4254             
#define HDMIRX_PHY_b2_reg       		(unsigned int)(&hd21_aphy[nport]->b2)//0x180B4258             
#define HDMIRX_PHY_b3_reg       		(unsigned int)(&hd21_aphy[nport]->b3)//0x180B425C             
#define HDMIRX_PHY_b4_reg      		 (unsigned int)(&hd21_aphy[nport]->b4)//0x180B4260             
#define HDMIRX_PHY_acdr_b0_reg  	(unsigned int)(&hd21_aphy[nport]->acdr_b0)//0x180B4264        
#define HDMIRX_PHY_acdr_b1_reg  	(unsigned int)(&hd21_aphy[nport]->acdr_b1)//0x180B4268        
#define HDMIRX_PHY_acdr_b2_reg 	 (unsigned int)(&hd21_aphy[nport]->acdr_b2)//0x180B426C        
#define HDMIRX_PHY_g0_reg       		(unsigned int)(&hd21_aphy[nport]->g0)//0x180B4270             
#define HDMIRX_PHY_g1_reg       		(unsigned int)(&hd21_aphy[nport]->g1)//0x180B4274             
#define HDMIRX_PHY_g2_reg      		(unsigned int)(&hd21_aphy[nport]->g2)//0x180B4278             
#define HDMIRX_PHY_g3_reg       		(unsigned int)(&hd21_aphy[nport]->g3)//0x180B427C             
#define HDMIRX_PHY_g4_reg			(unsigned int)(&hd21_aphy[nport]->g4)//0x180B4280             
#define HDMIRX_PHY_acdr_g0_reg  	(unsigned int)(&hd21_aphy[nport]->acdr_g0)//0x180B4284        
#define HDMIRX_PHY_acdr_g1_reg		(unsigned int)(&hd21_aphy[nport]->acdr_g1)//0x180B4288        
#define HDMIRX_PHY_acdr_g2_reg		(unsigned int)(&hd21_aphy[nport]->acdr_g2)//0x180B428C        
#define HDMIRX_PHY_r0_reg			(unsigned int)(&hd21_aphy[nport]->r0)//0x180B4290             
#define HDMIRX_PHY_r1_reg			(unsigned int)(&hd21_aphy[nport]->r1)//0x180B4294             
#define HDMIRX_PHY_r2_reg			(unsigned int)(&hd21_aphy[nport]->r2)//0x180B4298             
#define HDMIRX_PHY_r3_reg			(unsigned int)(&hd21_aphy[nport]->r3)//0x180B429C             
#define HDMIRX_PHY_r4_reg			(unsigned int)(&hd21_aphy[nport]->r4)//0x180B42A0             
#define HDMIRX_PHY_acdr_r0_reg		(unsigned int)(&hd21_aphy[nport]->acdr_r0)//0x180B42A4        
#define HDMIRX_PHY_acdr_r1_reg		(unsigned int)(&hd21_aphy[nport]->acdr_r1)//0x180B42A8        
#define HDMIRX_PHY_acdr_r2_reg		(unsigned int)(&hd21_aphy[nport]->acdr_r2)//0x180B42AC        
#define HDMIRX_PHY_rsvd2_reg		(unsigned int)(&hd21_aphy[nport]->rsvd2)//0x180B42B0          
#define HDMIRX_PHY_ck_cmu_1_reg	(unsigned int)(&hd21_aphy[nport]->ck_cmu_1)//0x180B42B4      
#define HDMIRX_PHY_ck_cmu_2_reg	(unsigned int)(&hd21_aphy[nport]->ck_cmu_2)//0x180B42B8       
#define HDMIRX_PHY_ck_cmu_3_reg	(unsigned int)(&hd21_aphy[nport]->ck_cmu_3)//0x180B42BC       

#endif
/////////////////////


#define cmu_ck_1_HYS_AMP_EN								(_BIT6)   //Hysteresis amp power
#define cmu_ck_1_HYS_WIN_SEL									(_BIT3|_BIT2|_BIT1)
#define cmu_ck_1_ENHANCE_BIAS_111							(_BIT3|_BIT1)
#define cmu_ck_1_ENHANCE_BIAS_101								(_BIT2)
#define cmu_ck_1_ENHANCE_BIAS_001								(_BIT1)
#define cmu_ck_1_ENHANCE_BIAS_000								(0)
//#define cmu_ck_1_NON_HYS_CLKDIV									(_BIT4)
#define cmu_ck_1_MD_DET_SRC										(_BIT5)
#define cmu_ck_1_CKIN_SEL											(_BIT4)
#define cmu_ck_1_NON_HYS_AMP_EN									(_BIT0)  //Non-Hysteresis Amp Power



//////////////////////
struct hdmi21_aphy_rstb_reg_st {
	uint32_t rega00;  //BAFC	
};

extern const struct hdmi21_aphy_rstb_reg_st *hd21_aphy_rstb[4];

#define HDMIRX_PHY_rega00	(unsigned int)(unsigned long)(&hd21_aphy_rstb[nport]->rega00)//BAFC	                

/////////////////////////////////////////////////

struct hdmi21_dphy_reg_st {
	
	uint32_t cdr_regd00;  //0x180B4300
	uint32_t cdr_regd01;  //0x180B4304
	uint32_t cdr_regd02;  //0x180B4308
	uint32_t cdr_regd03;  //0x180B430C
//CDR_B	
	uint32_t cdr_regd04;  //0x180B4310
//CDR_G	
	uint32_t cdr_regd05;  //0x180B4314
//CDR_R	
	uint32_t cdr_regd06;  //0x180B4318
//CDR_CK	
	uint32_t cdr_regd07;  //0x180B431C
//CDR_B		
	uint32_t cdr_regd08;  //0x180B4320
//CDR_G
	uint32_t cdr_regd09;  //0x180B4324
//CDR_R	
	uint32_t cdr_regd10;  //0x180B4328
//CDR_CK	
	uint32_t cdr_regd11;  //0x180B432C
//CDR_B	
	uint32_t cdr_regd12;  //0x180B4330
//CDR_G	
	uint32_t cdr_regd13;  //0x180B4334
//CDR_R	
	uint32_t cdr_regd14;  //0x180B4338
//CDR_CK	
	uint32_t cdr_regd15;  //0x180B433C
	
	uint32_t clk_regd00;  //0x180B4340
       //KOFF B
	uint32_t koff_regd00;  //0x180B4344
	uint32_t koff_regd01;  //0x180B4348
	uint32_t koff_regd02;  //0x180B434C
	uint32_t koff_regd03;  //0x180B4350
       //KOFF G	
	uint32_t koff_regd04;  //0x180B4354
	uint32_t koff_regd05;  //0x180B4358
	uint32_t koff_regd06;  //0x180B435C
	uint32_t koff_regd07;  //0x180B4360
       //KOFF R	
	uint32_t koff_regd08;  //0x180B4364
	uint32_t koff_regd09;  //0x180B4368
	uint32_t koff_regd10;  //0x180B436C
	uint32_t koff_regd11;  //0x180B4370
       //KOFF CK	
	uint32_t koff_regd12;  //0x180B4374
	uint32_t koff_regd13;  //0x180B4378
	uint32_t koff_regd14; //0x180B437C
	uint32_t koff_regd15; //0x180B4380
	
	uint32_t koff_regd16; //0x180B4384
	uint32_t data_regd00; //0x180B4388
	uint32_t data_regd01; //0x180B438C
	uint32_t data_regd02; //0x180B4390
	uint32_t data_regd03; //0x180B4394
	
	uint32_t data_regd04; //0x180B4398
	
	uint32_t data_regd05;  //0x180B439C
	uint32_t data_regd06;  //0x180B43A0
	uint32_t data_regd07;  //0x180B43A4
	uint32_t data_regd08;  //0x180B43A8
	uint32_t data_regd09;  //0x180B43AC
	uint32_t data_regd10;  //0x180B43B0
	uint32_t data_regd11;  //0x180B43B0
	//FLD_B
	uint32_t fld_regd00;  //0x180B43B8
	uint32_t fld_regd01;  //0x180B43BC
	uint32_t fld_regd02;  //0x180B43C0
	uint32_t fld_regd03;  //0x180B43C4
	uint32_t fld_regd04;  //0x180B43C8
	uint32_t fld_regd05;  //0x180B43CC
	uint32_t fld_regd06;  //0x180B43D0
	
	uint32_t rsvd5;  //0x180B43D4
	//FLD_G
	uint32_t fld_regd07;  //0x180B43D8
	uint32_t fld_regd08;  //0x180B43DC	
	uint32_t fld_regd09;  //0x180B43E0		
	uint32_t fld_regd10;  //0x180B43E4		
	uint32_t fld_regd11;  //0x180B43E8		
	uint32_t fld_regd12;  //0x180B43EC		
	uint32_t fld_regd13;  //0x180B43F0		
	//FLD_R	
	uint32_t fld_regd14;  //0x180B43F4
	uint32_t fld_regd15;  //0x180B43F8		
	uint32_t fld_regd16;  //0x180B43FC		
	uint32_t fld_regd17;  //0x180B4400		
	uint32_t fld_regd18;  //0x180B4404		
	uint32_t fld_regd19;  //0x180B4408		
	uint32_t fld_regd20;  //0x180B440C		
	//FLD_CK	
	uint32_t fld_regd21;  //0x180B4410		
	uint32_t fld_regd22;  //0x180B4414		
	uint32_t fld_regd23;  //0x180B4418		
	uint32_t fld_regd24;  //0x180B441C		
	uint32_t fld_regd25;  //0x180B4420		
	uint32_t fld_regd26;  //0x180B4424		
	uint32_t fld_regd27;  //0x180B4428
	
	uint32_t mod_regd00;  //0x180B442C	
	uint32_t mod_regd01;  //0x180B4430	
	uint32_t pllcdr_regd00;  //0x180B4434	
	uint32_t pllcdr_regd01;  //0x180B4438	
	//PLLCDR_B	
	uint32_t pllcdr_regd02;  //0x180B443C	
	uint32_t pllcdr_regd03;  //0x180B4440
	uint32_t pllcdr_regd04;  //0x180B4444
	//PLLCDR_G	
	uint32_t pllcdr_regd05;  //0x180B4448
	uint32_t pllcdr_regd06;  //0x180B444C
	uint32_t pllcdr_regd07;  //0x180B4450
	//PLLCDR_R	
	uint32_t pllcdr_regd08;  //0x180B4454
	uint32_t pllcdr_regd09;  //0x180B4458
	uint32_t pllcdr_regd10;  //0x180B445C
	//PLLCDR_CK	
	uint32_t pllcdr_regd11;  //0x180B4460
	uint32_t pllcdr_regd12;  //0x180B4464
	uint32_t pllcdr_regd13;  //0x180B4468
	
	uint32_t psave_regd00;  //0x180B446C
	uint32_t dfe_regd00;  //0x180B4470
	uint32_t dfe_regd01;  //0x180B4474
	uint32_t dfe_regd02;  //0x180B4478
	uint32_t misc_regd00;  //0x180B447C
	uint32_t misc_regd01;  //0x180B4480
	uint32_t misc_regd02;  //0x180B4484
};


extern const struct hdmi21_dphy_reg_st *hd21_dphy[4];


/////////////////////////////////////////////////////
#ifdef CONFIG_ARM64
#define HDMIRX_PHY_cdr_regd00				(unsigned int)(unsigned long)(&hd21_dphy[nport]->cdr_regd00)//0x180B4300         
#define HDMIRX_PHY_cdr_regd01				(unsigned int)(unsigned long)(&hd21_dphy[nport]->cdr_regd01)//0x180B4304         
#define HDMIRX_PHY_cdr_regd02				(unsigned int)(unsigned long)(&hd21_dphy[nport]->cdr_regd02)//0x180B4308         
#define HDMIRX_PHY_cdr_regd03				(unsigned int)(unsigned long)(&hd21_dphy[nport]->cdr_regd03)//0x180B430C         
#define HDMIRX_PHY_cdr_regd04				(unsigned int)(unsigned long)(&hd21_dphy[nport]->cdr_regd04)//0x180B4310         
#define HDMIRX_PHY_cdr_regd05				(unsigned int)(unsigned long)(&hd21_dphy[nport]->cdr_regd05)//0x180B4314         
#define HDMIRX_PHY_cdr_regd06				(unsigned int)(unsigned long)(&hd21_dphy[nport]->cdr_regd06)//0x180B4318         
#define HDMIRX_PHY_cdr_regd07				(unsigned int)(unsigned long)(&hd21_dphy[nport]->cdr_regd07)//0x180B431C         
#define HDMIRX_PHY_cdr_regd08				(unsigned int)(unsigned long)(&hd21_dphy[nport]->cdr_regd08)//0x180B4320         
#define HDMIRX_PHY_cdr_regd09				(unsigned int)(unsigned long)(&hd21_dphy[nport]->cdr_regd09)//0x180B4324         
#define HDMIRX_PHY_cdr_regd10				(unsigned int)(unsigned long)(&hd21_dphy[nport]->cdr_regd10)//0x180B4328         
#define HDMIRX_PHY_cdr_regd11				(unsigned int)(unsigned long)(&hd21_dphy[nport]->cdr_regd11)//0x180B432C         
#define HDMIRX_PHY_cdr_regd12				(unsigned int)(unsigned long)(&hd21_dphy[nport]->cdr_regd12)//0x180B4330         
#define HDMIRX_PHY_cdr_regd13				(unsigned int)(unsigned long)(&hd21_dphy[nport]->cdr_regd13)//0x180B4334         
#define HDMIRX_PHY_cdr_regd14				(unsigned int)(unsigned long)(&hd21_dphy[nport]->cdr_regd14)//0x180B4338         
#define HDMIRX_PHY_cdr_regd15				(unsigned int)(unsigned long)(&hd21_dphy[nport]->cdr_regd15)//0x180B433C         
#define HDMIRX_PHY_clk_regd00				(unsigned int)(unsigned long)(&hd21_dphy[nport]->clk_regd00)//0x180B4340         
//KOFF B                                                                                            
#define HDMIRX_PHY_koff_regd00				(unsigned int)(unsigned long)(&hd21_dphy[nport]->koff_regd00)//0x180B4344        
#define HDMIRX_PHY_koff_regd01				(unsigned int)(unsigned long)(&hd21_dphy[nport]->koff_regd01)//0x180B4348        
#define HDMIRX_PHY_koff_regd02				(unsigned int)(unsigned long)(&hd21_dphy[nport]->koff_regd02)//0x180B434C        
#define HDMIRX_PHY_koff_regd03				(unsigned int)(unsigned long)(&hd21_dphy[nport]->koff_regd03)//0x180B4350        
 //KOFF G                                                                                           
#define HDMIRX_PHY_koff_regd04				(unsigned int)(unsigned long)(&hd21_dphy[nport]->koff_regd04)//0x180B4354        
#define HDMIRX_PHY_koff_regd05				(unsigned int)(unsigned long)(&hd21_dphy[nport]->koff_regd05)//0x180B4358        
#define HDMIRX_PHY_koff_regd06				(unsigned int)(unsigned long)(&hd21_dphy[nport]->koff_regd06)//0x180B435C        
#define HDMIRX_PHY_koff_regd07				(unsigned int)(unsigned long)(&hd21_dphy[nport]->koff_regd07)//0x180B4360        
 //KOFF R                                                                                           
#define HDMIRX_PHY_koff_regd08				(unsigned int)(unsigned long)(&hd21_dphy[nport]->koff_regd08)//0x180B4364        
#define HDMIRX_PHY_koff_regd09				(unsigned int)(unsigned long)(&hd21_dphy[nport]->koff_regd09)//0x180B4368        
#define HDMIRX_PHY_koff_regd10				(unsigned int)(unsigned long)(&hd21_dphy[nport]->koff_regd10)//0x180B436C        
#define HDMIRX_PHY_koff_regd11				(unsigned int)(unsigned long)(&hd21_dphy[nport]->koff_regd11)//0x180B4370        
 //KOFF CK                                                                                           
#define HDMIRX_PHY_koff_regd12				(unsigned int)(unsigned long)(&hd21_dphy[nport]->koff_regd12)//0x180B4374        
#define HDMIRX_PHY_koff_regd13				(unsigned int)(unsigned long)(&hd21_dphy[nport]->koff_regd13)//0x180B4378        
#define HDMIRX_PHY_koff_regd14				(unsigned int)(unsigned long)(&hd21_dphy[nport]->koff_regd14)//0x180B437C         
#define HDMIRX_PHY_koff_regd15				(unsigned int)(unsigned long)(&hd21_dphy[nport]->koff_regd15)//0x180B4380         
                                                                                            
#define HDMIRX_PHY_koff_regd16				(unsigned int)(unsigned long)(&hd21_dphy[nport]->koff_regd16)//0x180B4384         
#define HDMIRX_PHY_data_regd00				(unsigned int)(unsigned long)(&hd21_dphy[nport]->data_regd00)//0x180B4388         
#define HDMIRX_PHY_data_regd01				(unsigned int)(unsigned long)(&hd21_dphy[nport]->data_regd01)//0x180B438C         
#define HDMIRX_PHY_data_regd02				(unsigned int)(unsigned long)(&hd21_dphy[nport]->data_regd02)//0x180B4390         
#define HDMIRX_PHY_data_regd03				(unsigned int)(unsigned long)(&hd21_dphy[nport]->data_regd03)//0x180B4394         
                                                                                            
#define HDMIRX_PHY_data_regd04				(unsigned int)(unsigned long)(&hd21_dphy[nport]->data_regd04)//0x180B4398         
                           
#define HDMIRX_PHY_data_regd05				(unsigned int)(unsigned long)(&hd21_dphy[nport]->data_regd05)//0x180B439C        
#define HDMIRX_PHY_data_regd06				(unsigned int)(unsigned long)(&hd21_dphy[nport]->data_regd06)//0x180B43A0        
#define HDMIRX_PHY_data_regd07				(unsigned int)(unsigned long)(&hd21_dphy[nport]->data_regd07)//0x180B4360        
#define HDMIRX_PHY_data_regd08				(unsigned int)(unsigned long)(&hd21_dphy[nport]->data_regd08)//0x180B43A8        
#define HDMIRX_PHY_data_regd09				(unsigned int)(unsigned long)(&hd21_dphy[nport]->data_regd09)//0x180B43AC        
#define HDMIRX_PHY_data_regd10				(unsigned int)(unsigned long)(&hd21_dphy[nport]->data_regd10)//0x180B43B0     
#define HDMIRX_PHY_data_regd11				(unsigned int)(unsigned long)(&hd21_dphy[nport]->data_regd11)//0x180B43B4     

//#define HDMIRX_PHY_rsvd4					(unsigned int)(unsigned long)(&hd21_dphy[nport]->rsvd4)//BBB4                    
 //FLD_B                                                                                            
#define HDMIRX_PHY_fld_regd00				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd00)//0x180B43B8          
#define HDMIRX_PHY_fld_regd01				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd01)//0x180B43BC          
#define HDMIRX_PHY_fld_regd02				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd02)//0x180B43C0          
#define HDMIRX_PHY_fld_regd03				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd03)//0x180B43C4          
#define HDMIRX_PHY_fld_regd04				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd04)//0x180B43C8          
#define HDMIRX_PHY_fld_regd05				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd05)//0x180B43CC          
#define HDMIRX_PHY_fld_regd06				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd06)//0x180B43D0          
                                                                                            
#define HDMIRX_PHY_rsvd5					(unsigned int)(unsigned long)(&hd21_dphy[nport]->rsvd5)//0x180B43D4                    
 //FLD_G                                                                                           
#define HDMIRX_PHY_fld_regd07				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd07)//0x180B43D8          
#define HDMIRX_PHY_fld_regd08				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd08)//0x180B43DC          
#define HDMIRX_PHY_fld_regd09				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd09)//0x180B43E0		      
#define HDMIRX_PHY_fld_regd10				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd10)//0x180B43E4		      
#define HDMIRX_PHY_fld_regd11				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd11)//0x180B43E8		      
#define HDMIRX_PHY_fld_regd12				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd12)//0x180B43EC		      
#define HDMIRX_PHY_fld_regd13				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd13)//0x180B43F0		      
 //FLD_R                                                                                            
#define HDMIRX_PHY_fld_regd14				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd14)//0x180B43F4          
#define HDMIRX_PHY_fld_regd15				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd15)//0x180B43F8		      
#define HDMIRX_PHY_fld_regd16				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd16)//0x180B43FC		      
#define HDMIRX_PHY_fld_regd17				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd17)//0x180B4400          
#define HDMIRX_PHY_fld_regd18				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd18)//0x180B4404		      
#define HDMIRX_PHY_fld_regd19				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd19)//0x180B4408		      
#define HDMIRX_PHY_fld_regd20				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd20)//0x180B440C		      
 //FLD_CK                                                                                           
#define HDMIRX_PHY_fld_regd21				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd21)//0x180B4410		      
#define HDMIRX_PHY_fld_regd22				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd22)//0x180B4414		      
#define HDMIRX_PHY_fld_regd23				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd23)//0x180B4418		      
#define HDMIRX_PHY_fld_regd24				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd24)//0x180B441C		      
#define HDMIRX_PHY_fld_regd25				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd25)//0x180B4420		      
#define HDMIRX_PHY_fld_regd26				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd26)//0x180B4424          
#define HDMIRX_PHY_fld_regd27				(unsigned int)(unsigned long)(&hd21_dphy[nport]->fld_regd27)//0x180B4428          
                                                                                            
#define HDMIRX_PHY_mod_regd00				(unsigned int)(unsigned long)(&hd21_dphy[nport]->mod_regd00)//0x180B442C	        
#define HDMIRX_PHY_mod_regd01				(unsigned int)(unsigned long)(&hd21_dphy[nport]->mod_regd01)//0x180B4430	        
#define HDMIRX_PHY_pllcdr_regd00			(unsigned int)(unsigned long)(&hd21_dphy[nport]->pllcdr_regd00)//0x180B4434	  
#define HDMIRX_PHY_pllcdr_regd01			(unsigned int)(unsigned long)(&hd21_dphy[nport]->pllcdr_regd01)//0x180B4438	  
                                                                                            
#define HDMIRX_PHY_pllcdr_regd02			(unsigned int)(unsigned long)(&hd21_dphy[nport]->pllcdr_regd02)//0x180B443C	  
#define HDMIRX_PHY_pllcdr_regd03			(unsigned int)(unsigned long)(&hd21_dphy[nport]->pllcdr_regd03)//0x180B4440    
#define HDMIRX_PHY_pllcdr_regd04			(unsigned int)(unsigned long)(&hd21_dphy[nport]->pllcdr_regd04)//0x180B4444    
                                                                                            
#define HDMIRX_PHY_pllcdr_regd05			(unsigned int)(unsigned long)(&hd21_dphy[nport]->pllcdr_regd05)//0x180B4448    
#define HDMIRX_PHY_pllcdr_regd06			(unsigned int)(unsigned long)(&hd21_dphy[nport]->pllcdr_regd06)//0x180B444C    
#define HDMIRX_PHY_pllcdr_regd07			(unsigned int)(unsigned long)(&hd21_dphy[nport]->pllcdr_regd07)//0x180B4450    
                                                                                            
#define HDMIRX_PHY_pllcdr_regd08			(unsigned int)(unsigned long)(&hd21_dphy[nport]->pllcdr_regd08)//0x180B4454    
#define HDMIRX_PHY_pllcdr_regd09			(unsigned int)(unsigned long)(&hd21_dphy[nport]->pllcdr_regd09)//0x180B4458    
#define HDMIRX_PHY_pllcdr_regd10			(unsigned int)(unsigned long)(&hd21_dphy[nport]->pllcdr_regd10)//0x180B445C    
                                                                                            
#define HDMIRX_PHY_pllcdr_regd11			(unsigned int)(unsigned long)(&hd21_dphy[nport]->pllcdr_regd11)//0x180B4460    
#define HDMIRX_PHY_pllcdr_regd12			(unsigned int)(unsigned long)(&hd21_dphy[nport]->pllcdr_regd12)//0x180B4464    
#define HDMIRX_PHY_pllcdr_regd13			(unsigned int)(unsigned long)(&hd21_dphy[nport]->pllcdr_regd13)//0x180B4468    
                                                                                            
#define HDMIRX_PHY_psave_regd00			(unsigned int)(unsigned long)(&hd21_dphy[nport]->psave_regd00)//0x180B446C      
#define HDMIRX_PHY_dfe_regd00			(unsigned int)(unsigned long)(&hd21_dphy[nport]->dfe_regd00)//0x180B4470          
#define HDMIRX_PHY_dfe_regd01			(unsigned int)(unsigned long)(&hd21_dphy[nport]->dfe_regd01)//0x180B4474          
#define HDMIRX_PHY_dfe_regd02			(unsigned int)(unsigned long)(&hd21_dphy[nport]->dfe_regd02)//0x180B4478          
#define HDMIRX_PHY_misc_regd00			(unsigned int)(unsigned long)(&hd21_dphy[nport]->misc_regd00)//0x180B447C        
#define HDMIRX_PHY_misc_regd01			(unsigned int)(unsigned long)(&hd21_dphy[nport]->misc_regd01)//0x180B4480        
#define HDMIRX_PHY_misc_regd02			(unsigned int)(unsigned long)(&hd21_dphy[nport]->misc_regd02)//0x180B4484        

#else
                                                                                            
#define HDMIRX_PHY_cdr_regd00				(unsigned int)(&hd21_dphy[nport]->cdr_regd00)//0x180B4300         
#define HDMIRX_PHY_cdr_regd01				(unsigned int)(&hd21_dphy[nport]->cdr_regd01)//0x180B4304         
#define HDMIRX_PHY_cdr_regd02				(unsigned int)(&hd21_dphy[nport]->cdr_regd02)//0x180B4308         
#define HDMIRX_PHY_cdr_regd03				(unsigned int)(&hd21_dphy[nport]->cdr_regd03)//0x180B430C         
#define HDMIRX_PHY_cdr_regd04				(unsigned int)(&hd21_dphy[nport]->cdr_regd04)//0x180B4310         
#define HDMIRX_PHY_cdr_regd05				(unsigned int)(&hd21_dphy[nport]->cdr_regd05)//0x180B4314         
#define HDMIRX_PHY_cdr_regd06				(unsigned int)(&hd21_dphy[nport]->cdr_regd06)//0x180B4318         
#define HDMIRX_PHY_cdr_regd07				(unsigned int)(&hd21_dphy[nport]->cdr_regd07)//0x180B431C         
#define HDMIRX_PHY_cdr_regd08				(unsigned int)(&hd21_dphy[nport]->cdr_regd08)//0x180B4320         
#define HDMIRX_PHY_cdr_regd09				(unsigned int)(&hd21_dphy[nport]->cdr_regd09)//0x180B4324         
#define HDMIRX_PHY_cdr_regd10				(unsigned int)(&hd21_dphy[nport]->cdr_regd10)//0x180B4328         
#define HDMIRX_PHY_cdr_regd11				(unsigned int)(&hd21_dphy[nport]->cdr_regd11)//0x180B432C         
#define HDMIRX_PHY_cdr_regd12				(unsigned int)(&hd21_dphy[nport]->cdr_regd12)//0x180B4330         
#define HDMIRX_PHY_cdr_regd13				(unsigned int)(&hd21_dphy[nport]->cdr_regd13)//0x180B4334         
#define HDMIRX_PHY_cdr_regd14				(unsigned int)(&hd21_dphy[nport]->cdr_regd14)//0x180B4338         
#define HDMIRX_PHY_cdr_regd15				(unsigned int)(&hd21_dphy[nport]->cdr_regd15)//0x180B433C         
#define HDMIRX_PHY_clk_regd00				(unsigned int)(&hd21_dphy[nport]->clk_regd00)//0x180B4340         
//KOFF B                                                                                            
#define HDMIRX_PHY_koff_regd00				(unsigned int)(&hd21_dphy[nport]->koff_regd00)//0x180B4344        
#define HDMIRX_PHY_koff_regd01				(unsigned int)(&hd21_dphy[nport]->koff_regd01)//0x180B4348        
#define HDMIRX_PHY_koff_regd02				(unsigned int)(&hd21_dphy[nport]->koff_regd02)//0x180B434C        
#define HDMIRX_PHY_koff_regd03				(unsigned int)(&hd21_dphy[nport]->koff_regd03)//0x180B4350        
 //KOFF G                                                                                           
#define HDMIRX_PHY_koff_regd04				(unsigned int)(&hd21_dphy[nport]->koff_regd04)//0x180B4354        
#define HDMIRX_PHY_koff_regd05				(unsigned int)(&hd21_dphy[nport]->koff_regd05)//0x180B4358        
#define HDMIRX_PHY_koff_regd06				(unsigned int)(&hd21_dphy[nport]->koff_regd06)//0x180B435C        
#define HDMIRX_PHY_koff_regd07				(unsigned int)(&hd21_dphy[nport]->koff_regd07)//0x180B4360        
 //KOFF R                                                                                           
#define HDMIRX_PHY_koff_regd08				(unsigned int)(&hd21_dphy[nport]->koff_regd08)//0x180B4364        
#define HDMIRX_PHY_koff_regd09				(unsigned int)(&hd21_dphy[nport]->koff_regd09)//0x180B4368        
#define HDMIRX_PHY_koff_regd10				(unsigned int)(&hd21_dphy[nport]->koff_regd10)//0x180B436C        
#define HDMIRX_PHY_koff_regd11				(unsigned int)(&hd21_dphy[nport]->koff_regd11)//0x180B4370        
 //KOFF CK                                                                                           
#define HDMIRX_PHY_koff_regd12				(unsigned int)(&hd21_dphy[nport]->koff_regd12)//0x180B4374        
#define HDMIRX_PHY_koff_regd13				(unsigned int)(&hd21_dphy[nport]->koff_regd13)//0x180B4378        
#define HDMIRX_PHY_koff_regd14				(unsigned int)(&hd21_dphy[nport]->koff_regd14)//0x180B437C         
#define HDMIRX_PHY_koff_regd15				(unsigned int)(&hd21_dphy[nport]->koff_regd15)//0x180B4380         
                                                                                            
#define HDMIRX_PHY_koff_regd16				(unsigned int)(&hd21_dphy[nport]->koff_regd16)//0x180B4384         
#define HDMIRX_PHY_data_regd00				(unsigned int)(&hd21_dphy[nport]->data_regd00)//0x180B4388         
#define HDMIRX_PHY_data_regd01				(unsigned int)(&hd21_dphy[nport]->data_regd01)//0x180B438C         
#define HDMIRX_PHY_data_regd02				(unsigned int)(&hd21_dphy[nport]->data_regd02)//0x180B4390         
#define HDMIRX_PHY_data_regd03				(unsigned int)(&hd21_dphy[nport]->data_regd03)//0x180B4394         
                                                                                            
#define HDMIRX_PHY_data_regd04				(unsigned int)(&hd21_dphy[nport]->data_regd04)//0x180B4398         
                           
#define HDMIRX_PHY_data_regd05				(unsigned int)(&hd21_dphy[nport]->data_regd05)//0x180B439C        
#define HDMIRX_PHY_data_regd06				(unsigned int)(&hd21_dphy[nport]->data_regd06)//0x180B43A0        
#define HDMIRX_PHY_data_regd07				(unsigned int)(&hd21_dphy[nport]->data_regd07)//0x180B43A4        
#define HDMIRX_PHY_data_regd08				(unsigned int)(&hd21_dphy[nport]->data_regd08)//0x180B43A8        
#define HDMIRX_PHY_data_regd09				(unsigned int)(&hd21_dphy[nport]->data_regd09)//0x180B43C        
#define HDMIRX_PHY_data_regd10				(unsigned int)(&hd21_dphy[nport]->data_regd10)//0x180B43B0        
#define HDMIRX_PHY_data_regd11				(unsigned int)(&hd21_dphy[nport]->data_regd11)//0x180B43B4        
 //FLD_B                                                                                            
#define HDMIRX_PHY_fld_regd00				(unsigned int)(&hd21_dphy[nport]->fld_regd00)//0x180B43B8          
#define HDMIRX_PHY_fld_regd01				(unsigned int)(&hd21_dphy[nport]->fld_regd01)//0x180B43BC          
#define HDMIRX_PHY_fld_regd02				(unsigned int)(&hd21_dphy[nport]->fld_regd02)//0x180B43C0          
#define HDMIRX_PHY_fld_regd03				(unsigned int)(&hd21_dphy[nport]->fld_regd03)//0x180B43C4          
#define HDMIRX_PHY_fld_regd04				(unsigned int)(&hd21_dphy[nport]->fld_regd04)//0x180B43C8          
#define HDMIRX_PHY_fld_regd05				(unsigned int)(&hd21_dphy[nport]->fld_regd05)//0x180B43CC          
#define HDMIRX_PHY_fld_regd06				(unsigned int)(&hd21_dphy[nport]->fld_regd06)//0x180B43D0          
                                                                                            
#define HDMIRX_PHY_rsvd5					(unsigned int)(&hd21_dphy[nport]->rsvd5)//0x180B43D4                    
 //FLD_G                                                                                           
#define HDMIRX_PHY_fld_regd07				(unsigned int)(&hd21_dphy[nport]->fld_regd07)//0x180B43D8          
#define HDMIRX_PHY_fld_regd08				(unsigned int)(&hd21_dphy[nport]->fld_regd08)//0x180B43DC          
#define HDMIRX_PHY_fld_regd09				(unsigned int)(&hd21_dphy[nport]->fld_regd09)//0x180B43E0		      
#define HDMIRX_PHY_fld_regd10				(unsigned int)(&hd21_dphy[nport]->fld_regd10)//0x180B43E4		      
#define HDMIRX_PHY_fld_regd11				(unsigned int)(&hd21_dphy[nport]->fld_regd11)//0x180B43E8		      
#define HDMIRX_PHY_fld_regd12				(unsigned int)(&hd21_dphy[nport]->fld_regd12)//0x180B43EC		      
#define HDMIRX_PHY_fld_regd13				(unsigned int)(&hd21_dphy[nport]->fld_regd13)//0x180B43F0		      
 //FLD_R                                                                                            
#define HDMIRX_PHY_fld_regd14				(unsigned int)(&hd21_dphy[nport]->fld_regd14)//0x180B43F4          
#define HDMIRX_PHY_fld_regd15				(unsigned int)(&hd21_dphy[nport]->fld_regd15)//0x180B43F8		      
#define HDMIRX_PHY_fld_regd16				(unsigned int)(&hd21_dphy[nport]->fld_regd16)//0x180B43FC		      
#define HDMIRX_PHY_fld_regd17				(unsigned int)(&hd21_dphy[nport]->fld_regd17)//0x180B4400          
#define HDMIRX_PHY_fld_regd18				(unsigned int)(&hd21_dphy[nport]->fld_regd18)//0x180B4404		      
#define HDMIRX_PHY_fld_regd19				(unsigned int)(&hd21_dphy[nport]->fld_regd19)//0x180B4408		      
#define HDMIRX_PHY_fld_regd20				(unsigned int)(&hd21_dphy[nport]->fld_regd20)//0x180B440C		      
 //FLD_CK                                                                                           
#define HDMIRX_PHY_fld_regd21				(unsigned int)(&hd21_dphy[nport]->fld_regd21)//0x180B4410		      
#define HDMIRX_PHY_fld_regd22				(unsigned int)(&hd21_dphy[nport]->fld_regd22)//0x180B4414		      
#define HDMIRX_PHY_fld_regd23				(unsigned int)(&hd21_dphy[nport]->fld_regd23)//0x180B4418		      
#define HDMIRX_PHY_fld_regd24				(unsigned int)(&hd21_dphy[nport]->fld_regd24)//0x180B441C		      
#define HDMIRX_PHY_fld_regd25				(unsigned int)(&hd21_dphy[nport]->fld_regd25)//0x180B4420		      
#define HDMIRX_PHY_fld_regd26				(unsigned int)(&hd21_dphy[nport]->fld_regd26)//0x180B4424          
#define HDMIRX_PHY_fld_regd27				(unsigned int)(&hd21_dphy[nport]->fld_regd27)//0x180B4428          
                                                                                            
#define HDMIRX_PHY_mod_regd00				(unsigned int)(&hd21_dphy[nport]->mod_regd00)//0x180B442C	        
#define HDMIRX_PHY_mod_regd01				(unsigned int)(&hd21_dphy[nport]->mod_regd01)//0x180B4430	        
#define HDMIRX_PHY_pllcdr_regd00			(unsigned int)(&hd21_dphy[nport]->pllcdr_regd00)//0x180B4434	  
#define HDMIRX_PHY_pllcdr_regd01			(unsigned int)(&hd21_dphy[nport]->pllcdr_regd01)//0x180B4438	  
                                                                                            
#define HDMIRX_PHY_pllcdr_regd02			(unsigned int)(&hd21_dphy[nport]->pllcdr_regd02)//0x180B443C	  
#define HDMIRX_PHY_pllcdr_regd03			(unsigned int)(&hd21_dphy[nport]->pllcdr_regd03)//0x180B4440    
#define HDMIRX_PHY_pllcdr_regd04			(unsigned int)(&hd21_dphy[nport]->pllcdr_regd04)//0x180B4444    
                                                                                            
#define HDMIRX_PHY_pllcdr_regd05			(unsigned int)(&hd21_dphy[nport]->pllcdr_regd05)//0x180B4448    
#define HDMIRX_PHY_pllcdr_regd06			(unsigned int)(&hd21_dphy[nport]->pllcdr_regd06)//0x180B444C    
#define HDMIRX_PHY_pllcdr_regd07			(unsigned int)(&hd21_dphy[nport]->pllcdr_regd07)//0x180B4450    
                                                                                            
#define HDMIRX_PHY_pllcdr_regd08			(unsigned int)(&hd21_dphy[nport]->pllcdr_regd08)//0x180B4454    
#define HDMIRX_PHY_pllcdr_regd09			(unsigned int)(&hd21_dphy[nport]->pllcdr_regd09)//0x180B4458    
#define HDMIRX_PHY_pllcdr_regd10			(unsigned int)(&hd21_dphy[nport]->pllcdr_regd10)//0x180B445C    
                                                                                            
#define HDMIRX_PHY_pllcdr_regd11			(unsigned int)(&hd21_dphy[nport]->pllcdr_regd11)//0x180B4460    
#define HDMIRX_PHY_pllcdr_regd12			(unsigned int)(&hd21_dphy[nport]->pllcdr_regd12)//0x180B4464    
#define HDMIRX_PHY_pllcdr_regd13			(unsigned int)(&hd21_dphy[nport]->pllcdr_regd13)//0x180B4468    
                                                                                            
#define HDMIRX_PHY_psave_regd00			(unsigned int)(&hd21_dphy[nport]->psave_regd00)//0x180B446C      
#define HDMIRX_PHY_dfe_regd00			(unsigned int)(&hd21_dphy[nport]->dfe_regd00)//0x180B4470          
#define HDMIRX_PHY_dfe_regd01			(unsigned int)(&hd21_dphy[nport]->dfe_regd01)//0x180B4474          
#define HDMIRX_PHY_dfe_regd02			(unsigned int)(&hd21_dphy[nport]->dfe_regd02)//0x180B4478          
#define HDMIRX_PHY_misc_regd00			(unsigned int)(&hd21_dphy[nport]->misc_regd00)//0x180B447C        
#define HDMIRX_PHY_misc_regd01			(unsigned int)(&hd21_dphy[nport]->misc_regd01)//0x180B4480        
#define HDMIRX_PHY_misc_regd02			(unsigned int)(&hd21_dphy[nport]->misc_regd02)//0x180B4484        
#endif

///
#define  RS_CAL_EN					   (_BIT21)			//Mac8p
#define  POW_LEQ					   (_BIT24)			//Mac8p

/////////////////////////

#define acdr_manual_RDY		(_BIT30)  //Mac8p
#define acdr_manual_RDY_FROM_REG	(_BIT30)  //Mac8p
#define acdr_manual_CMU_SEL	(_BIT31)  //Mac8p

#define acdr_PFD_RSTB						(_BIT0)  //Mac8p
#define acdr_DIV_IQ_RSTB					(_BIT16)  //Mac8p
#define acdr_VCO_EN					              (_BIT8)  //Mac8p

#define  HDMIRX_PHY_ADPHY_REG4_P0_reg                                               0xB80B4510
#define  HDMIRX_PHY_ADPHY_REG4_P1_reg                                               0xB80B4D10
//#define  HDMIRX_PHY_ADPHY_REG4_P2_reg                                               0xB800A510
//#define  HDMIRX_PHY_ADPHY_REG4_P3_reg                                               0xB800AD10

#define CK_LATCH						(_BIT1)
#define REG_CK_LATCH					(_BIT1)   //From Mac8p
#define reg_adphy_latch_clk_0	(_BIT31)
#define reg_adphy_latch_clk_1	(_BIT30)
#define reg_adphy_rst				(_BIT26)
#define reg_adphy_rst_mod			(_BIT25)

#define reg_adphy_clock_det_en	(_BIT18)


//Z0K
#define REG_Z0_FT_PN_SHORT_EN			(_BIT9)
#define REG_Z0_N_OFF						(_BIT7)
#define REG_Z0_P_OFF						(_BIT6)
#define REG_P0_Z0_Z0POW_FIX 				(_BIT5)

//TOP_IN_0
#define REG_TOP_IN_4_Z0_POW					(_BIT31|_BIT30|_BIT29|_BIT28)	//Z0_POW_R/Z0_POW_G/Z0_POW_B/Z0_POW_CK
#define REG_TOP_IN_3_CKDETECT_ENHANCE_I	(_BIT21)
#define REG_TOP_IN_3_REG_RSTB_CKDETECT	(_BIT20)
#define REG_TOP_IN_3_CKDETECT_ENHANCE	(_BIT19|_BIT18)
#define REG_TOP_IN_3_REG_EN_CKDETECT		(_BIT17)
#define REG_TOP_IN_3_REG_EN_CKIN			(_BIT16)

//TOP_IN_1
#define Combo_Port_CLK_Detect_Enable		(_BIT4)


#define TOP_IN_REG_IBHN_TUNE		(_BIT21|_BIT20)
//#define TOP_IN_CK_TX_3			(_BIT3)
#define TOP_IN_CK_TX_2			(_BIT2)
#define TOP_IN_CK_TX_1			(_BIT1)
#define TOP_IN_CK_TX_0			(_BIT0)


#define REG_HDMIRX_BIAS_EN (_BIT0)



#define  p0_REG_ACDR_CPCVM_EN                     (_BIT26)
#define  p0_REG_ACDR_VCO_TUNER                   (_BIT23|_BIT22)
#define  p0_REG_ACDR_VCOGAIN                       (_BIT21|_BIT20)
#define  p0_REG_ACDR_LFRS_SEL                      (_BIT15|_BIT14|_BIT13)
#define  p0_REG_ACDR_CKFLD_EN                     (_BIT5)
#define  p0_REG_ACDR_HR_PD	                   (_BIT4)
#define  p0_REG_ACDR_EN                                 (_BIT1)
#define  p0_REG_ACDR_TOP_EN                         (_BIT0)



//////////////////////////////////////// B lane //////////////////////////////////////////////////////////


#define  P0_b_4_FORE_KOFF_AUTOK                                                                    (_BIT28)
#define  P0_b_4_FORE_POW_FORE_KOFF                                                                  (_BIT24)
#define  P0_b_3_FORE_EN_FORE_KOFF                                                                    (_BIT23)
#define  P0_b_1_FORE_KOFF_RANGE                                                                    (_BIT22|_BIT21)
#define  P0_b_1_inputoff_p				       		(_BIT2)
#define  P0_b_1_inputoff_n				       		(_BIT1)
#define  P0_b_1_inputoff                                                                      		(_BIT0)

#define  P0_b_11_transition_cnt_en										     (_BIT20)
#define  P0_b_8_DEMUX                                                                                             (_BIT29|_BIT30|_BIT31)
#define  P0_b_8_PR_RATE_SEL                                                                                    (_BIT26|_BIT27|_BIT28)
#define  P0_b_8_POW_PR                                                                                            (_BIT25)
#define  P0_b_8_BY_PASS_PR                                                                                      (_BIT24)
#define  P0_b_7_TAP0_ISEL                                                                                         (_BIT21|_BIT22|_BIT23)
#define  P0_b_7_TAP0_ISEL_400uA                                                                              (_BIT22|_BIT23)
#define  P0_b_7_LEQ_ISEL                                                                                           (_BIT18|_BIT19|_BIT20)
#define  P0_b_7_LEQ_ISEL_400uA                                                                                (_BIT19|_BIT20)
#define  P0_b_7_LEQ_ISEL_200uA                                                                                (_BIT20)
#define  P0_b_7_PI_ISEL                                                                                              (_BIT24|_BIT17|_BIT16)
#define  P0_b_6_LEQ_BIT_RATE                                                                                    (_BIT14|_BIT15)
#define  P0_b_6_LEQ_BIT_RATE_HBR                                                                            (_BIT15)
#define  P0_b_6_LEQ_BIT_RATE_MBR                                                                            (_BIT14)
#define  P0_b_6_TAP0_HBR                                                                              	        (_BIT13)
#define  P0_b_6_LEQ_PASSIVE_CORNER                                                                      (_BIT12|_BIT11)
#define  P0_b_6_TAP0_LE_ISO							(_BIT8)
#define  P0_b_5_LEQ_CURRENT_ADJ                                                                            (_BIT3|_BIT4)
#define  P0_b_5_LEQ_CURRENT_ADJ_1X                                                                            (_BIT3)
#define  P0_b_5_LEQ_CURRENT_ADJ_1p25X                                                                       (_BIT4)
#define  P0_b_5_EQ_POW                                                                                             (_BIT0)


//#define P0_B_DFE_TAPEN4321                                                                              (_BIT6|_BIT5|_BIT4|_BIT3)
#define P0_b_12_PI_CURRENT														(_BIT28)
#define P0_b_12_PI_CSEL 														 (_BIT24|_BIT25)
#define P0_b_9_DFE_ADAPTION_POW_EN 								              (_BIT7)
#define P0_b_9_DFE_TAP_EN 								                           (_BIT3|_BIT4|_BIT5|_BIT6)
#define P0_b_9_DFE_TAP1_EN 								                           (_BIT3)
#define DFE_HDMI_RX_PHY_P0_DFE_ADAPTION_EN(data)							(0x00000080&((data)<<7))
#define DFE_HDMI_RX_PHY_P0_b_tap_en(data)								(0x00000078&((data)<<3))
#define P0_b_11_ACDR_RATE_SEL_HALF_RATE 								 (_BIT16)
#define P0_b_10_DFE_SUMAMP_LP_MANUALOFF								 (_BIT15)
#define P0_b_10_DFE_SUMAMP_ISEL_mask 								              (_BIT11|_BIT12|_BIT13)
#define P0_b_10_DFE_SUMAMP_ISEL_400uA 								              (_BIT13)
#define P0_b_10_DFE_PREAMP_ISEL_mask 								                    (_BIT8|_BIT9|_BIT10)
#define P0_b_10_DFE_PREAMP_ISEL_400uA 								                    (_BIT10)


//////////////////////////////////////// CK lane //////////////////////////////////////////////////////////

#define P0_LDO10V_CMU_EN					(_BIT7)

#define  p0_ck_4_SEL_SOFT_ON_OFF_TIMER 	(_BIT25|_BIT24)
#define  p0_ck_4_SEL_SOFT_ON_OFF 			(_BIT26)
#define  p0_ck_4_POW_SOFT_ON_OFF			(_BIT27)


#define  p0_ck_3_CMU_PREDIVN                                                                          (_BIT20|_BIT21|_BIT22|_BIT23)
#define  p0_ck_3_CMU_N_code                                                                          (_BIT18|_BIT19)
#define  p0_ck_3_CMU_N_code_SEL                                                                          (_BIT18)
#define  p0_ck_3_CMU_SEL_D4										 (_BIT16)
#define  p0_ck_2_CMU_CKIN_SEL                                                                   (_BIT1) //From Mac8p
#define  p0_ck_2_ACDR_CBUS_REF										(_BIT13|_BIT14)
#define  p0_ck_2_CK_MD_DET_SEL										(_BIT12)

//the same as hdmi2.1 
#define  p0_ck_2_HYS_WIN_SEL								 (_BIT11|_BIT10)
#define  p0_ck_2_ENHANCE_BIAS_11							 (_BIT11|_BIT10)
#define  p0_ck_2_ENHANCE_BIAS_10							 (_BIT11)
#define  p0_ck_2_LDO_EN										 (_BIT11) //For Ma7p
#define  p0_ck_2_ENHANCE_BIAS_01							 (_BIT10)
#define  p0_ck_2_ENHANCE_BIAS_00							 (0)

#define  p0_ck_2_CK_MD_REF_SEL                                                                   (_BIT9)
#define  p0_ck_2_NON_HYS_AMP_EN									   (_BIT8)  //CKD2S
#define  p0_ck_1_port_bias                                                                              (_BIT2)
#define  p0_ck_1_HYS_AMP_EN										    (_BIT1)

#define  DFE_HDMI_RX_PHY_P0_get_CMU_SEL_D4(data)                        ((0x00010000&(data))>>16)

#define P0_ck_11_transition_cnt_en									   (_BIT20)
#define P0_ck_8_LDO_EN                                                                                  (_BIT25)
//#define  P0_ck_8_VSEL_LDO_A_mask                                                                   (_BIT29|_BIT30|_BIT31)  merlin5 remove
//#define  P0_ck_8_CCO_BAND_SEL_mask                                                                   (_BIT26|_BIT27|_BIT28)
#define P0_ck_8_ENC_mask												(_BIT31|_BIT30|_BIT29|_BIT28)  //cco band sel
//#define  P0_ck_8_LDO_EN_mask                                                                          (_BIT25)  //merlin5 remove
#define  P0_ck_8_CMU_BPPSR_mask                                                                          (_BIT24)
#define P0_ck_8_CMU_SEL_CP_mask										   (_BIT20|_BIT21)
#define  P0_ck_7_CMU_BYPASS_PI_mask                                                              (_BIT19)
#define  P0_ck_7_CMU_PI_I_SEL_mask                                                                 (_BIT16|_BIT17|_BIT18)
//#define  P0_ck_6_CMU_BIG_KVCO_mask                                                                 (_BIT13)  //merlin5 remove
#define  P0_ck_6_CMU_EN_CAP_mask                                                                     (_BIT12)
#define  P0_ck_6_CMU_SEL_CS_mask                                                                     (_BIT8|_BIT9|_BIT10)
#define  P0_ck_5_CMU_SEL_PUMP_I_mask                                                               (_BIT4|_BIT5|_BIT6|_BIT7)
#define  P0_ck_5_CMU_SEL_PUMP_DB_mask				(_BIT3)
#define  P0_ck_5_CMU_SEL_R1_mask                                                                       (_BIT0|_BIT1|_BIT2)
//#define  DFE_HDMI_RX_PHY_P0_VSEL_LDO(data)					   (0xE0000000&((data)<<29))
#define  DFE_HDMI_RX_PHY_P0_ENC(data)                            		    (0xF0000000&((data)<<28))
#define  DFE_HDMI_RX_PHY_P0_CMU_BPPSR(data)                            (0x01000000&((data)<<24))
#define  DFE_HDMI_RX_PHY_P0_CMU_SEL_CS(data)                            (0x00000700&((data)<<8))
#define  DFE_HDMI_RX_PHY_P0_CMU_SEL_PUMP_I(data)                            (0x000000F0&((data)<<4))
#define  DFE_HDMI_RX_PHY_P0_CMU_SEL_R1(data)                            (0x00000007&((data)<<0))
#define  DFE_HDMI_RX_PHY_P0_get_CMU_BPPSR(data)                        ((0x01000000&(data))>>24)


#define  P0_ck_9_CMU_PFD_RSTB                                                                          (_BIT5)
#define  P0_ck_9_CMU_WDRST                                                                          (_BIT2)




//////////////////////////////////////// G lane //////////////////////////////////////////////////////////

#define  P0_g_4_FORE_KOFF_AUTOK                                                                    (_BIT28)
#define  P0_g_4_FORE_POW_FORE_KOFF                                                                  (_BIT24)
#define  P0_g_3_FORE_EN_FORE_KOFF                                                                    (_BIT23)
#define  P0_g_1_FORE_KOFF_RANGE                                                                    (_BIT22|_BIT21)
#define  P0_g_1_inputoff_p				       		(_BIT2)
#define  P0_g_1_inputoff_n				       		(_BIT1)
#define  P0_g_1_inputoff                                                                      		(_BIT0)



#define  P0_g_11_transition_cnt_en										     (_BIT20)
#define  P0_g_8_DEMUX                                                                                             (_BIT29|_BIT30|_BIT31)
#define  P0_g_8_PR_RATE_SEL                                                                                    (_BIT26|_BIT27|_BIT28)
#define  P0_g_8_POW_PR                                                                                            (_BIT25)
#define  P0_g_8_BY_PASS_PR                                                                                      (_BIT24)
#define  P0_g_7_TAP0_ISEL                                                                                         (_BIT21|_BIT22|_BIT23)
#define  P0_g_7_TAP0_ISEL_400uA                                                                              (_BIT22|_BIT23)
#define  P0_g_7_LEQ_ISEL                                                                                           (_BIT18|_BIT19|_BIT20)
#define  P0_g_7_LEQ_ISEL_400uA                                                                                (_BIT19|_BIT20)
#define  P0_g_7_LEQ_ISEL_200uA                                                                                (_BIT20)
#define  P0_g_7_PI_ISEL                                                                                              (_BIT24|_BIT17|_BIT16)
#define  P0_g_6_LEQ_BIT_RATE                                                                                    (_BIT14|_BIT15)
#define  P0_g_6_LEQ_BIT_RATE_HBR                                                                            (_BIT15)
#define  P0_g_6_LEQ_BIT_RATE_MBR                                                                            (_BIT14)
#define  P0_g_6_TAP0_HBR                                                                              	        (_BIT13)
#define  P0_g_6_LEQ_PASSIVE_CORNER                                                                      (_BIT12|_BIT11)
#define  P0_g_5_LEQ_CURRENT_ADJ                                                                            (_BIT3|_BIT4)
#define  P0_g_5_LEQ_CURRENT_ADJ_1X                                                                            (_BIT3)
#define  P0_g_5_LEQ_CURRENT_ADJ_1p25X                                                                       (_BIT4)
#define  P0_g_5_EQ_POW                                                                                             (_BIT0)


#define P0_G_DFE_TAPEN4321 													(_BIT6|_BIT5|_BIT4|_BIT3)
#define P0_g_12_PI_CURRENT														(_BIT28)
#define P0_g_12_PI_CSEL 														 (_BIT24|_BIT25)
#define P0_g_9_DFE_ADAPTION_POW_EN 						              (_BIT7)
#define P0_g_9_DFE_TAP_EN 						                           (_BIT3|_BIT4|_BIT5|_BIT6)
#define P0_g_9_DFE_TAP1_EN 						                           (_BIT3)
#define  DFE_HDMI_RX_PHY_P0_g_tap_en(data)                                                           (0x00000078&((data)<<3))
#define P0_g_11_ACDR_RATE_SEL_HALF_RATE 								 (_BIT16)
#define P0_g_10_DFE_SUMAMP_LP_MANUALOFF								 (_BIT15)
#define P0_g_10_DFE_SUMAMP_ISEL_mask 								              (_BIT11|_BIT12|_BIT13)
#define P0_g_10_DFE_SUMAMP_ISEL_400uA 								              (_BIT13)
#define P0_g_10_DFE_PREAMP_ISEL_mask 								                    (_BIT8|_BIT9|_BIT10)
#define P0_g_10_DFE_PREAMP_ISEL_400uA 								                    (_BIT10)

//////////////////////////////////////// R lane //////////////////////////////////////////////////////////

#define  P0_r_4_FORE_KOFF_AUTOK                                                                    (_BIT28)
#define  P0_r_4_FORE_POW_FORE_KOFF                                                                  (_BIT24)
#define  P0_r_3_FORE_EN_FORE_KOFF                                                                    (_BIT23)
#define  P0_r_1_FORE_KOFF_RANGE                                                                    (_BIT22|_BIT21)
#define  P0_r_1_inputoff                                                                      		(_BIT0)
#define  P0_r_1_inputoff_n				       		(_BIT1)
#define  P0_r_1_inputoff_p				       		(_BIT2)


#define  P0_r_11_transition_cnt_en											 (_BIT20)
#define  P0_r_8_DEMUX                                                                                             (_BIT29|_BIT30|_BIT31)
#define  P0_r_8_PR_RATE_SEL                                                                                    (_BIT26|_BIT27|_BIT28)
#define  P0_r_8_POW_PR                                                                                            (_BIT25)
#define  P0_r_8_BY_PASS_PR                                                                                      (_BIT24)
#define  P0_r_7_TAP0_ISEL                                                                                         (_BIT21|_BIT22|_BIT23)
#define  P0_r_7_TAP0_ISEL_400uA                                                                              (_BIT22|_BIT23)
#define  P0_r_7_LEQ_ISEL                                                                                           (_BIT18|_BIT19|_BIT20)
#define  P0_r_7_LEQ_ISEL_400uA                                                                                (_BIT19|_BIT20)
#define  P0_r_7_LEQ_ISEL_200uA                                                                                (_BIT20)
#define  P0_r_7_PI_ISEL                                                                                              (_BIT24|_BIT17|_BIT16)
#define  P0_r_6_LEQ_BIT_RATE                                                                                    (_BIT14|_BIT15)
#define  P0_r_6_LEQ_BIT_RATE_HBR                                                                            (_BIT15)
#define  P0_r_6_LEQ_BIT_RATE_MBR                                                                            (_BIT14)
#define  P0_r_6_TAP0_HBR                                                                              	        (_BIT13)
#define  P0_r_6_LEQ_PASSIVE_CORNER                                                                      (_BIT12|_BIT11)
#define  P0_r_5_LEQ_CURRENT_ADJ                                                                            (_BIT3|_BIT4)
#define  P0_r_5_LEQ_CURRENT_ADJ_1X                                                                            (_BIT3)
#define  P0_r_5_LEQ_CURRENT_ADJ_1p25X                                                                       (_BIT4)
#define  P0_r_5_EQ_POW                                                                                             (_BIT0)



#define  P0_R_DFE_TAPEN4321                                                                             (_BIT6|_BIT5|_BIT4|_BIT3)
#define P0_r_12_PI_CURRENT														(_BIT28)
#define  P0_r_12_PI_CSEL 														      (_BIT24|_BIT25)
#define P0_r_9_DFE_ADAPTION_POW_EN 								              (_BIT7)
#define P0_r_9_DFE_TAP_EN 								                           (_BIT3|_BIT4|_BIT5|_BIT6)
#define P0_r_9_DFE_TAP1_EN 								                           (_BIT3)
#define  DFE_HDMI_RX_PHY_P0_r_tap_en(data)                                                           (0x00000078&((data)<<3))
#define  P0_r_11_ACDR_RATE_SEL 												(_BIT24|_BIT25)
#define P0_r_11_ACDR_RATE_SEL_HALF_RATE 								             (_BIT16)
#define P0_r_10_DFE_SUMAMP_LP_MANUALOFF								 (_BIT15)
#define P0_r_10_DFE_SUMAMP_ISEL_mask 								              (_BIT11|_BIT12|_BIT13)
#define P0_r_10_DFE_SUMAMP_ISEL_400uA 								              (_BIT13)
#define P0_r_10_DFE_PREAMP_ISEL_mask 								                    (_BIT8|_BIT9|_BIT10)
#define P0_r_10_DFE_PREAMP_ISEL_400uA 								                    (_BIT10)




#define  P0_b_1_FORE_KOFF_ADJR			(_BIT16|_BIT17|_BIT18|_BIT19|_BIT20)
#define  P0_b_2_LEQ_BIT_RATE_mask			(_BIT8|_BIT9)
#define  P0_b_2_LEQ_BIT_RATE_MBR			(_BIT8)
#define  P0_b_2_LEQ_BIT_RATE_HBR			(_BIT9)
#define  P1_b_2_LEQ_BIT_RATE_mask			(_BIT8|_BIT9)
#define  P1_b_2_LEQ_BIT_RATE_MBR			(_BIT8)
#define  P1_b_2_LEQ_BIT_RATE_HBR			(_BIT9)
#define  P2_b_2_LEQ_BIT_RATE_mask			(_BIT8|_BIT9)
#define  P2_b_2_LEQ_BIT_RATE_MBR			(_BIT8)
#define  P2_b_2_LEQ_BIT_RATE_HBR			(_BIT9)


#define  P0_g_1_FORE_KOFF_ADJR			(_BIT16|_BIT17|_BIT18|_BIT19|_BIT20)
#define  P0_g_2_LEQ_BIT_RATE_mask			(_BIT8|_BIT9)
#define  P0_g_2_LEQ_BIT_RATE_MBR			(_BIT8)
#define  P0_g_2_LEQ_BIT_RATE_HBR			(_BIT9)
#define  P1_g_2_LEQ_BIT_RATE_mask			(_BIT8|_BIT9)
#define  P1_g_2_LEQ_BIT_RATE_MBR			(_BIT8)
#define  P1_g_2_LEQ_BIT_RATE_HBR			(_BIT9)
#define  P2_g_2_LEQ_BIT_RATE_mask			(_BIT8|_BIT9)
#define  P2_g_2_LEQ_BIT_RATE_MBR			(_BIT8)
#define  P2_g_2_LEQ_BIT_RATE_HBR			(_BIT9)


#define  P0_r_1_FORE_KOFF_ADJR			(_BIT16|_BIT17|_BIT18|_BIT19|_BIT20)
#define  P0_r_2_LEQ_BIT_RATE_mask			(_BIT8|_BIT9)
#define  P0_r_2_LEQ_BIT_RATE_MBR			(_BIT8)
#define  P0_r_2_LEQ_BIT_RATE_HBR			(_BIT9)
#define  P1_r_2_LEQ_BIT_RATE_mask			(_BIT8|_BIT9)
#define  P1_r_2_LEQ_BIT_RATE_MBR			(_BIT8)
#define  P1_r_2_LEQ_BIT_RATE_HBR			(_BIT9)
#define  P2_r_2_LEQ_BIT_RATE_mask			(_BIT8|_BIT9)
#define  P2_r_2_LEQ_BIT_RATE_MBR			(_BIT8)
#define  P2_r_2_LEQ_BIT_RATE_HBR			(_BIT9)


#define PIX2_RATE_SEL  (_BIT26)
#define PIX2_RATE_DIV5  (_BIT26)
#define PIX2_RATE_DIV10  (0)





#define  P1_b_8_BY_PASS_PR                                                                                      (_BIT24)
#define  P1_g_8_BY_PASS_PR                                                                                      (_BIT24)
#define  P1_r_8_BY_PASS_PR                                                                                      (_BIT24)
#define  P2_b_8_BY_PASS_PR                                                                                      (_BIT24)
#define  P2_g_8_BY_PASS_PR                                                                                      (_BIT24)
#define  P2_r_8_BY_PASS_PR                                                                                      (_BIT24)




