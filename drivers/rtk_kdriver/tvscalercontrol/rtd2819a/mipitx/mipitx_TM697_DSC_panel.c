#include <tvscalercontrol/mipitx/mipitx.h>
#include <tvscalercontrol/mipitx/mipitx_dphy.h>
#include <tvscalercontrol/mipitx/mipitx_dsi.h>
#include <tvscalercontrol/dsce/dsce2.h>
#include <tvscalercontrol/io/ioregdrv.h>
#include <rtk_kdriver/io.h>

UINT8 tm697_pps[128] = {
    0x11,0x00,0x00,0x89,0x10,0x80,0x07,0x80,0x04,0x38,0x00,0x14,0x02,0x1C,0x02,0x1C,
    0x00,0xAA,0x02,0x0E,0x00,0x20,0x00,0x71,0x00,0x07,0x00,0x0C,0x05,0x0E,0x05,0x16,
    0x18,0x00,0x1B,0xA0,0x03,0x0c,0x20,0x00,0x06,0x0b,0x0b,0x33,0x0e,0x1c,0x2a,0x38,
    0x46,0x54,0x62,0x69,0x70,0x77,0x79,0x7b,0x7d,0x7e,0x01,0x02,0x01,0x00,0x09,0x40,
    0x09,0xbe,0x19,0xfc,0x19,0xfa,0x19,0xf8,0x1a,0x38,0x1a,0x78,0x1a,0xb6,0x2a,0xf6,
    0x2b,0x34,0x2b,0x74,0x3b,0x74,0x6b,0xf4,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

void mipitx_TM697_DSC_panel_config(void)
{   // 1k2k 120hz dsc panel 294M
    default_mipitx_parameter.is_cmd_panel = 0;
    default_mipitx_parameter.lane_cnt = SINGLE_PANEL_FOUR_LANE;
    default_mipitx_parameter.color_depth = DSC_MODE;
    default_mipitx_parameter.color_bits = RGB_24BITS;
    default_mipitx_parameter.pixelclk = 294000;
    default_mipitx_parameter.hwidth = 1080;
    default_mipitx_parameter.vlength = 1920;
    default_mipitx_parameter.htotal = 1260;
    default_mipitx_parameter.vtotal = 1944;
    default_mipitx_parameter.hsyncwidth = 18;
    default_mipitx_parameter.vsyncwidth = 2;
    default_mipitx_parameter.hfrontporch = 90;
    default_mipitx_parameter.hbackporch = 72;
    default_mipitx_parameter.vfrontporch = 14;
    default_mipitx_parameter.vbackporch = 8;
    default_mipitx_parameter.hpolarity = 1;
    default_mipitx_parameter.vpolarity = 1;
    default_mipitx_parameter.dsc_en = 1;
    default_mipitx_parameter.dsc_pps = tm697_pps;
}

void mipitx_TM697_DSC_panel_cmd_init(void)
{
#ifndef MIPITX_BRINGUP_SCRIPT
//~~~~~~~~~TM697  DSC~~~~~~~//
//*****************CMD1*************//

UINT8 dcs0[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFF,0x10};

UINT8 dcs1[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFB,0x01};
UINT8 dcs2[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x36,0x00};
UINT8 dcs02[] = {MIPITX_DSI_GENERIC_LONG_WRITE,0xC1,0x89,0x08,0x00,0x14,0x00,0xAA,0x02,0x0E,0x00,0x71,0x00,0x07,0x05,0x0E,0x05,0x16};//20191024 PPS table setting
UINT8 dcs3[] = {MIPITX_DSI_GENERIC_LONG_WRITE,0x3B,0x03,0x14,0x36,0x04,0x04}; //0X214}; //VBP//0X236}; //VFP 0X204};  0X204}; 

UINT8 dcs4[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xB0,0x00}; //MIPI CRC disabled

UINT8 dcs5[] = {MIPITX_DSI_GENERIC_LONG_WRITE,0xC2,0x1B,0xA0}; //20191024 PPS table setting 


//*****************CMD2 Page E***************************************************************************************//

UINT8 dcs6[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFF,0xE0}; 
UINT8 dcs7[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFB,0x01}; 
UINT8 dcs8[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x1F,0xC0}; 

//*****************CMD2 Page 0***************************************************************************************//

UINT8 dcs9[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFF,0x20}; 
UINT8 dcs10[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFB,0x01}; 
UINT8 dcs11[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x30,0x1D}; 
//***********cl ID******
UINT8 dcs12[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x44,0x00}; 
UINT8 dcs13[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x45,0x00}; 
UINT8 dcs14[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x46,0x93}; 
//***********CL ID******	
UINT8 dcs15[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x01,0x66}; //20191024 VGH/ VGL Step-up Frequency setting

//UINT8 dcs16[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x06,0x64};  // 3C VGH =8V      64 VGH =10V       
UINT8 dcs17[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x06,0x64};  // 3C VGH =8V      64 VGH =10V     
UINT8 dcs18[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x07,0x28};  // VGL =-7V	

UINT8 dcs19[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x1B,0x00};    // 4PWR mode VDD setting   
//UINT8 dcs20[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x1B,0x01};    // 4PWR mode VDD setting
UINT8 dcs21[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x5C,0x70};    // notch area setting
UINT8 dcs22[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x5E,0xCA};    // notch area setting
UINT8 dcs23[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x69,0xD0};    // chopper setting
UINT8 dcs24[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x89,0x3A}; 
UINT8 dcs25[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x8A,0x3A}; 

UINT8 dcs26[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x95,0xA9};    //GVDDP=4.6V
//	UINT8 dcs27[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x95,0xC9};    //GVDDP=4.6V
UINT8 dcs28[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x96,0xA9};    //GVDDN=4.6V
//	UINT8 dcs29[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x96,0xC9};    //GVDDP=4.6V

UINT8 dcs29[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xF2,0x65};    //TP term charge pump setting
UINT8 dcs30[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xF3,0x44};    //TP term charge pump setting
UINT8 dcs31[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xF4,0x65};    //TP term charge pump setting
UINT8 dcs32[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xF5,0x44};    //TP term charge pump setting
UINT8 dcs33[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xF6,0x65};    //TP term charge pump setting
UINT8 dcs34[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xF7,0x44};    //TP term charge pump setting
UINT8 dcs35[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xF8,0x65};    //TP term charge pump setting
UINT8 dcs36[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xF9,0x44};    //TP term charge pump setting


//*****************CMD2 Page 3 PWM************************************************************//	
UINT8 dcs37[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0XFF,0X23}; 	
UINT8 dcs38[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0XFB,0X01}; 
UINT8 dcs39[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0X07,0X20}; 	
UINT8 dcs40[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0X08,0X07}; 	
UINT8 dcs41[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0X09,0X04}; 	
//*****************CMD2 Page 3 PWM************************************************************//

//*****************CMD2 Page 4***************************************************************************************//
UINT8 dcs42[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFF,0x24}; 
UINT8 dcs43[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFB,0x01}; 
UINT8 dcs44[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x00,0x20};  //DUMMY     (CGOUTL[1])
UINT8 dcs45[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x01,0x20};  //STV       (CGOUTL[2])
UINT8 dcs46[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x02,0x20};  //CKV2_L    (CGOUTL[3])
UINT8 dcs47[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x03,0x20};  //CKV4_L    (CGOUTL[4])
UINT8 dcs48[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x04,0x20};  //CKV1_L    (CGOUTL[5])
UINT8 dcs49[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x05,0x20};  //CKV3_L    (CGOUTL[6])
UINT8 dcs50[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x06,0x13};  //D2U       (CGOUTL[7])
UINT8 dcs51[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x07,0x15};  //U2D       (CGOUTL[8])
UINT8 dcs52[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x08,0x17};  //DUMMY     (CGOUTL[9])
UINT8 dcs53[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x09,0x13};  //GAS       (CGOUTL[10])
UINT8 dcs54[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x0A,0x15};  //GRESET    (CGOUTL[11])	
UINT8 dcs55[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x0B,0x17};  //VTCOMSW1  (CGOUTL[12])
UINT8 dcs56[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x0C,0x24};  //CKHB      (CGOUTL[13])
UINT8 dcs57[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x0D,0x01};  //CKHG      (CGOUTL[14])
UINT8 dcs58[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x0E,0x2F};  //CKHR      (CGOUTL[15])
UINT8 dcs59[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x0F,0x2D};  //CKHB      (CGOUTL[16])
UINT8 dcs60[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x10,0x2E};  //CKHG      (CGOUTL[17])
UINT8 dcs61[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x11,0x2C};  //CKHR      (CGOUTL[18])
UINT8 dcs62[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x12,0x8B};  //DUMMY     (CGOUTL[19])
UINT8 dcs63[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x13,0x8C};  //DE        (CGOUTL[20])
UINT8 dcs64[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x14,0x20};  //DO        (CGOUTL[21])
UINT8 dcs65[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x15,0x20};  //VTSW      (CGOUTL[22])
UINT8 dcs66[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x16,0x0F};  //DUMMY     (CGOUTL[23])
UINT8 dcs67[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x17,0x22};  //DUMMY     (CGOUTL[24]) 

UINT8 dcs68[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x18,0x20};  //DUMMY     (CGOUTR[1])
UINT8 dcs69[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x19,0x20}; 	//STV       (CGOUTR[2])
UINT8 dcs70[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x1A,0x20};  //CKV2_R    (CGOUTR[3])
UINT8 dcs71[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x1B,0x20};  //CKV4_R    (CGOUTR[4])
UINT8 dcs72[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x1C,0x20};  //CKV1_R    (CGOUTR[5])
UINT8 dcs73[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x1D,0x20};  //CKV3_R    (CGOUTR[6])
UINT8 dcs74[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x1E,0x13};  //D2U       (CGOUTR[7])
UINT8 dcs75[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x1F,0x15};  //U2D       (CGOUTR[8])
UINT8 dcs76[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x20,0x17};  //DUMMY     (CGOUTR[9])
UINT8 dcs77[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x21,0x13};  //GAS       (CGOUTR[10])
UINT8 dcs78[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x22,0x15};  //GRESET    (CGOUTR[11])
UINT8 dcs79[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x23,0x17};  //VTCOMSW1  (CGOUTR[12])
UINT8 dcs80[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x24,0x24};  //CKHB      (CGOUTR[13])
UINT8 dcs81[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x25,0x01};  //CKHG      (CGOUTR[14])
UINT8 dcs82[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x26,0x2F};  //CKHR      (CGOUTR[15])
UINT8 dcs83[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x27,0x2D};  //CKHB      (CGOUTR[16])
UINT8 dcs84[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x28,0x2E};  //CKHG      (CGOUTR[17])
UINT8 dcs85[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x29,0x2C};  //CKHR      (CGOUTR[18])
UINT8 dcs86[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x2A,0x8B};  //DUMMY     (CGOUTR[19])
UINT8 dcs87[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x2B,0x8C};  //DE        (CGOUTR[20])
UINT8 dcs88[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x2D,0x20};  //DO        (CGOUTR[21])
UINT8 dcs89[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x2F,0x20};  //VTSW      (CGOUTR[22])
UINT8 dcs90[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x30,0x0F};  //DUMMY     (CGOUTR[23])
UINT8 dcs91[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x31,0x22};  //DUMMY     (CGOUTR[24])       


UINT8 dcs92[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x32,0x00};  //STVR/STVL sequence setting
UINT8 dcs93[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x33,0x03};  //STV rising position by line                         
UINT8 dcs94[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x34,0x01};  //STV falling position  by line
UINT8 dcs95[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x35,0x1F};  //STV start position by clk of Table A            
UINT8 dcs96[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x36,0x2A};  //STV end position by clk of Table A   
//UINT8 dcs97[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x35,0x10};  //STV start position by clk of Table A            
//UINT8 dcs98[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x36,0x1A};  //STV end position by clk of Table A  
UINT8 dcs99[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x4D,0x06};  //CKV start position of Table A  02              
UINT8 dcs100[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x4E,0x46};  //normal line CKV end position of Table A  3c  
UINT8 dcs101[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x4F,0x46};  //line N+1 CKV end position of Table A     3c
UINT8 dcs102[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x53,0x46};  //line N CKV End position                  3c
//UINT8 dcs103[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x4D,0x01};  //CKV start position of Table A  02              
//UINT8 dcs104[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x4E,0x4A};  //normal line CKV end position of Table A  3c  
//UINT8 dcs105[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x4F,0x4A};  //line N+1 CKV end position of Table A     3c
//UINT8 dcs106[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x53,0x4A};  //line N CKV End position                  3c
UINT8 dcs107[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x79,0x03}; 	//MUX dummy line in VFP&VBP porch
UINT8 dcs108[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x7A,0x86}; 	//MUX rising position of Table A
UINT8 dcs109[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x7A,0x86}; 	//MUX rising position of Table A
UINT8 dcs110[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x7B,0x92}; //normal line MUX width of table A    8E 

UINT8 dcs111[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x7D,0x06};  //normal line MUXG1 setting 
UINT8 dcs112[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x80,0x06};  //Source output hold time                
UINT8 dcs113[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x81,0x06};  //Source signal delay time
//UINT8 dcs114[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x7D,0x05};  //normal line MUXG1 setting 
//UINT8 dcs115[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x80,0x05};  //Source output hold time                
//UINT8 dcs116[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x81,0x05};  //Source signal delay time

//*****************RGBBGR****************
UINT8 dcs117[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x82,0x13};  //MUX sequence
UINT8 dcs118[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x84,0x31};  //MUX sequence
UINT8 dcs119[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x85,0x00};  //MUX sequence
UINT8 dcs120[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x86,0x00};  //MUX sequence
UINT8 dcs121[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x87,0x00};  //MUX sequence

UINT8 dcs122[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x90,0x13};  // Source RGB sequence
UINT8 dcs123[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x92,0x31};  // Source RGB sequence
UINT8 dcs124[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x93,0x00};  // Source RGB sequence
UINT8 dcs125[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x94,0x00};  // Source RGB sequence
UINT8 dcs126[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x95,0x00};  // Source RGB sequence
UINT8 dcs127[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x9C,0xF4};  // RGBBGR setting
UINT8 dcs128[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x9D,0x01};  // RGBBGR setting

//*****************RGBBGR****************
UINT8 dcs129[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xA0,0x12};  // line n MUX width of table A
UINT8 dcs130[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xA2,0x12};  // line n+1 MUX width of table A
UINT8 dcs131[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xA3,0x06};  // line n MUX start position of table A

UINT8 dcs132[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xA4,0x06};  // line n MUXG1 setting
UINT8 dcs133[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xA5,0x06};  // line n+1 MUXG1 setting
//UINT8 dcs134[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xA4,0x05};  // line n MUXG1 setting
//UINT8 dcs135[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xA5,0x05};  // line n+1 MUXG1 setting

UINT8 dcs136[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xC4,0x80};  // slew-rate setting
UINT8 dcs137[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xC6,0xC0};  // slew-rate setting
UINT8 dcs138[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xC9,0x00};  // disable internal no use IP setting
UINT8 dcs139[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xD1,0x30};  // up/down & left/right setting
//UINT8 dcs140[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xD9,0x80};  // RGBBGR setting
UINT8 dcs141[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xD9,0x80};  // RGBBGR setting
UINT8 dcs142[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xE9,0x06};  // Line n+1 MUX rising position

//*****************CMD2 Page 5***************************************************************************************//
UINT8 dcs143[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFF,0x25}; 
UINT8 dcs144[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFB,0x01}; 
UINT8 dcs145[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x0F,0x1B};   
UINT8 dcs146[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x18,0x20};   //table select
UINT8 dcs147[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x19,0xE4};   //frame rate mapping
UINT8 dcs148[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x20,0x01};   //frame rate mapping
UINT8 dcs149[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x21,0x00};   //frame rate mapping   
UINT8 dcs150[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x66,0x40}; 	 //U2D/D2U power on setting             
UINT8 dcs151[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x67,0x29}; 	 //20190926 D2U power off setting                            
UINT8 dcs152[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x68,0x50};   //U2D/D2U power off setting
UINT8 dcs153[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x69,0x60};   //MUX power off setting
UINT8 dcs154[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x6B,0x00};   //MUX start active position during power on sequence
UINT8 dcs155[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x71,0x6D}; 	 //STV power on/off setting                                       
UINT8 dcs156[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x77,0x60};   //sleep-in setting
UINT8 dcs157[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x79,0x7A};   //RTN setting during power off sequence 
UINT8 dcs158[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x7D,0x40}; 	 //CKV power on stting                                       
UINT8 dcs159[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x7E,0x2D}; 	 //CKV power on/off setting                                      
//UINT8 dcs160[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x81,0x04}; 	                                      
UINT8 dcs161[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x84,0x70}; 	 //GAS power on setting
	 
//UINT8 dcs162[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x85,0x15}; 	                                      
//UINT8 dcs163[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x8E,0x10}; 	                                      
//UINT8 dcs164[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x8D,0x04}; 	  //20190926      GAS power off setting                      
UINT8 dcs165[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xC0,0x4D}; 	 //GRESET power on setting                                       
UINT8 dcs166[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xC1,0xA9}; 	 //GRESET power off setting                                       
UINT8 dcs167[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xC2,0xD2}; 	 //VCOMSW power on/off setting                                       
UINT8 dcs168[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xC4,0x11};   //GRESET power on setting
UINT8 dcs169[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xC6,0x11};   //VCOMSW power on setting

UINT8 dcs170[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xD6,0x80}; 	// MUX EQ setting
UINT8 dcs171[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xD7,0x02}; 	// MUX EQ setting
UINT8 dcs172[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xDA,0x02}; 	// Falling EQ for positive polarity                              DA ????
UINT8 dcs173[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xDD,0x02}; 	// MUX EQ setting

UINT8 dcs174[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xE0,0x02}; 	// Falling EQ for negative polarity                              E0
//UINT8 dcs175[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xEF,0x00}; 	                                      
UINT8 dcs176[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xF0,0x00}; 	 //APO setting                                       
UINT8 dcs177[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xF1,0x04};   //APO setting



//*****************CMD2 Page 6*************************************************************************************//
UINT8 dcs178[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFF,0x26}; 
UINT8 dcs179[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFB,0x01}; 
UINT8 dcs180[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x00,0x10}; 	 //GRESET pulse width setting during TP term bit[11:8] of Table A 

UINT8 dcs181[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x01,0xEE}; 	 //GRESET pulse width setting during TP term bit[7:0] of Table A 

UINT8 dcs182[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x03,0x00};   //VCOMSW pulse width setting during TP term bit[11:8] of Table A 

UINT8 dcs183[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x04,0xEE};   //VCOMSW pulse width setting during TP term bit[7:0] of Table A 
UINT8 dcs184[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x05,0x08};  
UINT8 dcs185[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x06,0x14}; 	 //GRESET shift setting during TP term of Table A

UINT8 dcs186[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x08,0x14};   //VCOMSW shift setting during TP term of Table A

UINT8 dcs187[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x14,0x02};   //GIP setting during V-porch LHB

UINT8 dcs188[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x15,0x01}; 	 //MUX timing during V-porch LHB

UINT8 dcs189[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x74,0xAF};   //MUX setting during TP term of Table A

UINT8 dcs190[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x81,0x14}; 	 //TP ter timing setting                                    
UINT8 dcs191[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x83,0x03}; 	 //TP ter timing setting
UINT8 dcs192[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x84,0x04}; 	 //TP ter timing setting	                                     
UINT8 dcs193[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x85,0x01}; 	 //TP ter timing setting
UINT8 dcs194[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x86,0x04}; 	 //TP ter timing setting	                                     
UINT8 dcs195[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x87,0x01}; 	 //TP ter timing setting
UINT8 dcs196[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x88,0x02}; 	 //TP ter timing setting	                                     
UINT8 dcs197[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x8A,0x1A}; 	 //rescan line RGBBGR setting                                    
UINT8 dcs198[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x8B,0x11};   //rescan line source setting
UINT8 dcs199[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x8C,0x24}; 	 //rescan line source RGB sequence                                    
UINT8 dcs200[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x8E,0x42}; 	 //rescan line source RGB sequence	                                     
UINT8 dcs201[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x8F,0x11}; 	 //rescan line source RGB sequence
UINT8 dcs202[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x90,0x11}; 	 //rescan line source RGB sequence
UINT8 dcs203[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x91,0x11}; 	 //rescan line source RGB sequence
UINT8 dcs204[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x9A,0x80};   //TP ter timing setting
UINT8 dcs205[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x9B,0x04};   //TP ter timing setting
UINT8 dcs206[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x9C,0x00};   //TP ter timing setting
UINT8 dcs207[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x9D,0x00};   //TP ter timing setting
UINT8 dcs208[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x9E,0x00};   //TP ter timing setting


//*****************CMD2 Page 7*************************************************************************************//
UINT8 dcs209[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFF,0x27}; 
UINT8 dcs210[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFB,0x01}; 

UINT8 dcs211[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x00,0x74}; 
UINT8 dcs212[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x01,0x80};   //vertical resolution	                                                                                                             
UINT8 dcs213[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x20,0x81};   //OSC1 video trim setting bit[14:8] of Tabe A 	                                        
UINT8 dcs214[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x21,0xC8}; 	 //OSC1 video trim setting bit[7:0] of Tabe A                                        
UINT8 dcs215[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x25,0x81};   //OSC2 video trim setting bit[14:8] of Tabe A OSC Trim bit7 1:enable 0:disable       
UINT8 dcs216[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x26,0xFA}; 	 //OSC2 video trim setting bit[7:0] of Tabe A                                        
UINT8 dcs217[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x6E,0x23};   //CKV sequence setting
UINT8 dcs218[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x6F,0x01};   //CKV sequence setting
UINT8 dcs219[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x70,0x00};   //CKV sequence setting
UINT8 dcs220[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x71,0x00};   //CKV sequence setting
UINT8 dcs221[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x72,0x00};   //CKV sequence setting
UINT8 dcs222[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x73,0x21};   //CKV sequence setting	                                      
UINT8 dcs223[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x74,0x03};   //CKV sequence setting	                                      
UINT8 dcs224[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x75,0x00};   //CKV sequence setting
UINT8 dcs225[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x76,0x00};   //CKV sequence setting
UINT8 dcs226[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x77,0x00};   //CKV sequence setting
UINT8 dcs227[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x7D,0x07};   //CKV sequence setting
UINT8 dcs228[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x7E,0x83};   //CKV sequence setting
UINT8 dcs229[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x80,0x23};   //CKV sequence setting
UINT8 dcs230[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x82,0x07};   //CKV sequence setting
UINT8 dcs231[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x83,0x83};   //CKV sequence setting
UINT8 dcs232[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x88,0x03};   //STV BW rsing setting
UINT8 dcs233[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x89,0x01}; 
UINT8 dcs234[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xB7,0x04};   //STV power on setting

UINT8 dcs235[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xE3,0x03};   //OSC1 video trim setting bit[14:8] of Tabe B 
UINT8 dcs236[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xE4,0x91};   //OSC1 video trim setting bit[7:0] of Tabe B 
UINT8 dcs237[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xE5,0x00};   //OSC1 video trim setting bit[14:8] of Tabe C
UINT8 dcs238[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xE6,0x7B};   //OSC1 video trim setting bit[7:0] of Tabe C
UINT8 dcs239[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xE9,0x03};   //OSC2 video trim setting bit[14:8] of Tabe B 
UINT8 dcs240[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xEA,0xF3};    //OSC2 video trim setting bit[7:0] of Tabe B 
UINT8 dcs241[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xEB,0x00};   //OSC2 video trim setting bit[14:8] of Tabe C
UINT8 dcs242[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xEC,0x7B};    //OSC2 video trim setting bit[7:0] of Tabe C

//*****************CMD2 Page A***********************************************************************************//
UINT8 dcs243[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFF,0x2A}; 
UINT8 dcs244[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFB,0x01}; 

UINT8 dcs245[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0X00,0X91}; 	  //20190926  table switch setting
UINT8 dcs246[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0X03,0X20}; 	  //20190926  table switch setting
//UINT8 dcs247[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x06,0x07}; 
UINT8 dcs248[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0X07,0X28}; 	  
UINT8 dcs249[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x0A,0x60};   //Internal TP signal setting of Table A
UINT8 dcs250[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x0C,0x04}; 	 //TP unit number of Table A   
UINT8 dcs251[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x0D,0x40}; 	
UINT8 dcs252[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x0E,0x01};   //Internal TP setting of Table A
UINT8 dcs253[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x0F,0x01}; 	 //Line number for a unit bit[11:8] of Table A
	
UINT8 dcs254[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x11,0x80}; 	 //Line number for a unit bit[7:0] of Table A                                      	
UINT8 dcs255[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x15,0x0E};   //Internal TP H-sync #1 setting bit[15:8] of Table A
UINT8 dcs256[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x16,0xE6};   //Internal TP H-sync #1 setting bit[7:0] of Table A
UINT8 dcs257[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x19,0x0E};   //Internal TP H-sync #2 setting bit[15:8] of Table A
UINT8 dcs258[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x1A,0xBA};   //Internal TP H-sync #2 setting bit[7:0] of Table A

UINT8 dcs259[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x1B,0x14}; 	                                    
UINT8 dcs260[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x1D,0x36}; 	                                    
UINT8 dcs261[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x1E,0x4F}; //RTNA
UINT8 dcs262[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x1F,0x4F}; //RTNA_LINE
UINT8 dcs263[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x20,0x4F}; //RTNA_LINE+1
UINT8 dcs264[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x27,0x80};   //V-porch sensing setting of Table A
UINT8 dcs265[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x28,0xFD};   //V-porch sensing setting of Table A 
UINT8 dcs266[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x29,0x08}; 
UINT8 dcs267[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x2A,0x01}; 
UINT8 dcs268[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x2B,0x00}; 
UINT8 dcs269[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x2D,0x08}; 
UINT8 dcs270[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x2F,0x01}; 	
UINT8 dcs271[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x30,0x45};   //V-porch sensing setting of Table A
UINT8 dcs272[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x31,0x35};   //V-porch sensing setting of Table A
UINT8 dcs273[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x33,0x11};  
UINT8 dcs274[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x34,0xFF};   //V-porch sensing setting of Table A
UINT8 dcs275[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x35,0x2A};   //V-porch sensing setting of Table A
UINT8 dcs276[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x36,0x73};   //V-porch sensing setting of Table A
UINT8 dcs277[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x37,0xFB};   //V-porch sensing setting of Table A
UINT8 dcs278[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x38,0x2D};   //V-porch sensing setting of Table A
UINT8 dcs279[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x39,0x70};   //V-porch sensing setting of Table A
UINT8 dcs280[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x3A,0x45};   //V-porch sensing setting of Table A
UINT8 dcs281[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x45,0x09}; 
UINT8 dcs282[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x46,0x40}; 
UINT8 dcs283[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x47,0x02};  	
UINT8 dcs284[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x48,0x00};  
UINT8 dcs285[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x4A,0xC0};  
UINT8 dcs286[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x4E,0x0E};  
UINT8 dcs287[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x4F,0xE6};  
UINT8 dcs288[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x52,0x0E};  
UINT8 dcs289[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x53,0xBA};  
UINT8 dcs290[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x54,0x14};  
UINT8 dcs291[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x56,0x36};  
UINT8 dcs292[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x57,0x9E};  
UINT8 dcs293[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x58,0x9E};  
UINT8 dcs294[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x59,0x9E};  
UINT8 dcs295[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x60,0x00};  
UINT8 dcs296[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x61,0x08}; 
UINT8 dcs297[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x62,0x00}; 
UINT8 dcs298[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x63,0x38}; 
UINT8 dcs299[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x67,0x01}; 
UINT8 dcs300[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x6B,0x02}; 
UINT8 dcs301[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x6C,0x36}; 
UINT8 dcs302[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x6D,0x00}; 
UINT8 dcs303[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x6E,0x00}; 
UINT8 dcs304[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x6F,0x36}; 
UINT8 dcs305[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x70,0x00}; 
UINT8 dcs306[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x71,0x01}; 
UINT8 dcs307[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x7A,0x13};  
UINT8 dcs308[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x7B,0x90}; 
UINT8 dcs309[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x7F,0x75}; 
UINT8 dcs310[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x83,0x07}; 
UINT8 dcs311[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x84,0XF9}; 
UINT8 dcs312[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x87,0x07}; 
UINT8 dcs313[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x88,0x77}; 	

//*****************CMD2 Page C***********************************************************************************//
UINT8 dcs314[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFF,0x2C}; 
UINT8 dcs315[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFB,0x01}; 

UINT8 dcs316[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x00,0x04}; 
UINT8 dcs317[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x01,0x04}; 
UINT8 dcs318[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x02,0x04}; 
UINT8 dcs319[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x03,0x2C}; 
UINT8 dcs320[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x04,0x2C}; 
UINT8 dcs321[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x05,0x2C}; 
UINT8 dcs322[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x0D,0x10}; 
UINT8 dcs323[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x0E,0x1A}; 
UINT8 dcs324[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x16,0x01}; 
UINT8 dcs325[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x17,0x95}; 
UINT8 dcs326[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x18,0x95}; 
UINT8 dcs327[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x19,0x95}; 

UINT8 dcs328[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x2D,0xAF}; 
UINT8 dcs329[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x2F,0x10}; 	

UINT8 dcs330[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x30,0xEE}; 
UINT8 dcs331[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x32,0x00}; 	
UINT8 dcs332[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x33,0xEE}; 
UINT8 dcs333[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x35,0x28}; 
UINT8 dcs334[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x37,0x28}; 
UINT8 dcs335[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x4D,0x2C}; 
UINT8 dcs336[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x4E,0x04}; 
UINT8 dcs337[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x4F,0x08}; 
UINT8 dcs338[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x53,0x03}; 
UINT8 dcs339[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x54,0x03}; 
UINT8 dcs340[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x55,0x03};  
UINT8 dcs341[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x56,0x21};  
UINT8 dcs342[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x58,0x21};  
UINT8 dcs343[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x59,0x21};  
UINT8 dcs344[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x61,0x10};   
UINT8 dcs345[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x62,0x1A};   
UINT8 dcs346[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x6A,0x03};  
UINT8 dcs347[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x6B,0x76};  
UINT8 dcs348[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x6C,0x76};  
UINT8 dcs349[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x6D,0x76};  
UINT8 dcs350[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x80,0xAF}; 
UINT8 dcs351[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x81,0x10};
UINT8 dcs352[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x82,0xEA}; 
UINT8 dcs353[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x84,0x00};   
UINT8 dcs354[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x85,0xEA};   
UINT8 dcs355[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x87,0x21};   
UINT8 dcs356[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x89,0x21};   
UINT8 dcs357[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x9D,0x21};   
UINT8 dcs358[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x9E,0x03};   
UINT8 dcs359[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x9F,0x0F};   


//Gamma

//Gamma  

//*****************CMD 2 Page B***********************************************************************************//
UINT8 dcs360[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFF,0x2B};   
UINT8 dcs361[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFB,0x01}; 
UINT8 dcs362[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xB7,0x09};   
UINT8 dcs0362[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xB8,0x16};   
UINT8 dcs363[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xC0,0x01};   

//*****************CMD F Page 0***********************************************************************************//
UINT8 dcs364[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFF,0xF0};   
UINT8 dcs365[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFB,0x01}; 
UINT8 dcs366[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x1C,0x01};   
UINT8 dcs367[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x33,0x01};  
UINT8 dcs368[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x5A,0x00};   //internal DP-TP setting
UINT8 dcs369[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x9C,0x1F};  

//*****************CMD C Page 0***********************************************************************************//
UINT8 dcs370[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFF,0xC0};   
UINT8 dcs371[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFB,0x01}; 
UINT8 dcs372[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x9C,0x11};   
UINT8 dcs373[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x9D,0x11};  
//*****************CMD D Page 0***********************************************************************************//
UINT8 dcs374[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFF,0xD0};   
UINT8 dcs375[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFB,0x01}; 
UINT8 dcs376[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x53,0x22};   
UINT8 dcs377[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x54,0x02};  
//*****************CMD E Page 0***********************************************************************************//
UINT8 dcs378[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFF,0xE0};   
UINT8 dcs379[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFB,0x01}; 
UINT8 dcs380[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x35,0x82};   
UINT8 dcs381[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x85,0x32};  

//*****************20191107 Bist code *****************************************************//
UINT8 dcs382[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFF,0x20}; 
UINT8 dcs383[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFB,0x01};
UINT8 dcs384[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x74,0x00};   
UINT8 dcs385[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x75,0x00};   
UINT8 dcs386[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x76,0x00};   
UINT8 dcs387[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x77,0x00};   
UINT8 dcs388[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x78,0x00};   
UINT8 dcs389[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x79,0x00};   
UINT8 dcs390[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x7A,0x00};   
UINT8 dcs391[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x7B,0x20};   
UINT8 dcs392[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x7C,0x00};   
UINT8 dcs393[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x7D,0x00};   
UINT8 dcs394[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x7E,0x00};   
UINT8 dcs395[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x7F,0x00};   
UINT8 dcs396[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x80,0x00};
//---------------Free run mode	--------//


//---------------60Hz Vporch Bist mode--------//
UINT8 dcs397[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFF,0x2A};   
UINT8 dcs398[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFB,0x01};   
UINT8 dcs399[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x1C,0x0B};  //60Hz
UINT8 dcs400[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x1D,0x50};   //38
UINT8 dcs401[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x1C,0x00};     //120Hz
UINT8 dcs402[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x1D,0x45}; //20 

UINT8 dcs403[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFF,0x25};   
UINT8 dcs404[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFB,0x01};   
UINT8 dcs405[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x18,0x20};  //

UINT8 dcs406[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFF,0x10}; 
//---------------60Hz Vporch Bist mode	--------//

//--------Command 1--------  
UINT8 dcs407[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFF,0x10};   
UINT8 dcs408[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFB,0x01}; 		
//    UINT8 dcs[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xC0,0x00};  //
UINT8 dcs409[] = {MIPITX_DSI_GENERIC_LONG_WRITE,0x35,0x00,0x00};  		

//UINT8 dcs410[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x35,0x00}; 	//TE on	   //AAAAAAAAAAAAAAAA

//  0xF00


//~~~~~~~~~TM697~~~~~~~//

UINT8 dcs411[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFF,0x25}; 
UINT8 dcs412[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFB,0x01}; 
//UINT8 dcs413[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x18,0x21};     //TableB 60Hz
UINT8 dcs414[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x18,0x20};     //TableB 120Hz


UINT8 dcs415[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFF,0x10}; 
UINT8 dcs416[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xFB,0x01}; 
UINT8 dcs417[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xC0,0x03};     //DSC:03, NO DSC:00

//UINT8 dcs00[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0XFF,0x20}; 
//UINT8 dcs01[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0xfb,0x01};
//UINT8 dcs002[] = {MIPITX_DSI_GENERIC_SHORT_WRITE_2_PARAM,0x86,0x03}; //BIST

UINT8 dcs418[] = {MIPITX_DSI_DCS_SHORT_WRITE,0x11,0x00};
//Delay(200};  
UINT8 dcs419[] = {MIPITX_DSI_DCS_SHORT_WRITE,0x29,0x00}; 
//Delay(100};  
UINT8 dcs420[] = {MIPITX_DSI_TURN_ON_PERIPHERAL,0x00,0x00};

UINT8 channel = 0;

/* Send CMD */
//*****************CMD1*************//
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs0, sizeof(dcs0));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs1, sizeof(dcs1));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs2, sizeof(dcs2));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs02, sizeof(dcs02));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs3, sizeof(dcs3));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs4, sizeof(dcs4));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs5, sizeof(dcs5));
//*****************CMD2 Page E***************************************************************************************//

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs6, sizeof(dcs6));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs7, sizeof(dcs7));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs8, sizeof(dcs8));

