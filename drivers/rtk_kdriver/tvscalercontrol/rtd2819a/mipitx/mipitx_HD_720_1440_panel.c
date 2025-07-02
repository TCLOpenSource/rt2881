#include <tvscalercontrol/mipitx/mipitx.h>
#include <tvscalercontrol/mipitx/mipitx_dphy.h>
#include <tvscalercontrol/mipitx/mipitx_dsi.h>
#include <tvscalercontrol/io/ioregdrv.h>
#include <rtk_kdriver/io.h>


void mipitx_HD_720_1440_panel_config(void)
{   // hd 720*1440 
    default_mipitx_parameter.is_cmd_panel = 0;
    default_mipitx_parameter.lane_cnt = SINGLE_PANEL_FOUR_LANE;
    default_mipitx_parameter.color_depth = RGB_8BIT_MODE;
    default_mipitx_parameter.color_bits = RGB_24BITS;
    default_mipitx_parameter.pixelclk = 81000;
    default_mipitx_parameter.hwidth = 720;
    default_mipitx_parameter.vlength = 1440;
    default_mipitx_parameter.htotal = 900;
    default_mipitx_parameter.vtotal = 1500;
    default_mipitx_parameter.hsyncwidth = 16;
    default_mipitx_parameter.vsyncwidth = 4;
    default_mipitx_parameter.hfrontporch = 90;
    default_mipitx_parameter.hbackporch = 74;
    default_mipitx_parameter.vfrontporch = 29;
    default_mipitx_parameter.vbackporch = 27;
    default_mipitx_parameter.hpolarity = 1;
    default_mipitx_parameter.vpolarity = 1;
    default_mipitx_parameter.dsc_en = 0;
    default_mipitx_parameter.dsc_pps = NULL;
}

void mipitx_HD_720_1440_sfg_config(void)
{

}

