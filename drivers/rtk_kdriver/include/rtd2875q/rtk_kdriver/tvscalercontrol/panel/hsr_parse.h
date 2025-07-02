#ifndef _HSR_PARSE_
#define _HSR_PARSE_
#include <scaler/scalerCommon.h>

#define TCON_SETTING_MAX 128

#define PMIC_BIN_MAX_SIZE 128

#define I2C_CMD_MAX_LEN 16

#define MAX_LINE 200

#define MAX_SECTION 100

#define MAX_NAME 100

#define PMIC_NAME_LEN  12

#define HSR_I2C_OPERATOR 5

#define DLG_PRINT_PARSE(fmt, args...) \
    do              \
    {               \
        rtd_pr_vbe_notice("[DLG][%s %d]"fmt, __FUNCTION__, __LINE__, ##args);\
    }while(0)


typedef enum
{
    HSR_NOMAL,
    HSR_4k1k_120,
    HSR_4k1k_144,
    HSR_4k2k_144,
    HSR_4k1k_240,
    HSR_4k1k_288,
    HSR_MAX,
}HSR_MODE_TYPE;

typedef struct i2c_data
{
    unsigned char i2c_group_id;
    unsigned char i2c_addr;
    unsigned char i2c_cmd[I2C_CMD_MAX_LEN];
    unsigned char i2c_len;
}I2C_DATA;

typedef struct hsr_i2c_info
{
    I2C_DATA i2c_data[HSR_I2C_OPERATOR];
    unsigned char operator;
}HSR_I2C_INFO;

typedef struct hsr_switch_panel_mode_i2c
{
    HSR_I2C_INFO i2c_info[HSR_MAX];
    unsigned char wp_enable;
    unsigned int wp_hsr_before_write_data_delay;
    unsigned int wp_hsr_after_write_data_delay;
}HSR_SWITCH_PANEL_MODE_I2C;

typedef struct hsr_switch_panel_mode_gpio
{
    unsigned char high[HSR_MAX];
}HSR_SWITCH_PANEL_MODE_GPIO;

typedef struct hsr_vrr_mode
{
    unsigned char type;
    unsigned char i2c_group_id;
    unsigned char i2c_addr;
    unsigned char wp_enable;
    unsigned int wp_vrr_before_mute_delay;
    unsigned int wp_vrr_before_on_off_delay;
    unsigned int wp_vrr_before_unmute_delay;
    unsigned int wp_vrr_after_unmute_delay;
    unsigned char i2c_cmd_mute[I2C_CMD_MAX_LEN];
    unsigned char i2c_len_mute;
    unsigned char i2c_cmd_vrr_on[I2C_CMD_MAX_LEN];
    unsigned char i2c_len_vrr_on;
    unsigned char i2c_cmd_vrr_off[I2C_CMD_MAX_LEN];
    unsigned char i2c_len_vrr_off;
    unsigned char i2c_cmd_unmute[I2C_CMD_MAX_LEN];
    unsigned char i2c_len_unmute;
}HSR_VRR_MODE;

typedef struct hsr_panel_parameter
{
    unsigned int refresh;
    unsigned int typical;
    unsigned int htotal;
    unsigned int vtotal;
    unsigned int den_hstart;
    unsigned int den_hend;
    unsigned int den_vstart;
    unsigned int den_vend;
    unsigned int act_hstart;
    unsigned int act_hend;
    unsigned int act_vstart;
    unsigned int act_vend;
    unsigned int min_vtotal;
    unsigned int max_vtotal;
    unsigned int hsync_last_line;
}HSR_PANEL_PARAMETER;

typedef struct hsr_panel_parameter_info
{
   HSR_PANEL_PARAMETER panel_parameter[HSR_MAX];
}HSR_PANEL_PARAMETER_INFO;

typedef struct _Tcon_Setting_Header {
       unsigned int Offset;
       unsigned int endCode;
}TCON_SETTING_HEADER;

typedef struct _Tcon_Setting_Data{
       unsigned int type;//0:rtd_outl,1<A1><EA>ortd_maskl,2:rtd_inl,3:delay
       unsigned int addr;
       unsigned int value1;
       unsigned int value2;
}TCON_SETTING_DATA;
typedef struct hsr_tcon_data
{
    TCON_SETTING_DATA tcon[TCON_SETTING_MAX];
    unsigned int len;
}HSR_TCON_DATA;

typedef struct hsr_tcon_setting
{
    HSR_TCON_DATA tcon_data[HSR_MAX];
}HSR_TCON_SETTING;

typedef struct hsr_pmic_data
{
    unsigned char pmic_bin[PMIC_BIN_MAX_SIZE];
    unsigned int len;
}HSR_PMIC_DATA;

typedef struct hsr_pmic_setting
{
    HSR_PMIC_DATA pmic_data[HSR_MAX];
    char pmic_name[PMIC_NAME_LEN];
    unsigned char i2c_group_id;
    unsigned char i2c_addr;
    unsigned char reg_addr;
}HSR_PMIC_SETTING;

typedef struct pmic_data_info
{
    unsigned char u8_reg;
    unsigned char data[PMIC_BIN_MAX_SIZE];
}PMIC_DATA_INFO;

int hsr_parse_tcon_setting(unsigned char *src, int len, unsigned int type);
int hsr_parse_ini(unsigned char *src, int len, unsigned int type);
int hsr_parse_pmic_setting(unsigned char *src, int len, unsigned int type);
const HSR_TCON_SETTING *hsr_get_tcon_setting_by_type(HSR_MODE_TYPE hsrModeType);
const HSR_PMIC_SETTING *hsr_get_pmic_setting_by_type(HSR_MODE_TYPE hsrModeType);
const HSR_SWITCH_PANEL_MODE_I2C *hsr_get_switch_panel_info_i2c(void);
const HSR_VRR_MODE *hsr_get_vrr_info(void);
const HSR_PANEL_PARAMETER_INFO *hsr_get_panel_parameter_info(void);
const HSR_SWITCH_PANEL_MODE_GPIO *hsr_get_switch_panel_info_gpio(void);
unsigned int hsr_get_switch_panel_type(void);

#endif