//*****************CMD2 Page 0***************************************************************************************//

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs9, sizeof(dcs9));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs10, sizeof(dcs10));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs11, sizeof(dcs11));
//***********cl ID******
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs12, sizeof(dcs12));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs13, sizeof(dcs13));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs14, sizeof(dcs14));
//***********CL ID******	
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs15, sizeof(dcs15));//20191024 VGH/ VGL Step-up Frequency setting

//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs16, sizeof(dcs16));  // 3C VGH =8V      64 VGH =10V       
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs17, sizeof(dcs17));  // 3C VGH =8V      64 VGH =10V     
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs18, sizeof(dcs18));  // VGL =-7V	

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs19, sizeof(dcs19));    // 4PWR mode VDD setting   
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs20, sizeof(dcs20));    // 4PWR mode VDD setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs21, sizeof(dcs21));   // notch area setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs22, sizeof(dcs22));    // notch area setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs23, sizeof(dcs23));    // chopper setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs24, sizeof(dcs24)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs25, sizeof(dcs25));

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs26, sizeof(dcs26));    //GVDDP=4.6V
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs27, sizeof(dcs27));    //GVDDP=4.6V
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs28, sizeof(dcs28));    //GVDDN=4.6V
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs29, sizeof(dcs29));    //GVDDP=4.6V

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs29, sizeof(dcs29));    //TP term charge pump setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs30, sizeof(dcs30));    //TP term charge pump setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs31, sizeof(dcs31));    //TP term charge pump setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs32, sizeof(dcs32));    //TP term charge pump setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs33, sizeof(dcs33));    //TP term charge pump setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs34, sizeof(dcs34));    //TP term charge pump setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs35, sizeof(dcs35));    //TP term charge pump setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs36, sizeof(dcs36));    //TP term charge pump setting