void mipitx_HD_720_1440_panel_cmd_init(void)
{
#ifndef MIPITX_BRINGUP_SCRIPT
    UINT8 cmd0[] = {MIPITX_DSI_GENERIC_LONG_WRITE,0xff,0x98,0x81,0x03};
    UINT8 cmd1[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x01,0x00};
    UINT8 cmd2[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x02,0x00};
    UINT8 cmd3[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x03,0x53};
    UINT8 cmd4[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x04,0x13};
    UINT8 cmd5[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x05,0x00};
    UINT8 cmd6[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x06,0x04};
    UINT8 cmd7[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x07,0x00};
    UINT8 cmd8[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x08,0x00};
    UINT8 cmd9[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x09,0x21};
    UINT8 cmd10[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x0a,0x21};
    UINT8 cmd11[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x0b,0x00};
    UINT8 cmd12[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x0c,0x01};
    UINT8 cmd13[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x0d,0x00};
    UINT8 cmd14[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x0e,0x00};
    UINT8 cmd15[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x0f,0x21};
    UINT8 cmd16[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x10,0x21};
    UINT8 cmd17[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x11,0x00};
    UINT8 cmd18[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x12,0x00};
    UINT8 cmd19[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x13,0x00};
    UINT8 cmd20[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x14,0x00};
    UINT8 cmd21[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x15,0x00};
    UINT8 cmd22[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x16,0x00};
    UINT8 cmd23[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x17,0x00};
    UINT8 cmd24[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x18,0x00};
    UINT8 cmd25[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x19,0x00};
    UINT8 cmd26[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x1a,0x00};
    UINT8 cmd27[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x1b,0x00};
    UINT8 cmd28[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x1c,0x00};
    UINT8 cmd29[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x1d,0x00};
    UINT8 cmd30[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x1e,0x40};
    UINT8 cmd31[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x1f,0x80};
    UINT8 cmd32[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x20,0x02};
    UINT8 cmd33[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x21,0x03};
    UINT8 cmd34[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x22,0x00};
    UINT8 cmd35[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x23,0x00};
    UINT8 cmd36[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x24,0x00};
    UINT8 cmd37[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x25,0x00};
    UINT8 cmd38[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x26,0x00};
    UINT8 cmd39[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x27,0x00};
    UINT8 cmd40[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x28,0x33};
    UINT8 cmd41[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x29,0x03};
    UINT8 cmd42[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x2a,0x00};
    UINT8 cmd43[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x2b,0x00};
    UINT8 cmd44[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x2c,0x00};
    UINT8 cmd45[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x2d,0x00};
    UINT8 cmd46[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x2e,0x00};
    UINT8 cmd47[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x2f,0x00};
    UINT8 cmd48[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x30,0x00};
    UINT8 cmd49[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x31,0x00};
    UINT8 cmd50[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x32,0x00};
    UINT8 cmd51[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x33,0x00};
    UINT8 cmd52[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x34,0x04};
    UINT8 cmd53[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x35,0x00};
    UINT8 cmd54[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x36,0x00};
    UINT8 cmd55[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x37,0x00};
    UINT8 cmd56[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x38,0x3c};
    UINT8 cmd57[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x39,0x00};
    UINT8 cmd58[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x3a,0x40};
    UINT8 cmd59[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x3b,0x40};
    UINT8 cmd60[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x3c,0x00};
    UINT8 cmd61[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x3d,0x00};
    UINT8 cmd62[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x3e,0x00};
    UINT8 cmd63[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x3f,0x00};
    UINT8 cmd64[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x40,0x00};
    UINT8 cmd65[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x41,0x00};
    UINT8 cmd66[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x42,0x00};
    UINT8 cmd67[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x43,0x00};
    UINT8 cmd68[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x44,0x00};
    UINT8 cmd69[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x50,0x01};
    UINT8 cmd70[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x51,0x23};
    UINT8 cmd71[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x52,0x45};
    UINT8 cmd72[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x53,0x67};
    UINT8 cmd73[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x54,0x89};
    UINT8 cmd74[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x55,0xab};
    UINT8 cmd75[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x56,0x01};
    UINT8 cmd76[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x57,0x23};
    UINT8 cmd77[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x58,0x45};
    UINT8 cmd78[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x59,0x67};
    UINT8 cmd79[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x5a,0x89};
    UINT8 cmd80[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x5b,0xab};
    UINT8 cmd81[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x5c,0xcd};
    UINT8 cmd82[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x5d,0xef};
    UINT8 cmd83[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x5e,0x11};
    UINT8 cmd84[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x5f,0x01};
    UINT8 cmd85[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x60,0x00};
    UINT8 cmd86[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x61,0x15};
    UINT8 cmd87[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x62,0x14};
    UINT8 cmd88[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x63,0x0c};
    UINT8 cmd89[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x64,0x0d};
    UINT8 cmd90[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x65,0x0e};
    UINT8 cmd91[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x66,0x0f};
    UINT8 cmd92[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x67,0x06};
    UINT8 cmd93[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x68,0x02};
    UINT8 cmd94[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x69,0x02};
    UINT8 cmd95[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x6a,0x02};
    UINT8 cmd96[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x6b,0x02};
    UINT8 cmd97[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x6c,0x02};
    UINT8 cmd98[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x6d,0x02};
    UINT8 cmd99[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x6e,0x08};
    UINT8 cmd100[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x6f,0x02};
    UINT8 cmd101[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x70,0x02};
    UINT8 cmd102[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x71,0x02};
    UINT8 cmd103[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x72,0x02};
    UINT8 cmd104[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x73,0x02};
    UINT8 cmd105[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x74,0x02};
    UINT8 cmd106[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x75,0x01};
    UINT8 cmd107[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x76,0x00};
    UINT8 cmd108[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x77,0x15};
    UINT8 cmd109[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x78,0x14};
    UINT8 cmd110[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x79,0x0c};
    UINT8 cmd111[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x7a,0x0f};
    UINT8 cmd112[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x7b,0x0e};
    UINT8 cmd113[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x7c,0x0f};
    UINT8 cmd114[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x7d,0x08};
    UINT8 cmd115[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x7e,0x02};
    UINT8 cmd116[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x7f,0x02};
    UINT8 cmd117[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x80,0x02};
    UINT8 cmd118[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x81,0x02};
    UINT8 cmd119[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x82,0x02};
    UINT8 cmd120[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x83,0x02};
    UINT8 cmd121[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x84,0x06};
    UINT8 cmd122[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x85,0x02};
    UINT8 cmd123[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x86,0x02};
    UINT8 cmd124[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x87,0x02};
    UINT8 cmd125[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x88,0x02};
    UINT8 cmd126[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x89,0x02};
    UINT8 cmd127[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x8a,0x02};
    UINT8 cmd128[] = {MIPITX_DSI_GENERIC_LONG_WRITE,0xff,0x98,0x81,0x04};
    UINT8 cmd129[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x6c,0x15};
     UINT8 cmd130[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x6e,0x2b};
    UINT8 cmd131[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x6f,0x35};
    UINT8 cmd132[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x35,0x1f};
    UINT8 cmd133[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x33,0x14};
    UINT8 cmd134[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x3a,0x24};
    UINT8 cmd135[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x8d,0x1a};
    UINT8 cmd136[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x87,0xba};
    UINT8 cmd137[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x26,0x76};
    UINT8 cmd138[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xb2,0xd1};
    UINT8 cmd139[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xb5,0x06};
    UINT8 cmd140[] = {MIPITX_DSI_GENERIC_LONG_WRITE,0xff,0x98,0x81,0x01};
    UINT8 cmd141[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x22,0x0a};
    UINT8 cmd142[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x31,0x00};
    UINT8 cmd143[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x53,0x8c};
    UINT8 cmd144[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x55,0x8c};
    UINT8 cmd145[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x50,0xc7};
    UINT8 cmd146[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x51,0xc4};
    UINT8 cmd147[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x60,0x1c};
    UINT8 cmd148[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x62,0x00};
    UINT8 cmd149[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x63,0x00};
    UINT8 cmd150[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0x2e,0xf0};
    UINT8 cmd151[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xa0,0x00};
    UINT8 cmd152[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xa1,0x26};
    UINT8 cmd153[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xa2,0x34};
    UINT8 cmd154[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xa3,0x14};
    UINT8 cmd155[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xa4,0x17};
    UINT8 cmd156[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xa5,0x2a};
    UINT8 cmd157[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xa6,0x1d};
    UINT8 cmd158[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xa7,0x1e};
    UINT8 cmd159[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xa8,0x8b};
    UINT8 cmd160[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xa9,0x1b};
    UINT8 cmd161[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xaa,0x27};
    UINT8 cmd162[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xab,0x72};
    UINT8 cmd163[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xac,0x1d};
    UINT8 cmd164[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xad,0x1d};
    UINT8 cmd165[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xae,0x51};
    UINT8 cmd166[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xaf,0x26};
    UINT8 cmd167[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xb0,0x2b};
    UINT8 cmd168[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xb1,0x49};
    UINT8 cmd169[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xb2,0x58};
    UINT8 cmd170[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xb3,0x26};
    UINT8 cmd171[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xc0,0x00};
    UINT8 cmd172[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xc1,0x26};
    UINT8 cmd173[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xc2,0x34};
    UINT8 cmd174[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xc3,0x14};
    UINT8 cmd175[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xc4,0x17};
    UINT8 cmd176[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xc5,0x2a};
    UINT8 cmd177[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xc6,0x1d};
    UINT8 cmd178[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xc7,0x1e};
    UINT8 cmd179[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xc8,0x8b};
    UINT8 cmd180[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xc9,0x1b};
    UINT8 cmd181[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xca,0x27};
    UINT8 cmd182[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xcb,0x72};
    UINT8 cmd183[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xcc,0x1d};
    UINT8 cmd184[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xcd,0x1d};
    UINT8 cmd185[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xce,0x51};
    UINT8 cmd186[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xcf,0x26};
    UINT8 cmd187[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xd0,0x26};
    UINT8 cmd188[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xd1,0x49};
    UINT8 cmd189[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xd2,0x58};
    UINT8 cmd190[] = {MIPITX_DSI_DCS_SHORT_WRITE_PARAM,0xd3,0x26};
    UINT8 cmd191[] = {MIPITX_DSI_GENERIC_LONG_WRITE,0xff,0x98,0x81,0x00};
    UINT8 cmd192[] = {MIPITX_DSI_DCS_SHORT_WRITE,0x35,0x00};
    UINT8 cmd193[] = {MIPITX_DSI_DCS_SHORT_WRITE,0x11,0x00};
    UINT8 cmd194[] = {MIPITX_DSI_DCS_SHORT_WRITE,0x29,0x00};

    UINT8 channel = 0;
    
    /* Send CMD */
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd0, sizeof(cmd0));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd1, sizeof(cmd1));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd2, sizeof(cmd2));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd3, sizeof(cmd3));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd4, sizeof(cmd4));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd5, sizeof(cmd5));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd6, sizeof(cmd6));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd7, sizeof(cmd7));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd8, sizeof(cmd8));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd9, sizeof(cmd9));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd10, sizeof(cmd10));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd11, sizeof(cmd11));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd12, sizeof(cmd12));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd13, sizeof(cmd13));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd14, sizeof(cmd14));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd15, sizeof(cmd15));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd16, sizeof(cmd16));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd17, sizeof(cmd17));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd18, sizeof(cmd18));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd19, sizeof(cmd19));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd20, sizeof(cmd20));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd21, sizeof(cmd21));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd22, sizeof(cmd22));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd23, sizeof(cmd23));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd24, sizeof(cmd24));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd25, sizeof(cmd25));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd26, sizeof(cmd26));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd27, sizeof(cmd27));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd28, sizeof(cmd28));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd29, sizeof(cmd29));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd30, sizeof(cmd30));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd31, sizeof(cmd31));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd32, sizeof(cmd32));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd33, sizeof(cmd33));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd34, sizeof(cmd34));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd35, sizeof(cmd35));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd36, sizeof(cmd36));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd37, sizeof(cmd37));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd38, sizeof(cmd38));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd39, sizeof(cmd39));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd40, sizeof(cmd40));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd41, sizeof(cmd41));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd42, sizeof(cmd42));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd43, sizeof(cmd43));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd44, sizeof(cmd44));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd45, sizeof(cmd45));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd46, sizeof(cmd46));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd47, sizeof(cmd47));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd48, sizeof(cmd48));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd49, sizeof(cmd49));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd50, sizeof(cmd50));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd51, sizeof(cmd51));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd52, sizeof(cmd52));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd53, sizeof(cmd53));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd54, sizeof(cmd54));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd55, sizeof(cmd55));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd56, sizeof(cmd56));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd57, sizeof(cmd57));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd58, sizeof(cmd58));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd59, sizeof(cmd59));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd60, sizeof(cmd60));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd61, sizeof(cmd61));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd62, sizeof(cmd62));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd63, sizeof(cmd63));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd64, sizeof(cmd64));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd65, sizeof(cmd65));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd66, sizeof(cmd66));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd67, sizeof(cmd67));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd68, sizeof(cmd68));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd69, sizeof(cmd69));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd70, sizeof(cmd70));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd71, sizeof(cmd71));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd72, sizeof(cmd72));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd73, sizeof(cmd73));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd74, sizeof(cmd74));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd75, sizeof(cmd75));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd76, sizeof(cmd76));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd77, sizeof(cmd77));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd78, sizeof(cmd78));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd79, sizeof(cmd79));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd80, sizeof(cmd80));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd81, sizeof(cmd81));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd82, sizeof(cmd82));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd83, sizeof(cmd83));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd84, sizeof(cmd84));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd85, sizeof(cmd85));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd86, sizeof(cmd86));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd87, sizeof(cmd87));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd88, sizeof(cmd88));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd89, sizeof(cmd89));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd90, sizeof(cmd90));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd91, sizeof(cmd91));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd92, sizeof(cmd92));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd93, sizeof(cmd93));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd94, sizeof(cmd94));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd95, sizeof(cmd95));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd96, sizeof(cmd96));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd97, sizeof(cmd97));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd98, sizeof(cmd98));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd99, sizeof(cmd99));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd100, sizeof(cmd100));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd101, sizeof(cmd101));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd102, sizeof(cmd102));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd103, sizeof(cmd103));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd104, sizeof(cmd104));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd105, sizeof(cmd105));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd106, sizeof(cmd106));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd107, sizeof(cmd107));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd108, sizeof(cmd108));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd109, sizeof(cmd109));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd110, sizeof(cmd110));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd111, sizeof(cmd111));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd112, sizeof(cmd112));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd113, sizeof(cmd113));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd114, sizeof(cmd114));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd115, sizeof(cmd115));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd116, sizeof(cmd116));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd117, sizeof(cmd117));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd118, sizeof(cmd118));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd119, sizeof(cmd119));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd120, sizeof(cmd120));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd121, sizeof(cmd121));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd122, sizeof(cmd122));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd123, sizeof(cmd123));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd124, sizeof(cmd124));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd125, sizeof(cmd125));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd126, sizeof(cmd126));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd127, sizeof(cmd127));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd128, sizeof(cmd128));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd129, sizeof(cmd129));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd130, sizeof(cmd130));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd131, sizeof(cmd131));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd132, sizeof(cmd132));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd133, sizeof(cmd133));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd134, sizeof(cmd134));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd135, sizeof(cmd135));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd136, sizeof(cmd136));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd137, sizeof(cmd137));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd138, sizeof(cmd138));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd139, sizeof(cmd139));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd140, sizeof(cmd140));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd141, sizeof(cmd141));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd142, sizeof(cmd142));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd143, sizeof(cmd143));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd144, sizeof(cmd144));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd145, sizeof(cmd145));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd146, sizeof(cmd146));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd147, sizeof(cmd147));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd148, sizeof(cmd148));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd149, sizeof(cmd149));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd150, sizeof(cmd150));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd151, sizeof(cmd151));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd152, sizeof(cmd152));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd153, sizeof(cmd153));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd154, sizeof(cmd154));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd155, sizeof(cmd155));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd156, sizeof(cmd156));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd157, sizeof(cmd157));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd158, sizeof(cmd158));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd159, sizeof(cmd159));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd160, sizeof(cmd160));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd161, sizeof(cmd161));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd162, sizeof(cmd162));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd163, sizeof(cmd163));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd164, sizeof(cmd164));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd165, sizeof(cmd165));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd166, sizeof(cmd166));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd167, sizeof(cmd167));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd168, sizeof(cmd168));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd169, sizeof(cmd169));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd170, sizeof(cmd170));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd171, sizeof(cmd171));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd172, sizeof(cmd172));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd173, sizeof(cmd173));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd174, sizeof(cmd174));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd175, sizeof(cmd175));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd176, sizeof(cmd176));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd177, sizeof(cmd177));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd178, sizeof(cmd178));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd179, sizeof(cmd179));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd180, sizeof(cmd180));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd181, sizeof(cmd181));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd182, sizeof(cmd182));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd183, sizeof(cmd183));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd184, sizeof(cmd184));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd185, sizeof(cmd185));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd186, sizeof(cmd186));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd187, sizeof(cmd187));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd188, sizeof(cmd188));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd189, sizeof(cmd189));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd190, sizeof(cmd190));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd191, sizeof(cmd191));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd192, sizeof(cmd192));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd193, sizeof(cmd193));
    mipitx_dsi_msg_send(channel, VBK_PACKET_MSG_EVENT, cmd194, sizeof(cmd194));