//*****************CMD2 Page 3 PWM************************************************************//	
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs37, sizeof(dcs37));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs38, sizeof(dcs38));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs39, sizeof(dcs39));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs40, sizeof(dcs40));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs41, sizeof(dcs41));
//*****************CMD2 Page 3 PWM************************************************************//

//*****************CMD2 Page 4***************************************************************************************//
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs42, sizeof(dcs42));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs43, sizeof(dcs43));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs44, sizeof(dcs44)); //DUMMY     (CGOUTL[1])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs45, sizeof(dcs45)); //STV       (CGOUTL[2])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs46, sizeof(dcs46)); //CKV2_L    (CGOUTL[3])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs47, sizeof(dcs47)); //CKV4_L    (CGOUTL[4])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs48, sizeof(dcs48)); //CKV1_L    (CGOUTL[5])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs49, sizeof(dcs49)); //CKV3_L    (CGOUTL[6])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs50, sizeof(dcs50)); //D2U       (CGOUTL[7])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs51, sizeof(dcs51)); //U2D       (CGOUTL[8])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs52, sizeof(dcs52)); //DUMMY     (CGOUTL[9])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs53, sizeof(dcs53)); //GAS       (CGOUTL[10])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs54, sizeof(dcs54)); //GRESET    (CGOUTL[11])	
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs55, sizeof(dcs55)); //VTCOMSW1  (CGOUTL[12])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs56, sizeof(dcs56)); //CKHB      (CGOUTL[13])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs57, sizeof(dcs57)); //CKHG      (CGOUTL[14])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs58, sizeof(dcs58)); //CKHR      (CGOUTL[15])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs59, sizeof(dcs59)); //CKHB      (CGOUTL[16])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs60, sizeof(dcs60)); //CKHG      (CGOUTL[17])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs61, sizeof(dcs61)); //CKHR      (CGOUTL[18])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs62, sizeof(dcs62)); //DUMMY     (CGOUTL[19])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs63, sizeof(dcs63)); //DE        (CGOUTL[20])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs64, sizeof(dcs64)); //DO        (CGOUTL[21])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs65, sizeof(dcs65)); //VTSW      (CGOUTL[22])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs66, sizeof(dcs66)); //DUMMY     (CGOUTL[23])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs67, sizeof(dcs67)); //DUMMY     (CGOUTL[24]) 

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs68, sizeof(dcs68));  //DUMMY     (CGOUTR[1])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs69, sizeof(dcs69)); //STV       (CGOUTR[2])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs70, sizeof(dcs70));  //CKV2_R    (CGOUTR[3])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs71, sizeof(dcs71));  //CKV4_R    (CGOUTR[4])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs72, sizeof(dcs72));  //CKV1_R    (CGOUTR[5])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs73, sizeof(dcs73));  //CKV3_R    (CGOUTR[6])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs74, sizeof(dcs74));  //D2U       (CGOUTR[7])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs75, sizeof(dcs75));  //U2D       (CGOUTR[8])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs76, sizeof(dcs76));  //DUMMY     (CGOUTR[9])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs77, sizeof(dcs77));  //GAS       (CGOUTR[10])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs78, sizeof(dcs78));  //GRESET    (CGOUTR[11])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs79, sizeof(dcs79));  //VTCOMSW1  (CGOUTR[12])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs80, sizeof(dcs80));  //CKHB      (CGOUTR[13])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs81, sizeof(dcs81));  //CKHG      (CGOUTR[14])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs82, sizeof(dcs82));  //CKHR      (CGOUTR[15])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs83, sizeof(dcs83));  //CKHB      (CGOUTR[16])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs84, sizeof(dcs84));  //CKHG      (CGOUTR[17])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs85, sizeof(dcs85));  //CKHR      (CGOUTR[18])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs86, sizeof(dcs86));  //DUMMY     (CGOUTR[19])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs87, sizeof(dcs87));  //DE        (CGOUTR[20])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs88, sizeof(dcs88));  //DO        (CGOUTR[21])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs89, sizeof(dcs89));  //VTSW      (CGOUTR[22])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs90, sizeof(dcs90)); //DUMMY     (CGOUTR[23])
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs91, sizeof(dcs91));  //DUMMY     (CGOUTR[24])       


mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs92, sizeof(dcs92));  //STVR/STVL sequence setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs93, sizeof(dcs93));  //STV rising position by line                         
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs94, sizeof(dcs94)); //STV falling position  by line
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs95, sizeof(dcs95));  //STV start position by clk of Table A            
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs96, sizeof(dcs96));  //STV end position by clk of Table A   
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs97, sizeof(dcs97));  //STV start position by clk of Table A            
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs98, sizeof(dcs98));  //STV end position by clk of Table A  
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs99, sizeof(dcs99));  //CKV start position of Table A  02              
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs100, sizeof(dcs100));  //normal line CKV end position of Table A  3c  
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs101, sizeof(dcs101));  //line N+1 CKV end position of Table A     3c
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs102, sizeof(dcs102));  //line N CKV End position                  3c
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs103, sizeof(dcs103));  //CKV start position of Table A  02              
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs104, sizeof(dcs104));  //normal line CKV end position of Table A  3c  
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs105, sizeof(dcs105));  //line N+1 CKV end position of Table A     3c
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs106, sizeof(dcs106)); //line N CKV End position                  3c
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs107, sizeof(dcs107)); //MUX dummy line in VFP&VBP porch
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs108, sizeof(dcs108)); //MUX rising position of Table A
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs109, sizeof(dcs109)); //MUX rising position of Table A
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs110, sizeof(dcs110)); //normal line MUX width of table A    8E 

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs111, sizeof(dcs111));  //normal line MUXG1 setting 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs112, sizeof(dcs112));  //Source output hold time                
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs113, sizeof(dcs113)); //Source signal delay time
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs114, sizeof(dcs114));  //normal line MUXG1 setting 
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs115, sizeof(dcs115)); //Source output hold time                
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs116, sizeof(dcs116));  //Source signal delay time