#else

    IoReg_Write32(0x1806A800, 0x00410201);// CFG State
    IoReg_Write32(0x1806A81C, 0x02060290);// 
    IoReg_Write32(0x1806A830, 0xFF000429);// 
    IoReg_Write32(0x1806A834, 0x00038198);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00000115);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00000215);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00530315);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00130415);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00000515);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00040615);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00000715);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00000815);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00210915);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00210A15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00000B15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00010C15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00000D15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00000E15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00210F15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00211015);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00001115);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00001215);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00001315);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00001415);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00001515);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00001615);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00001715);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00001815);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00001915);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00001A15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00001B15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00001C15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00001D15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00401E15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00801F15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00022015);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00032115);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00002215);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00002315);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00002415);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00002515);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00002615);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00002715);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00332815);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00032915);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00002A15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00002B15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00002C15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00002D15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00002E15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00002F15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00003015);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00003115);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00003215);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00003315);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00043415);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00003515);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00003615);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00003715);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x003C3815);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00003915);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00403A15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00403B15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00003C15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00003D15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00003E15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00003F15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00004015);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00004115);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00004215);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00004315);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00004415);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00015015);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00235115);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00455215);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00675315);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00895415);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00AB5515);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00015615);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00235715);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00455815);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00675915);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00895A15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00AB5B15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00CD5C15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00EF5D15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00115E15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00015F15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00006015);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00156115);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00146215);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x000C6315);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x000D6415);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x000E6515);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x000F6615);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00066715);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00026815);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00026915);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00026A15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00026B15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00026C15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00026D15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00086E15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00026F15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00027015);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00027115);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00027215);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00027315);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00027415);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00017515);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00007615);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00157715);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00147815);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x000C7915);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x000F7A15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x000E7B15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x000F7C15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00087D15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00027E15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00027F15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00028015);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00028115);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00028215);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00028315);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00068415);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00028515);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00028615);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00028715);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00028815);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00028915);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00028A15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02060290);// 
    IoReg_Write32(0x1806A830, 0xFF000429);// 
    IoReg_Write32(0x1806A834, 0x00048198);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00156C15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x002B6E15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00356F15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x001F3515);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00143315);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00243A15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x001A8D15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00BA8715);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00762615);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00D1B215);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0006B515);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02060290);// 
    IoReg_Write32(0x1806A830, 0xFF000429);// 
    IoReg_Write32(0x1806A834, 0x00018198);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x000A2215);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00003115);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x008C5315);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x008C5515);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00C75015);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00C45115);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x001C6015);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00006215);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00006315);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00F02E15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0000A015);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0026A115);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0034A215);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0014A315);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0017A415);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x002AA515);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x001DA615);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x001EA715);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x008BA815);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x001BA915);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0027AA15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0072AB15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x001DAC15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x001DAD15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0051AE15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0026AF15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x002BB015);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0049B115);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0058B215);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0026B315);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0000C015);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0026C115);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0034C215);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0014C315);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0017C415);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x002AC515);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x001DC615);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x001EC715);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x008BC815);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x001BC915);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0027CA15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0072CB15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x001DCC15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x001DCD15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0051CE15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0026CF15);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0026D015);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0049D115);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0058D215);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x0026D315);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02060290);// 
    IoReg_Write32(0x1806A830, 0xFF000429);// 
    IoReg_Write32(0x1806A834, 0x00008198);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00003505);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00001105);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A81C, 0x02020290);// 
    IoReg_Write32(0x1806A830, 0x00002905);// 
    rtd_part_outl(0x1806A800, 26, 26, 0x1);	// LPDT Apply
    IoReg_Write32(0x1806A800, 0x00410203);// 
#endif
}

void mipitx_HD_720_1440_panel(void)
{
// hd 720*1440
//DTG DTG dv_den_sta=0x1E, dv_den_end=0x5BE, dh_den_sta=0x2D, dh_den_end=0x195, dhtot=0x1C1, dvtot=0x5DB, hs_width=0xf, vs_width=0x3

    // 1. config mipi panel parameters
    mipitx_HD_720_1440_panel_config();

    // 2. mipitx dphy init
    mipitx_dphy_init();

    // 3. mipitx dsi setting config
    mipitx_dsi_config();

    // 4. sfg setting
    mipitx_HD_720_1440_sfg_config();

    // 5. commad mode init
    mipitx_HD_720_1440_panel_cmd_init();
}