//*****************RGBBGR****************
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs117, sizeof(dcs117));  //MUX sequence
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs118, sizeof(dcs118));  //MUX sequence
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs119, sizeof(dcs119));  //MUX sequence
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs120, sizeof(dcs120));  //MUX sequence
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs121, sizeof(dcs121));  //MUX sequence

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs122, sizeof(dcs122));  // Source RGB sequence
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs123, sizeof(dcs123));  // Source RGB sequence
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs124, sizeof(dcs124));  // Source RGB sequence
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs125, sizeof(dcs125));  // Source RGB sequence
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs126, sizeof(dcs126));  // Source RGB sequence
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs127, sizeof(dcs127));  // RGBBGR setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs128, sizeof(dcs128));  // RGBBGR setting

//*****************RGBBGR****************
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs129, sizeof(dcs129));  // line n MUX width of table A
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs130, sizeof(dcs130));  // line n+1 MUX width of table A
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs131, sizeof(dcs131));  // line n MUX start position of table A

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs132, sizeof(dcs132));  // line n MUXG1 setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs133, sizeof(dcs133));  // line n+1 MUXG1 setting
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs134, sizeof(dcs134));  // line n MUXG1 setting
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs135, sizeof(dcs135));  // line n+1 MUXG1 setting

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs136, sizeof(dcs136));  // slew-rate setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs137, sizeof(dcs137));  // slew-rate setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs138, sizeof(dcs138));  // disable internal no use IP setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs139, sizeof(dcs139));  // up/down & left/right setting
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs140, sizeof(dcs140));  // RGBBGR setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs141, sizeof(dcs141));  // RGBBGR setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs142, sizeof(dcs142));  // Line n+1 MUX rising position

//*****************CMD2 Page 5***************************************************************************************//
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs143, sizeof(dcs143));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs144, sizeof(dcs144));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs145, sizeof(dcs145));   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs146, sizeof(dcs146));   //table select
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs147, sizeof(dcs147));  //frame rate mapping
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs148, sizeof(dcs148));  //frame rate mapping
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs149, sizeof(dcs149));   //frame rate mapping   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs150, sizeof(dcs150)); //U2D/D2U power on setting             
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs151, sizeof(dcs151)); //20190926 D2U power off setting                            
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs152, sizeof(dcs152));  //U2D/D2U power off setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs153, sizeof(dcs153));  //MUX power off setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs154, sizeof(dcs154));   //MUX start active position during power on sequence
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs155, sizeof(dcs155));  //STV power on/off setting                                       
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs156, sizeof(dcs156));  //sleep-in setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs157, sizeof(dcs157));  //RTN setting during power off sequence 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs158, sizeof(dcs158));  //CKV power on stting                                       
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs159, sizeof(dcs159));  //CKV power on/off setting                                      
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs160, sizeof(dcs160));                                      
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs161, sizeof(dcs161));  //GAS power on setting

//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs162, sizeof(dcs162));                                    
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs163, sizeof(dcs163));                                      
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs164, sizeof(dcs164));  //20190926      GAS power off setting                      
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs165, sizeof(dcs165)); //GRESET power on setting                                       
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs166, sizeof(dcs166)); //GRESET power off setting                                       
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs167, sizeof(dcs167)); //VCOMSW power on/off setting                                       
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs168, sizeof(dcs168));   //GRESET power on setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs169, sizeof(dcs169));   //VCOMSW power on setting

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs170, sizeof(dcs170)); // MUX EQ setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs171, sizeof(dcs171)); // MUX EQ setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs172, sizeof(dcs172)); // Falling EQ for positive polarity                              DA ????
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs173, sizeof(dcs173)); // MUX EQ setting

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs174, sizeof(dcs174)); // Falling EQ for negative polarity                              E0
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs175, sizeof(dcs175));                                      
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs176, sizeof(dcs176));  //APO setting                                       
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs177, sizeof(dcs177));  //APO setting



//*****************CMD2 Page 6*************************************************************************************//
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs178, sizeof(dcs178));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs179, sizeof(dcs179));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs180, sizeof(dcs180)); //GRESET pulse width setting during TP term bit[11:8] of Table A 

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs181, sizeof(dcs181)); //GRESET pulse width setting during TP term bit[7:0] of Table A 

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs182, sizeof(dcs182)); //VCOMSW pulse width setting during TP term bit[11:8] of Table A 

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs183, sizeof(dcs183));  //VCOMSW pulse width setting during TP term bit[7:0] of Table A 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs184, sizeof(dcs184));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs185, sizeof(dcs185)); //GRESET shift setting during TP term of Table A

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs186, sizeof(dcs186));   //VCOMSW shift setting during TP term of Table A

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs187, sizeof(dcs187));  //GIP setting during V-porch LHB

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs188, sizeof(dcs188)); //MUX timing during V-porch LHB

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs189, sizeof(dcs189));   //MUX setting during TP term of Table A

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs190, sizeof(dcs190)); //TP ter timing setting                                    
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs191, sizeof(dcs191)); //TP ter timing setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs192, sizeof(dcs192)); //TP ter timing setting	                                     
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs193, sizeof(dcs193)); //TP ter timing setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs194, sizeof(dcs194)); //TP ter timing setting	                                     
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs195, sizeof(dcs195)); //TP ter timing setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs196, sizeof(dcs196)); //TP ter timing setting	                                     
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs197, sizeof(dcs197)); //rescan line RGBBGR setting                                    
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs198, sizeof(dcs198));   //rescan line source setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs199, sizeof(dcs199)); //rescan line source RGB sequence                                    
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs200, sizeof(dcs200)); //rescan line source RGB sequence	                                     
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs201, sizeof(dcs201)); //rescan line source RGB sequence
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs202, sizeof(dcs202)); //rescan line source RGB sequence
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs203, sizeof(dcs203)); //rescan line source RGB sequence
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs204, sizeof(dcs204));  //TP ter timing setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs205, sizeof(dcs205));  //TP ter timing setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs206, sizeof(dcs206));  //TP ter timing setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs207, sizeof(dcs207));  //TP ter timing setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs208, sizeof(dcs208)); //TP ter timing setting


//*****************CMD2 Page 7*************************************************************************************//
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs209, sizeof(dcs209));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs210, sizeof(dcs210));

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs211, sizeof(dcs211));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs212, sizeof(dcs212));  //vertical resolution	                                                                                                             
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs213, sizeof(dcs213));   //OSC1 video trim setting bit[14:8] of Tabe A 	                                        
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs214, sizeof(dcs214)); //OSC1 video trim setting bit[7:0] of Tabe A                                        
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs215, sizeof(dcs215));  //OSC2 video trim setting bit[14:8] of Tabe A OSC Trim bit7 1:enable 0:disable       
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs216, sizeof(dcs216));  //OSC2 video trim setting bit[7:0] of Tabe A                                        
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs217, sizeof(dcs217));  //CKV sequence setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs218, sizeof(dcs218));  //CKV sequence setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs219, sizeof(dcs219));  //CKV sequence setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs220, sizeof(dcs220));  //CKV sequence setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs221, sizeof(dcs221));   //CKV sequence setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs222, sizeof(dcs222));   //CKV sequence setting	                                      
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs223, sizeof(dcs223));  //CKV sequence setting	                                      
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs224, sizeof(dcs224));  //CKV sequence setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs225, sizeof(dcs225));   //CKV sequence setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs226, sizeof(dcs226));   //CKV sequence setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs227, sizeof(dcs227));   //CKV sequence setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs228, sizeof(dcs228));   //CKV sequence setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs229, sizeof(dcs229));   //CKV sequence setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs230, sizeof(dcs230));  //CKV sequence setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs231, sizeof(dcs231));   //CKV sequence setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs232, sizeof(dcs232));   //STV BW rsing setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs233, sizeof(dcs233)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs234, sizeof(dcs234));   //STV power on setting

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs235, sizeof(dcs235));   //OSC1 video trim setting bit[14:8] of Tabe B 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs236, sizeof(dcs236));   //OSC1 video trim setting bit[7:0] of Tabe B 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs237, sizeof(dcs237));   //OSC1 video trim setting bit[14:8] of Tabe C
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs238, sizeof(dcs238));  //OSC1 video trim setting bit[7:0] of Tabe C
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs239, sizeof(dcs239));  //OSC2 video trim setting bit[14:8] of Tabe B 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs240, sizeof(dcs240));   //OSC2 video trim setting bit[7:0] of Tabe B 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs241, sizeof(dcs241));   //OSC2 video trim setting bit[14:8] of Tabe C
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs242, sizeof(dcs242));    //OSC2 video trim setting bit[7:0] of Tabe C

//*****************CMD2 Page A***********************************************************************************//
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs243, sizeof(dcs243));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs244, sizeof(dcs244));

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs245, sizeof(dcs245));  //20190926  table switch setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs246, sizeof(dcs246));  //20190926  table switch setting
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs247, sizeof(dcs247));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs248, sizeof(dcs248));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs249, sizeof(dcs249));  //Internal TP signal setting of Table A
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs250, sizeof(dcs250)); //TP unit number of Table A   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs251, sizeof(dcs251));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs252, sizeof(dcs252));   //Internal TP setting of Table A
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs253, sizeof(dcs253)); //Line number for a unit bit[11:8] of Table A

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs254, sizeof(dcs254)); //Line number for a unit bit[7:0] of Table A                                      	
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs255, sizeof(dcs255));  //Internal TP H-sync #1 setting bit[15:8] of Table A
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs256, sizeof(dcs256));  //Internal TP H-sync #1 setting bit[7:0] of Table A
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs257, sizeof(dcs257));  //Internal TP H-sync #2 setting bit[15:8] of Table A
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs258, sizeof(dcs258)); //Internal TP H-sync #2 setting bit[7:0] of Table A

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs259, sizeof(dcs259));                                    
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs260, sizeof(dcs260));                                    
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs261, sizeof(dcs261)); //RTNA
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs262, sizeof(dcs262)); //RTNA_LINE
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs263, sizeof(dcs263)); //RTNA_LINE+1
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs264, sizeof(dcs264));   //V-porch sensing setting of Table A
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs265, sizeof(dcs265));   //V-porch sensing setting of Table A 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs266, sizeof(dcs266)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs267, sizeof(dcs267)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs268, sizeof(dcs268)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs269, sizeof(dcs269)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs270, sizeof(dcs270));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs271, sizeof(dcs271));   //V-porch sensing setting of Table A
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs272, sizeof(dcs272));   //V-porch sensing setting of Table A
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs273, sizeof(dcs273));  
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs274, sizeof(dcs274));   //V-porch sensing setting of Table A
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs275, sizeof(dcs275));  //V-porch sensing setting of Table A
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs276, sizeof(dcs276));   //V-porch sensing setting of Table A
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs277, sizeof(dcs277));   //V-porch sensing setting of Table A
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs278, sizeof(dcs278));   //V-porch sensing setting of Table A
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs279, sizeof(dcs279));   //V-porch sensing setting of Table A
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs280, sizeof(dcs280));   //V-porch sensing setting of Table A
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs281, sizeof(dcs281));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs282, sizeof(dcs282));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs283, sizeof(dcs283));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs284, sizeof(dcs284)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs285, sizeof(dcs285));  
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs286, sizeof(dcs286));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs287, sizeof(dcs287));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs288, sizeof(dcs288));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs289, sizeof(dcs289)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs290, sizeof(dcs290));  
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs291, sizeof(dcs291));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs292, sizeof(dcs292));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs293, sizeof(dcs293));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs294, sizeof(dcs294)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs295, sizeof(dcs295));  
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs296, sizeof(dcs296));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs297, sizeof(dcs297));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs298, sizeof(dcs298));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs299, sizeof(dcs299)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs300, sizeof(dcs300));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs301, sizeof(dcs301));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs302, sizeof(dcs302));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs303, sizeof(dcs303));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs304, sizeof(dcs304)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs305, sizeof(dcs305));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs306, sizeof(dcs306));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs307, sizeof(dcs307));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs308, sizeof(dcs308));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs309, sizeof(dcs309)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs310, sizeof(dcs310)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs311, sizeof(dcs311));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs312, sizeof(dcs312));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs313, sizeof(dcs313)); 

//*****************CMD2 Page C***********************************************************************************//
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs314, sizeof(dcs314));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs315, sizeof(dcs315)); 

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs316, sizeof(dcs316)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs317, sizeof(dcs317)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs318, sizeof(dcs318));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs319, sizeof(dcs319));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs320, sizeof(dcs320));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs321, sizeof(dcs321));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs322, sizeof(dcs322)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs323, sizeof(dcs323));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs324, sizeof(dcs324));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs325, sizeof(dcs325));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs326, sizeof(dcs326));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs327, sizeof(dcs327)); 

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs328, sizeof(dcs328));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs329, sizeof(dcs329));

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs330, sizeof(dcs330)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs331, sizeof(dcs331));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs332, sizeof(dcs332)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs333, sizeof(dcs333));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs334, sizeof(dcs334));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs335, sizeof(dcs335));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs336, sizeof(dcs336));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs337, sizeof(dcs337)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs338, sizeof(dcs338));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs339, sizeof(dcs339));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs340, sizeof(dcs340));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs341, sizeof(dcs341));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs342, sizeof(dcs342)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs343, sizeof(dcs343)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs344, sizeof(dcs344));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs345, sizeof(dcs345));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs346, sizeof(dcs346)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs347, sizeof(dcs347));  
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs348, sizeof(dcs348)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs349, sizeof(dcs349));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs350, sizeof(dcs350));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs351, sizeof(dcs351));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs352, sizeof(dcs352));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs353, sizeof(dcs353)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs354, sizeof(dcs354));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs355, sizeof(dcs355));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs356, sizeof(dcs356));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs357, sizeof(dcs357));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs358, sizeof(dcs358)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs359, sizeof(dcs359));   

//Gamma

//Gamma  

//*****************CMD 2 Page B***********************************************************************************//
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs360, sizeof(dcs360));   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs361, sizeof(dcs361)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs362, sizeof(dcs362));   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs0362, sizeof(dcs0362)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs363, sizeof(dcs363));    

//*****************CMD F Page 0***********************************************************************************//
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs364, sizeof(dcs364));   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs365, sizeof(dcs365)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs366, sizeof(dcs366));   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs367, sizeof(dcs367));  
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs368, sizeof(dcs368));  //internal DP-TP setting
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs369, sizeof(dcs369)); 

//*****************CMD C Page 0***********************************************************************************//
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs370, sizeof(dcs370));   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs371, sizeof(dcs371)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs372, sizeof(dcs372));   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs373, sizeof(dcs373));  
//*****************CMD D Page 0***********************************************************************************//
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs374, sizeof(dcs374));   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs375, sizeof(dcs375)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs376, sizeof(dcs376));   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs377, sizeof(dcs377));  
//*****************CMD E Page 0***********************************************************************************//
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs378, sizeof(dcs378));   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs379, sizeof(dcs379)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs380, sizeof(dcs380));   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs381, sizeof(dcs381));  

//*****************20191107 Bist code *****************************************************//
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs382, sizeof(dcs382));   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs383, sizeof(dcs383)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs384, sizeof(dcs384));   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs385, sizeof(dcs385));;   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs386, sizeof(dcs386));   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs387, sizeof(dcs387)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs388, sizeof(dcs388));   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs389, sizeof(dcs389));   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs390, sizeof(dcs390));   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs391, sizeof(dcs391)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs392, sizeof(dcs392));   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs393, sizeof(dcs393));   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs394, sizeof(dcs394));   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs395, sizeof(dcs395)); 
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs396, sizeof(dcs396));   
//---------------Free run mode	--------//


//---------------60Hz Vporch Bist mode--------//
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs397, sizeof(dcs397));  
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs398, sizeof(dcs398));  
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs399, sizeof(dcs399));  //60Hz
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs400, sizeof(dcs400));   //38
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs401, sizeof(dcs401));     //120Hz
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs402, sizeof(dcs402)); //20 

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs403, sizeof(dcs403));  
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs404, sizeof(dcs404));  
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs405, sizeof(dcs405));  //

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs406, sizeof(dcs406));
//---------------60Hz Vporch Bist mode	--------//

//--------Command 1--------  
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs407, sizeof(dcs407));;   
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs408, sizeof(dcs408));

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs409, sizeof(dcs409));

//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs410, sizeof(dcs410));	//TE on	   //AAAAAAAAAAAAAAAA

//  0xF00


//~~~~~~~~~TM697~~~~~~~//

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs411, sizeof(dcs411));  
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs412, sizeof(dcs412));
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs413, sizeof(dcs413));     //TableB 60Hz
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs414, sizeof(dcs414));     //TableB 120Hz


mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs415, sizeof(dcs415));  
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs416, sizeof(dcs416));
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs417, sizeof(dcs417));     //DSC:03, NO DSC:00

//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs00, sizeof(dcs00));  
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs01, sizeof(dcs01)); 
//mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs002, sizeof(dcs002)); //BIST

mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs418, sizeof(dcs418));
//Delay(200};  
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs419, sizeof(dcs419));
//Delay(100};  
mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, dcs420, sizeof(dcs420)); 

#else

    rtd_part_outl(0x1806A800, 1, 1, 0x0);	// CFG_State
    //====Jira RL6951-4802===//
    rtd_part_outl(0x1806A000, 30, 28, 0x1);	// sfg_data_sel_in=0x1
    rtd_part_outl(0x1806A800, 0, 0, 0x0);	// disp_disable
    rtd_part_outl(0x1806A800, 0, 0, 0x1);	// disp_enable
    //=======================//

    //*****************CMD1*************//
    rtd_part_outl(0x1806A81C, 23, 16, 0x2);	// vbk_pkt_byte_num
    IoReg_Write32(0x1806A830, 0x0010FF23);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    rtd_part_outl(0x1806A81C, 23, 16, 0x2);	// vbk_pkt_byte_num
    IoReg_Write32(0x1806A830, 0x0001FB23);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    rtd_part_outl(0x1806A81C, 23, 16, 0x2);	// vbk_pkt_byte_num 
    IoReg_Write32(0x1806A830, 0x00003623);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    rtd_part_outl(0x1806A81C, 23, 16, 0x13);	// vbk_pkt_byte_num 
    IoReg_Write32(0x1806A830, 0xC1001129);// 20191024 PPS table setting
    IoReg_Write32(0x1806A834, 0x14000889);// 
    IoReg_Write32(0x1806A838, 0x0E02AA00);// 
    IoReg_Write32(0x1806A83C, 0x07007100);// 
    IoReg_Write32(0x1806A840, 0x16050E05);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    rtd_part_outl(0x1806A81C, 23, 16, 0x8);	// vbk_pkt_byte_num 
    IoReg_Write32(0x1806A830, 0x3B000629);// 0X214}; //VBP//0X236}; //VFP 0X204};  0X204}; 
    IoReg_Write32(0x1806A834, 0x04361403);// 
    IoReg_Write32(0x1806A838, 0x00000004);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply 
    rtd_part_outl(0x1806A81C, 23, 16, 0x2);	// vbk_pkt_byte_num 
    IoReg_Write32(0x1806A830, 0x0000B023);//MIPI CRC disabled 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    rtd_part_outl(0x1806A81C, 23, 16, 0x5);	// vbk_pkt_byte_num 
    IoReg_Write32(0x1806A830, 0xC2000329);//20191024 PPS table setting
    IoReg_Write32(0x1806A834, 0x0000A01B);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply 
    //*****************CMD1*************//
    //*****************CMD2 Page E***************************************
    rtd_part_outl(0x1806A81C, 23, 16, 0x2);	// vbk_pkt_byte_num 
    IoReg_Write32(0x1806A830, 0x00E0FF23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0001FB23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00C01F23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    //*****************CMD2 Page 0*************************************
    IoReg_Write32(0x1806A830, 0x0020FF23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0001FB23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x001D3023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    //***********cl ID******
    IoReg_Write32(0x1806A830, 0x00004423);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00004523);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00934623);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    //***********CL ID******	
    IoReg_Write32(0x1806A830, 0x00660123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00640623);// 3C VGH =8V      64 VGH =10V
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00280723);// VGL =-7V	
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00001B23);// 4PWR mode VDD setting 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00705C23);// notch area setting
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00CA5E23);// notch area setting
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00D06923);// chopper setting
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x003A8923);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x003A8A23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00A99523);//GVDDP=4.6V
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00A99623);//GVDDN=4.6V
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0065F223);// TP term charge pump setting
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0044F323);// TP term charge pump setting
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0065F423);// TP term charge pump setting
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0044F523);// TP term charge pump setting
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0065F623);// TP term charge pump setting
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0044F723);// TP term charge pump setting
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0065F823);// TP term charge pump setting
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0044F923);// TP term charge pump setting
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply

    //*****************CMD2 Page 3 PWM************************************
    IoReg_Write32(0x1806A830, 0x0023FF23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0001FB23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00200723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00070823);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00040923);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    //*****************CMD2 Page 3 PWM************************************
    //*****************CMD2 Page 4(GOA)****************************************

    IoReg_Write32(0x1806A830, 0x0024FF23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0001FB23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00200023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00200123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00200223);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00200323);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00200423);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00200523);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00130623);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00150723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00170823);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00130923);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00150A23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00170B23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00240C23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00010D23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x002F0E23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x002D0F23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x002E1023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x002C1123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x008B1223);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x008C1323);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00201423);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00201523);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x000f1623);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00221723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00201823);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00201923);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00201a23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00201b23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00201c23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00201d23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00131e23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00151f23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00172023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00132123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00152223);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00172323);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00242423);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00012523);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x002f2623);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x002d2723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x002e2823);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x002c2923);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x008b2a23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x008C2B23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00202D23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00202F23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x000F3023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00223123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00003223);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00033323);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00013423);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x001F3523);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x002A3623);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00064D23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00464E23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00464F23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00465323);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00037923);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00867A23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00927B23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00067D23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00068023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00068123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    //*****************CMD2 Page 4****************************************
    //*****************RGBBGR****************
    IoReg_Write32(0x1806A830, 0x00138223);//MUX sequence
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00318423);//MUX sequence
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00008523);//MUX sequence
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00008623);//MUX sequence
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00008723);//MUX sequence
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00139023);//Source RGB sequence
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00319223);//Source RGB sequence
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00009323);//Source RGB sequence
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00009423);//Source RGB sequence
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00009523);//Source RGB sequence
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00f49c23);//RGBBGR setting
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00019d23);//RGBBGR setting
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    //*****************RGBBGR****************
    IoReg_Write32(0x1806A830, 0x0012a023);// line n MUX width of table A
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0012a223);// line n+1 MUX width of table A
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0006a323);// line n MUX start position of table A
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    rtd_part_outl(0x1806A81C, 23, 16, 0x2);	 // line n MUX start position of table A
    IoReg_Write32(0x1806A830, 0x0006a423);// line n MUXG1 setting
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0006a523);// line n+1 MUXG1 setting
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0080c423);// slew-rate setting
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00c0c623);// slew-rate setting
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0000c923);// disable internal no use IP setting
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0030d123);// up/down & left/right setting
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0080d923);// RGBBGR setting
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0006e923);// Line n+1 MUX rising position
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    //*****************CMD2 Page 5****************
    IoReg_Write32(0x1806A830, 0x0025ff23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0001fb23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x001b0f23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00201823);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00e41923);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00012023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00002123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00406623);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00296723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00506823);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00606923);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00006b23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x006d7123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00607723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x007a7923);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00407d23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x002d7e23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00708423);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x004dc023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00a9c123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00d2c223);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0011c423);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0011c623);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0080d623);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0002d723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0002da23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0002dd23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0002e023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0000f023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0004f123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    //*****************CMD2 Page 6*************************************************************************************//
    IoReg_Write32(0x1806A830, 0x0026ff23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0001fb23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00100023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00ee0123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00000323);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00ee0423);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00080523);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00140623);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00140823);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00021423);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00011523);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00af7423);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00148123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00038323);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00048423);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00018523);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00048623);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00018723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00028823);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x001a8a23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00118b23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00248c23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00428e23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00118f23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00119023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00119123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00809a23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00049b23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00009c23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00009d23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00009e23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    //*****************CMD2 Page 7*************************************************************************************//
    IoReg_Write32(0x1806A830, 0x0027ff23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0001fb23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00740023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00800123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00812023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00C82123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00812523);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00FA2623);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00236E23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00016F23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00007023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00007123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00007223);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00217323);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00007023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00037423);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00007523);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00007623);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00007723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00077D23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00837E23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00238023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00078223);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00838323);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00038823);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00018923);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0004B723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0003E323);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0091E423);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0000E523);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x007BE623);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0003E923);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00F3EA23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0000EB23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x007BEC23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    //*****************CMD2 Page A***********************************************************************************//
    IoReg_Write32(0x1806A830, 0x002AFF23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0001FB23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00910023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00200323);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00280723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00600A23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00040C23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00400D23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00010E23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00010F23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00801123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x000E1523);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00E61623);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x000E1923);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00BA1A23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00141B23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00361D23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x004F1E23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x004F1F23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x004F2023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00802723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00FD2823);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00082923);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00012A23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00002B23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00082D23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00012F23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00453023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00353123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00113323);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00FF3423);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x002A3523);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00733623);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00FB3723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x002D3823);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00703923);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00453A23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00094523);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00404623);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00024723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00004823);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00C04A23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x000E4E23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00E64F23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x000E5223);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00BA5323);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00145423);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00365623);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x009E5723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x009E5823);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x009E5923);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00006023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00086123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00006223);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00386323);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00016723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00026B23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00366C23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00006D23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00006E23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00366F23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00007023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00017123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00137A23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00907B23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00757F23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00078323);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00F98423);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00078723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00778823);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    //*****************CMD2 Page C***********************************************************************************//
    IoReg_Write32(0x1806A830, 0x002CFF23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0001FB23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00040023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00040123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00040223);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x002C0323);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x002C0423);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x002C0523);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00100D23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x001A0E23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00011623);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00951723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00951823);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00951923);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00AF2D23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00102F23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00EE3023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00003223);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00EE3323);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00283523);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00283723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x002C4D23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00044E23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00084F23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00035323);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00035423);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00035523);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00215623);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00215823);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00215923);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00106123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x001A6223);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00036A23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00766B23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00766C23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00766D23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00AF8023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00108123);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00EA8223);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00008423);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00EA8523);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00218723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00218923);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00219D23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00039E23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x000F9F23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    //Gamma
    //Gamma  
    //*****************CMD 2 Page B***********************************************************************************//
    IoReg_Write32(0x1806A830, 0x002BFF23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0001FB23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0009B723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0016B823);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0001C023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    //*****************CMD F Page 0***********************************************************************************//
    IoReg_Write32(0x1806A830, 0x00F0FF23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0001FB23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00011C23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00013323);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00005A23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x001F9C23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    //*****************CMD C Page 0***********************************************************************************//
    IoReg_Write32(0x1806A830, 0x00C0FF23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0001FB23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00119C23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00119D23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    //*****************CMD D Page 0***********************************************************************************//
    IoReg_Write32(0x1806A830, 0x00D0FF23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0001FB23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00225323);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00025423);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    //*****************CMD E Page 0***********************************************************************************//
    IoReg_Write32(0x1806A830, 0x00E0FF23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0001FB23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00823523);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00328523);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    //*****************20191107 Bist code *****************************************************//
    IoReg_Write32(0x1806A830, 0x0020FF23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0001FB23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00007423);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00007523);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00007623);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00007723);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00007823);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00007923);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00007A23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00207B23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00007C23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00007D23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00007E23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00007F23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00008023);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    //---------------Free run mode	--------//
    //---------------60Hz Vporch Bist mode--------//
    IoReg_Write32(0x1806A830, 0x002AFF23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0001FB23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x000B1C23);//  60Hz  
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply  
    IoReg_Write32(0x1806A830, 0x00501D23);//  38     
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00001C23);//  120Hz
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00451D23);//  20
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0025FF23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0001FB23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00201823);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0010FF23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    //---------------60Hz Vporch Bist mode--------//

    //--------Command 1-------- 
    IoReg_Write32(0x1806A830, 0x0010FF23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0001FB23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    rtd_part_outl(0x1806A81C, 23, 16, 0x5);	// vbk_pkt_byte_num 
    IoReg_Write32(0x1806A830, 0x35000329);// TE
    IoReg_Write32(0x1806A834, 0x00000000);// TE 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply 

    //~~~~~~~~~TM697~~~~~~~//
    rtd_part_outl(0x1806A81C, 23, 16, 0x2);	// vbk_pkt_byte_num 
    IoReg_Write32(0x1806A830, 0x0025FF23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0001FB23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00201823);//TableB 120Hz
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0010FF23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0001FB23);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x0003C023);//DSC:03, NO DSC:00
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A830, 0x00001105);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    //Delay(200};  
    IoReg_Write32(0x1806A830, 0x00002905);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    //Delay(100}; 
    IoReg_Write32(0x1806A830, 0x00000032);//
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply

    //====Jira RL6951-4802===//
    rtd_part_outl(0x1806A000, 30, 28, 0x2);	// sfg_data_sel_in=0x2
    //=======================//
    rtd_part_outl(0x1806A800, 1, 1, 0x1);	// disp_en
#endif
}

void mipitx_TM697_DSC_sfg_config(void)
{
    IoReg_Write32(0x1806A000, 0x23000500);	// sfg_data_sel_in[30:28]=0x2;sfg_top_bps_en[25]=0x1;dly_src_sel[24]=0x1;sfg_in_dly_val[15:4]=0x50
    IoReg_Write32(0x1806A0A0, 0x00000001);	// dsce2_linbuf_en[0]=0x1
    IoReg_Write32(0x1802D950, 0x078A000A);	// Dv_den_end[29:16] = 1930, Dv_den_sta[13:0] = 10
    IoReg_Write32(0x1802D954, 0x0092000B);	// Dh_den_end[29:16] = 146, Dh_den_sta[13:0] = 11
    IoReg_Write32(0x1802D958, 0x0202009D);	// Dv_width[31:24] = 0x9, Dh_width[23:16] = 2, Dh_total[13:0] = 0x157
    IoReg_Write32(0x1806A00C, 0xC010021B);	// dsc_byte_swap[31]=0x1;chunk_adj_dmy_en[30]=0x1;slice_num[21:20]=0x1;chunk_dmy_num[14:0]=0x21b
}

void mipitx_TM697_DSCE2_config(DSCE2_CONFIG_PARAMETER *dsce2_config)
{
    if(!dsce2_config)
        return;
    dsce2_config->dsce2_hactive = default_mipitx_parameter.hwidth;
    dsce2_config->dsce2_vactive = default_mipitx_parameter.vlength;
    dsce2_config->dsce2_htotal = default_mipitx_parameter.htotal;
    dsce2_config->dsce2_vtotal = default_mipitx_parameter.vtotal;
    dsce2_config->dsce2_hsync = default_mipitx_parameter.hsyncwidth;
    dsce2_config->dsce2_vsync = default_mipitx_parameter.vsyncwidth;
    dsce2_config->dsce2_hfront = default_mipitx_parameter.hfrontporch;
    dsce2_config->dsce2_hback = default_mipitx_parameter.hbackporch;
    dsce2_config->dsce2_vfront = default_mipitx_parameter.vfrontporch;
    dsce2_config->dsce2_vback = default_mipitx_parameter.vbackporch;
    dsce2_config->dsce2_en = default_mipitx_parameter.dsc_en;
    dsce2_config->dsce2_pps = default_mipitx_parameter.dsc_pps;

}

void mipitx_TM697_DSC_panel(void)
{
// fhd dsc 1080*1920
//DTG DTG dv_den_sta=0x14, dv_den_end=0x794, dh_den_sta=0x8, dh_den_end=0x8f, dhtot=0x263, dvtot=0x7c9, hs_width=0x2, vs_width=0x9
//HTT=1260,Hden=1080,HFp=90,Hsw=18,Hbp=72,Vtt=1944,Vden=1920,VFp=14,Vsw=2,Vbp=8
    dsce2_enc_clock_st dsce2_enc_clock;
    DSCE2_CONFIG_PARAMETER dsce2_config;

    dsce2_enc_clock.dsce2_src_div = 0;
    dsce2_enc_clock.dsce2_pixel_mode = 1;

    // 1. config mipi panel parameters
    mipitx_TM697_DSC_panel_config();
    mipitx_TM697_DSCE2_config(&dsce2_config);

    // 2. mipitx dphy init
    mipitx_dphy_init();

    // 3. mipitx dsi setting config
    mipitx_dsi_config();

    // 4. sfg setting
    mipitx_TM697_DSC_sfg_config();


    // 5.mipitx DSCE2 PPS config
    dsce2_driver_Config(&dsce2_config, &dsce2_enc_clock);

    // 6. commad mode init
    mipitx_TM697_DSC_panel_cmd_init();
}


