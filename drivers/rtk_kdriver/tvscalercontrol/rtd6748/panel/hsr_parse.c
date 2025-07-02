#include <generated/autoconf.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/string.h>
#include <rtd_log/rtd_module_log.h>
#include <tvscalercontrol/panel/hsr_parse.h>
#include <ioctrl/scaler/vsc_cmd_id.h>

static int ini_parse(char *filestart, unsigned int filelen,
    int (*handler)(void *, char *, char *, char *),	void *user);

static int ini_parse_handler(void *user, char *section, char *name, char *value);
static int ini_parse_wp_info(void *user, char *section, char *name, char *value);
static HSR_TCON_SETTING s_tconSetting;
static HSR_PMIC_SETTING s_pmic_info;
static HSR_EYE_DIAGRAMS_SETTING s_eyeDiagrams;
static unsigned int s_hsr_switch_panel_type = 0;
static HSR_SWITCH_PANEL_MODE_I2C s_hsr_switch_panel_info_i2c;
static HSR_SWITCH_PANEL_MODE_GPIO s_hsr_switch_panel_info_gpio;

static HSR_VRR_MODE s_hsr_vrr_info;
static HSR_PANEL_PARAMETER_INFO s_hsr_panel_parameter_info;

unsigned int hsr_get_switch_panel_type(void)
{
    return s_hsr_switch_panel_type;
}

const HSR_SWITCH_PANEL_MODE_I2C *hsr_get_switch_panel_info_i2c(void)
{
    return &s_hsr_switch_panel_info_i2c;
}

const HSR_SWITCH_PANEL_MODE_GPIO *hsr_get_switch_panel_info_gpio(void)
{
    return &s_hsr_switch_panel_info_gpio;
}

const HSR_VRR_MODE *hsr_get_vrr_info(void)
{
    return &s_hsr_vrr_info;
}

const HSR_PANEL_PARAMETER_INFO *hsr_get_panel_parameter_info(void)
{
    return &s_hsr_panel_parameter_info;
}

const HSR_TCON_SETTING *hsr_get_tcon_setting_by_type(HSR_MODE_TYPE hsrModeType)
{
    return &s_tconSetting;
}

const HSR_EYE_DIAGRAMS_SETTING *hsr_get_diagrams_setting_by_type(HSR_MODE_TYPE hsrModeType)
{
    return &s_eyeDiagrams;
}

const HSR_PMIC_SETTING *hsr_get_pmic_setting_by_type(HSR_MODE_TYPE hsrModeType)
{
    return &s_pmic_info;
}

int hsr_parse_pmic_setting(unsigned char *src, int len, unsigned int type)
{
    HSR_MODE_TYPE hsrIndex = HSR_NOMAL;

    if(src == NULL || len == 0)
    {
        return -1;
    }

    switch(type)
    {
        case OPERATE_PMIC_TYPE:
            hsrIndex = HSR_NOMAL;
            break;

        case OPERATE_HSR_PMIC_TYPE:
            hsrIndex = HSR_4k1k_120;
            break;

        case OPERATE_HSR_PMIC_4k1k144_TYPE:
            hsrIndex = HSR_4k1k_144;
            break;

        case OPERATE_HSR_PMIC_4k2k144_TYPE:
            hsrIndex = HSR_4k2k_144;
            break;

        case OPERATE_HSR_PMIC_4k1k240_TYPE:
            hsrIndex = HSR_4k1k_240;
            break;

        case OPERATE_HSR_PMIC_4k1k288_TYPE:
            hsrIndex = HSR_4k1k_288;
            break;

        default:
            break;
    }

    if(len <= PMIC_BIN_MAX_SIZE)
    {
        memcpy(s_pmic_info.pmic_data[hsrIndex].pmic_bin, src, len);

        s_pmic_info.pmic_data[hsrIndex].len = len;

        DLG_PRINT_PARSE("s_pmic_info.pmic_data[%d].len:%d\n", hsrIndex, s_pmic_info.pmic_data[hsrIndex].len);
    }
    else
    {
        DLG_PRINT_PARSE("Error pmic parse error\n");
        return -1;
    }

    return 0;
}

int hsr_parse_eye_diagrams_setting(unsigned char *src, int len, unsigned int type)
{
    BIN_SETTING_HEADER header = {0};
    BIN_SETTING_DATA Data = {0};
    int i = 0;
    int index = 0;
    HSR_MODE_TYPE hsrIndex = HSR_NOMAL;

    if(src == NULL || len == 0)
    {
        return -1;
    }

    switch(type)
    {
        case OPERATE_HSR_EYE_DEFAULT_TYPE:
            hsrIndex = HSR_NOMAL;
            break;

        case OPERATE_HSR_EYE_4k2k60_TYPE:
            hsrIndex = HSR_4k2k_60;
            break;

        case OPERATE_HSR_EYE_4k1k120_TYPE:
            hsrIndex = HSR_4k1k_120;
            break;

        case OPERATE_HSR_EYE_4k1k144_TYPE:
            hsrIndex = HSR_4k1k_144;
            break;

        case OPERATE_HSR_EYE_4k2k120_TYPE:
            hsrIndex = HSR_4k2k_120;
            break;

        case OPERATE_HSR_EYE_4k2k144_TYPE:
            hsrIndex = HSR_4k2k_144;
            break;

        case OPERATE_HSR_EYE_4k1k240_TYPE:
            hsrIndex = HSR_4k1k_240;
            break;

        case OPERATE_HSR_EYE_4k1k288_TYPE:
            hsrIndex = HSR_4k1k_288;
            break;

        case OPERATE_HSR_EYE_4k1k300_TYPE:
            hsrIndex = HSR_4k1k_300;
            break;

        case OPERATE_HSR_EYE_4k1k330_TYPE:
            hsrIndex = HSR_4k1k_330;
            break;

        default:
            break;
    }

    while(i < len)
    {
        if(i < sizeof(BIN_SETTING_HEADER))
        {
            if((i+8)<=len)
            {
                header.Offset = *src<<24 | *(src+1)<<16 | *(src+2)<<8 | *(src+3);
                header.endCode = *(src+4)<<24 | *(src+5)<<16 | *(src+6)<<8 | *(src+7);

                if(header.Offset != sizeof(BIN_SETTING_HEADER) || header.endCode != 0xffffffff)
                    return 0;

                i += sizeof(BIN_SETTING_HEADER);
                src += sizeof(BIN_SETTING_HEADER);;
            }
            else
                break;

        }
        else
        {
            if((i+sizeof(BIN_SETTING_DATA))<=len)
            {
                Data.type = *src<<24 | *(src+1)<<16 | *(src+2)<<8 | *(src+3);
                Data.addr = *(src+4)<<24 | *(src+5)<<16 | *(src+6)<<8 | *(src+7);
                Data.value1 = *(src+8)<<24 | *(src+9)<<16 | *(src+10)<<8 | *(src+11);
                Data.value2 = *(src+12)<<24 | *(src+13)<<16 | *(src+14)<<8 | *(src+15);

                DLG_PRINT_PARSE("Data.type:%d Data.addr:0x%08x Data.value1:0x%08x Data.value2:0x%08x\n", Data.type,Data.addr,Data.value1,Data.value2);

                if(index < EYE_DIAGRAMS_MAX)
                {
                    s_eyeDiagrams.eye_diagrams_data[hsrIndex].eye_digrams[index] = Data;
                    s_eyeDiagrams.eye_diagrams_data[hsrIndex].len += sizeof(BIN_SETTING_DATA);
                }

                index++;
                i += sizeof(BIN_SETTING_DATA);
                src += sizeof(BIN_SETTING_DATA);
            }
            else
            {
                break;
            }
        }
    }

    DLG_PRINT_PARSE("s_eyeDiagrams.eye_diagrams_data[%d].len:%d\n", hsrIndex, s_eyeDiagrams.eye_diagrams_data[hsrIndex].len);

    return 0;
}

int hsr_parse_tcon_setting(unsigned char *src, int len, unsigned int type)
{
    BIN_SETTING_HEADER header = {0};
    BIN_SETTING_DATA Data = {0};
    int i = 0;
    int index = 0;
    HSR_MODE_TYPE hsrIndex = HSR_NOMAL;

    if(src == NULL || len == 0)
    {
        DLG_PRINT_PARSE("[%s %d]parameter error len:%d\n", __FUNCTION__, __LINE__, len);
        return -1;
    }

    switch(type)
    {
        case OPERATE_TCON_TYPE:
            hsrIndex = HSR_NOMAL;
            break;

        case OPERATE_HSR_TCON_TYPE:
            hsrIndex = HSR_4k1k_120;
            break;

        case OPERATE_HSR_TCON_4k1k144_TYPE:
            hsrIndex = HSR_4k1k_144;
            break;

        case OPERATE_HSR_TCON_4k2k144_TYPE:
            hsrIndex = HSR_4k2k_144;
            break;

        case OPERATE_HSR_TCON_4k1k240_TYPE:
            hsrIndex = HSR_4k1k_240;
            break;

        case OPERATE_HSR_TCON_4k1k288_TYPE:
            hsrIndex = HSR_4k1k_288;
            break;

        case OPERATE_HSR_TCON_4k2k60_TYPE:
            hsrIndex = HSR_4k2k_60;
            break;

        case OPERATE_HSR_TCON_4k2k120_TYPE:
            hsrIndex = HSR_4k2k_120;
            break;

        default:
            break;
    }

    while(i < len)
    {
        if(i < sizeof(BIN_SETTING_HEADER))
        {
            if((i+8)<=len)
            {
                header.Offset = *src<<24 | *(src+1)<<16 | *(src+2)<<8 | *(src+3);
                header.endCode = *(src+4)<<24 | *(src+5)<<16 | *(src+6)<<8 | *(src+7);

                if(header.Offset != sizeof(BIN_SETTING_HEADER) || header.endCode != 0xffffffff)
                    return 0;

                i += sizeof(BIN_SETTING_HEADER);
                src += sizeof(BIN_SETTING_HEADER);;
            }
            else
                break;

        }
        else
        {
            if((i+sizeof(BIN_SETTING_DATA))<=len)
            {
                Data.type = *src<<24 | *(src+1)<<16 | *(src+2)<<8 | *(src+3);
                Data.addr = *(src+4)<<24 | *(src+5)<<16 | *(src+6)<<8 | *(src+7);
                Data.value1 = *(src+8)<<24 | *(src+9)<<16 | *(src+10)<<8 | *(src+11);
                Data.value2 = *(src+12)<<24 | *(src+13)<<16 | *(src+14)<<8 | *(src+15);

                //DLG_PRINT_PARSE("Data.type:%d Data.addr:0x%08x Data.value1:0x%08x Data.value2:0x%08x\n", Data.type,Data.addr,Data.value1,Data.value2);

                if(index < TCON_SETTING_MAX)
                {
                    s_tconSetting.tcon_data[hsrIndex].tcon[index] = Data;
                    s_tconSetting.tcon_data[hsrIndex].len += sizeof(BIN_SETTING_DATA);
                }

                index++;
                i += sizeof(BIN_SETTING_DATA);
                src += sizeof(BIN_SETTING_DATA);
            }
            else
            {
                break;
            }
        }
    }

    DLG_PRINT_PARSE("s_tconSetting.tcon_data[%d].len:%d\n", hsrIndex, s_tconSetting.tcon_data[hsrIndex].len);

    return 0;
}

int hsr_parse_ini(unsigned char *src, int len, unsigned int type)
{
    return ini_parse(src, len, ini_parse_handler, NULL);
}

/* Strip whitespace chars off end of given string, in place. Return s. */
static char *rstrip(char *s)
{
    char *p = s + strlen(s);

    while (p > s && isspace(*--p))
        *p = '\0';
    return s;
}

/* Return pointer to first non-whitespace char in given string. */
static char *lskip(const char *s)
{
    while (*s && isspace(*s))
        s++;
    return (char *)s;
}

/* Return pointer to first char c or ';' comment in given string, or pointer to
   null at end of string if neither found. ';' must be prefixed by a whitespace
   character to register as a comment. */
static char *find_char_or_comment(const char *s, char c)
{
    int was_whitespace = 0;

    while (*s && *s != c && !(was_whitespace && *s == ';')) {
        was_whitespace = isspace(*s);
        s++;
    }
    return (char *)s;
}

/* Version of strncpy that ensures dest (size bytes) is null-terminated. */
static char *strncpy0(char *dest, const char *src, size_t size)
{
    strncpy(dest, src, size);
    dest[size - 1] = '\0';
    return dest;
}

/* Emulate the behavior of fgets but on memory */
static char *memgets(char *str, int num, char **mem, size_t *memsize)
{
    char *end;
    int len;
    int newline = 1;

    end = memchr(*mem, '\n', *memsize);
    if (end == NULL) {
        if (*memsize == 0)
            return NULL;
        end = *mem + *memsize;
        newline = 0;
    }
    len = MIN((end - *mem) + newline, num);
    memcpy(str, *mem, len);
    if (len < num)
        str[len] = '\0';

    /* prepare the mem vars for the next call */
    *memsize -= (end - *mem) + newline;
    *mem += (end - *mem) + newline;

    return str;
}

/* Parse given INI-style file. May have [section]s, name=value pairs
   (whitespace stripped), and comments starting with ';' (semicolon). Section
   is "" if name=value pair parsed before any section heading. name:value
   pairs are also supported as a concession to Python's ConfigParser.

   For each name=value pair parsed, call handler function with given user
   pointer as well as section, name, and value (data only valid for duration
   of handler call). Handler should return nonzero on success, zero on error.

   Returns 0 on success, line number of first error on parse error (doesn't
   stop on first error).
*/
static int ini_parse_handler_i2c(void *user, char *section, char *name, char *value)
{
    int ret = 1;

    if((strcmp(section, "hsr_switch_panel_mode_i2c_normal") == 0) || (strcmp(section, "hsr_switch_panel_mode_i2c_120") == 0)
        || (strcmp(section, "hsr_switch_panel_mode_i2c_144") == 0) || (strcmp(section, "hsr_switch_panel_mode_i2c_240") == 0)
        || (strcmp(section, "hsr_switch_panel_mode_i2c_288") == 0) || (strcmp(section, "hsr_i2c_wp") == 0)
        || (strcmp(section, "hsr_switch_panel_mode_i2c_4k1k_144") == 0))
    {
        long result = 0;
        int i2c_index  = HSR_NOMAL;
        result = simple_strtoul(value, NULL, 0);

        if(strcmp(section, "hsr_switch_panel_mode_i2c_normal") == 0)
        {
            i2c_index = HSR_NOMAL;
        }
        else if(strcmp(section, "hsr_switch_panel_mode_i2c_120") == 0)
        {
            i2c_index = HSR_4k1k_120;
        }
        else if(strcmp(section, "hsr_switch_panel_mode_i2c_4k1k_144") == 0)
        {
            i2c_index = HSR_4k1k_144;
        }
        else if(strcmp(section, "hsr_switch_panel_mode_i2c_144") == 0)
        {
            i2c_index = HSR_4k2k_144;
        }
        else if(strcmp(section, "hsr_switch_panel_mode_i2c_240") == 0)
        {
            i2c_index = HSR_4k1k_240;
        }
        else if(strcmp(section, "hsr_switch_panel_mode_i2c_288") == 0)
        {
            i2c_index = HSR_4k1k_288;
        }

        if(strcmp(name, "i2c_group_id") == 0)
        {
            s_hsr_switch_panel_info_i2c.i2c_info[i2c_index].i2c_data[0].i2c_group_id = result;
            s_hsr_switch_panel_info_i2c.i2c_info[i2c_index].operator = 1;
        }
        else if(strcmp(name, "i2c_addr") == 0)
        {
            s_hsr_switch_panel_info_i2c.i2c_info[i2c_index].i2c_data[0].i2c_addr = result;
        }
        else if(strcmp(name, "i2c_cmd") == 0)
        {
            char *token = NULL;
            int cmd_index = 0;
            char *str = NULL;
            str = value;
            token = strsep(&str, " ");
            while(token != NULL)
            {
                if(cmd_index < I2C_CMD_MAX_LEN)
                {
                    s_hsr_switch_panel_info_i2c.i2c_info[i2c_index].i2c_data[0].i2c_cmd[cmd_index] = simple_strtoul(token, NULL, 0);
                }

                token = strsep(&str, " ");

                cmd_index++;
            }
        }
        else if(strcmp(name, "i2c_len") == 0)
        {
            s_hsr_switch_panel_info_i2c.i2c_info[i2c_index].i2c_data[0].i2c_len = result;
        }
        else if(strcmp(name, "i2c_group_id_1") == 0)
        {
            s_hsr_switch_panel_info_i2c.i2c_info[i2c_index].i2c_data[1].i2c_group_id = result;
            s_hsr_switch_panel_info_i2c.i2c_info[i2c_index].operator = 2;
        }
        else if(strcmp(name, "i2c_addr_1") == 0)
        {
            s_hsr_switch_panel_info_i2c.i2c_info[i2c_index].i2c_data[1].i2c_addr = result;
        }
        else if(strcmp(name, "i2c_cmd_1") == 0)
        {
            char *token = NULL;
            int cmd_index = 0;
            char *str = NULL;
            str = value;
            token = strsep(&str, " ");
            while(token != NULL)
            {
                if(cmd_index < I2C_CMD_MAX_LEN)
                {
                    s_hsr_switch_panel_info_i2c.i2c_info[i2c_index].i2c_data[1].i2c_cmd[cmd_index] = simple_strtoul(token, NULL, 0);
                }

                token = strsep(&str, " ");

                cmd_index++;
            }
        }
        else if(strcmp(name, "i2c_len_1") == 0)
        {
            s_hsr_switch_panel_info_i2c.i2c_info[i2c_index].i2c_data[1].i2c_len = result;
        }
        else
        {
            ret = 0;
        }
    }

    return ret;
}

static int ini_parse_handler_gpio(void *user, char *section, char *name, char *value)
{
    int ret = 1;

    if((strcmp(section, "hsr_switch_panel_mode_gpio_normal") == 0) || (strcmp(section, "hsr_switch_panel_mode_gpio_120") == 0)
            || (strcmp(section, "hsr_switch_panel_mode_gpio_144") == 0) || (strcmp(section, "hsr_switch_panel_mode_gpio_240") == 0)
            || (strcmp(section, "hsr_switch_panel_mode_gpio_288") == 0) || (strcmp(section, "hsr_switch_panel_mode_gpio_4k1k144") == 0))
    {
        long result = 0;
        int index = HSR_NOMAL;
        result = simple_strtoul(value, NULL, 0);

        if(strcmp(section, "hsr_switch_panel_mode_gpio_normal") == 0)
        {
            index = HSR_NOMAL;
        }
        else if(strcmp(section, "hsr_switch_panel_mode_gpio_120") == 0)
        {
            index = HSR_4k1k_120;
        }
        else if(strcmp(section, "hsr_switch_panel_mode_gpio_4k1k144") == 0)
        {
            index = HSR_4k1k_144;
        }
        else if(strcmp(section, "hsr_switch_panel_mode_gpio_144") == 0)
        {
            index = HSR_4k2k_144;
        }
        else if(strcmp(section, "hsr_switch_panel_mode_gpio_240") == 0)
        {
            index = HSR_4k1k_240;
        }
        else if(strcmp(section, "hsr_switch_panel_mode_gpio_288") == 0)
        {
            index = HSR_4k1k_288;
        }

        if(strcmp(name, "high") == 0)
        {
            s_hsr_switch_panel_info_gpio.high[index] = result;
        }
        else
        {
            ret = 0;
        }
    }

    return ret;
}

static int ini_parse_handler_vrr(void *user, char *section, char *name, char *value)
{
    int ret = 1;

    if(strcmp(section, "hsr_vrr_mode") == 0)
    {
        long result = 0;
        result = simple_strtoul(value, NULL, 0);
        if(strcmp(name, "type") == 0)
        {
            s_hsr_vrr_info.type = result;
        }
        else if(strcmp(name, "i2c_group_id") == 0)
        {
            s_hsr_vrr_info.i2c_group_id = result;
        }
        else if(strcmp(name, "i2c_addr") == 0)
        {
            s_hsr_vrr_info.i2c_addr = result;
        }
        else if(strcmp(name, "i2c_cmd_mute") == 0)
        {
            char *token = NULL;
            int index = 0;
            char *str = NULL;
            str = value;
            token = strsep(&str, " ");
            while(token != NULL)
            {
                if(index < I2C_CMD_MAX_LEN)
                {
                    s_hsr_vrr_info.i2c_cmd_mute[index] = simple_strtoul(token, NULL, 0);
                }
                token = strsep(&str, " ");
                index++;
            }
        }
        else if(strcmp(name, "i2c_len_mute") == 0)
        {
            s_hsr_vrr_info.i2c_len_mute = result;
        }
        else if(strcmp(name, "i2c_cmd_vrr_on") == 0)
        {
            char *token = NULL;
            int index = 0;
            char *str = NULL;
            str = value;
            token = strsep(&str, " ");
            while(token != NULL)
            {
                if(index < I2C_CMD_MAX_LEN)
                {
                    s_hsr_vrr_info.i2c_cmd_vrr_on[index] = simple_strtoul(token, NULL, 0);
                }
                token = strsep(&str, " ");
                index++;
            }
        }
        else if(strcmp(name, "i2c_len_vrr_on") == 0)
        {
            s_hsr_vrr_info.i2c_len_vrr_on = result;
        }
        else if(strcmp(name, "i2c_cmd_vrr_off") == 0)
        {
            char *token = NULL;
            int index = 0;
            char *str = NULL;
            str = value;
            token = strsep(&str, " ");
            while(token != NULL)
            {
                if(index < I2C_CMD_MAX_LEN)
                {
                    s_hsr_vrr_info.i2c_cmd_vrr_off[index] = simple_strtoul(token, NULL, 0);
                }
                token = strsep(&str, " ");
                index++;
            }
        }
        else if(strcmp(name, "i2c_len_vrr_off") == 0)
        {
            s_hsr_vrr_info.i2c_len_vrr_off = result;
        }
        else if(strcmp(name, "i2c_cmd_unmute") == 0)
        {
            char *token = NULL;
            int index = 0;
            char *str = NULL;
            str = value;
            token = strsep(&str, " ");
            while(token != NULL)
            {
                if(index < I2C_CMD_MAX_LEN)
                {
                    s_hsr_vrr_info.i2c_cmd_unmute[index] = simple_strtoul(token, NULL, 0);
                }
                token = strsep(&str, " ");
                index++;
            }
        }
        else if(strcmp(name, "i2c_len_unmute") == 0)
        {
            s_hsr_vrr_info.i2c_len_unmute = result;
        }
        else
        {
            ret = 0;
        }
    }

    return ret;
}

static int ini_parse_handler_parameter(void *user, char *section, char *name, char *value)
{
    int ret = 1;

    if((strcmp(section, "hsr_panel_parameter_120") == 0) || (strcmp(section, "hsr_panel_parameter_144") == 0)
        || (strcmp(section, "hsr_panel_parameter_240") == 0) || (strcmp(section, "hsr_panel_parameter_288") == 0)
        || (strcmp(section, "hsr_panel_parameter_4k1k_144") == 0) || (strcmp(section, "hsr_panel_parameter_4k2k_60") == 0))
    {
        long result = 0;
        int index = HSR_4k1k_120;
        result = simple_strtoul(value, NULL, 0);

        if(strcmp(section, "hsr_panel_parameter_120") == 0)
        {
            index = HSR_4k1k_120;
        }
        else if(strcmp(section, "hsr_panel_parameter_4k1k_144") == 0)
        {
            index = HSR_4k1k_144;
        }
        else if(strcmp(section, "hsr_panel_parameter_144") == 0)
        {
            index = HSR_4k2k_144;
        }
        else if(strcmp(section, "hsr_panel_parameter_240") == 0)
        {
            index = HSR_4k1k_240;
        }
        else if(strcmp(section, "hsr_panel_parameter_288") == 0)
        {
            index = HSR_4k1k_288;
        }
        else if(strcmp(section, "hsr_panel_parameter_4k2k60") == 0)
        {
            index = HSR_4k2k_60;
        }

        if(strcmp(name, "hsr_refresh") == 0)
        {
            s_hsr_panel_parameter_info.panel_parameter[index].refresh = result;
        }
        else if(strcmp(name, "hsr_typical") == 0)
        {
            s_hsr_panel_parameter_info.panel_parameter[index].typical = result;
        }
        else if(strcmp(name, "hsr_htotal") == 0)
        {
            s_hsr_panel_parameter_info.panel_parameter[index].htotal = result;
        }
        else if(strcmp(name, "hsr_vtotal") == 0)
        {
            s_hsr_panel_parameter_info.panel_parameter[index].vtotal = result;
        }
        else if(strcmp(name, "hsr_den_hstart") == 0)
        {
            s_hsr_panel_parameter_info.panel_parameter[index].den_hstart = result;
        }
        else if(strcmp(name, "hsr_den_hend") == 0)
        {
            s_hsr_panel_parameter_info.panel_parameter[index].den_hend = result;
        }
        else if(strcmp(name, "hsr_den_vstart") == 0)
        {
            s_hsr_panel_parameter_info.panel_parameter[index].den_vstart = result;
        }
        else if(strcmp(name, "hsr_den_vend") == 0)
        {
            s_hsr_panel_parameter_info.panel_parameter[index].den_vend = result;
        }
        else if(strcmp(name, "hsr_act_hstart") == 0)
        {
            s_hsr_panel_parameter_info.panel_parameter[index].act_hstart = result;
        }
        else if(strcmp(name, "hsr_act_hend") == 0)
        {
            s_hsr_panel_parameter_info.panel_parameter[index].act_hend = result;
        }
        else if(strcmp(name, "hsr_act_vstart") == 0)
        {
            s_hsr_panel_parameter_info.panel_parameter[index].act_vstart = result;
        }
        else if(strcmp(name, "hsr_act_vend") == 0)
        {
            s_hsr_panel_parameter_info.panel_parameter[index].act_vend = result;
        }
        else if(strcmp(name, "hsr_min_vtotal") == 0)
        {
            s_hsr_panel_parameter_info.panel_parameter[index].min_vtotal = result;
        }
        else if(strcmp(name, "hsr_max_vtotal") == 0)
        {
            s_hsr_panel_parameter_info.panel_parameter[index].max_vtotal = result;
        }
        else if(strcmp(name, "hsr_hsync_last_line") == 0)
        {
            s_hsr_panel_parameter_info.panel_parameter[index].hsync_last_line = result;
        }
        else
        {
            ret = 0;
        }
    }

    return ret;
}

static int ini_parse_handler_pmic(void *user, char *section, char *name, char *value)
{
    int ret = 1;

    if(strcmp(section, "PANEL") == 0)
    {
        if(strcmp(name, "PMIC_NAME") == 0)
        {
            strcpy(s_pmic_info.pmic_name, value);
        }
        else if(strcmp(name, "PMIC_I2C_ADDR") == 0)
        {
            long result = 0;
            result = simple_strtoul(value, NULL, 0);
            s_pmic_info.i2c_addr = result;
        }
        else if(strcmp(name, "PMIC_I2C_PORT") == 0)
        {
            long result = 0;
            result = simple_strtoul(value, NULL, 0);
            s_pmic_info.i2c_group_id = result;
        }
        else if(strcmp(name, "PMIC_I2C_REG") == 0)
        {
            long result = 0;
            result = simple_strtoul(value, NULL, 0);
            s_pmic_info.reg_addr = result;
        }
        else
        {
            ret = 0;
        }
    }

    return ret;
}

static int ini_parse_handler(void *user, char *section, char *name, char *value)
{
    int ret = 1;

    //DLG_PRINT_PARSE("section:%s\n", section);

    if(strcmp(section, "hsr_switch_panel_mode_type") == 0)
    {
        long result = 0;
        result = simple_strtoul(value, NULL, 0);
        if(strcmp(name, "type") == 0)
        {
            s_hsr_switch_panel_type = result;
        }
    }
    else if((strcmp(section, "hsr_switch_panel_mode_i2c_normal") == 0) || (strcmp(section, "hsr_switch_panel_mode_i2c_120") == 0)
        || (strcmp(section, "hsr_switch_panel_mode_i2c_144") == 0) || (strcmp(section, "hsr_switch_panel_mode_i2c_240") == 0)
        || (strcmp(section, "hsr_switch_panel_mode_i2c_288") == 0) || (strcmp(section, "hsr_switch_panel_mode_i2c_4k1k_144") == 0))
    {
        ret = ini_parse_handler_i2c(user, section, name, value);
    }
    else if(strcmp(section, "hsr_i2c_wp") == 0)
    {
       ini_parse_wp_info(user, section, name, value);
    }
    else if((strcmp(section, "hsr_switch_panel_mode_gpio_normal") == 0) || (strcmp(section, "hsr_switch_panel_mode_gpio_120") == 0)
            || (strcmp(section, "hsr_switch_panel_mode_gpio_144") == 0) || (strcmp(section, "hsr_switch_panel_mode_gpio_240") == 0)
            || (strcmp(section, "hsr_switch_panel_mode_gpio_288") == 0)
            || (strcmp(section, "hsr_switch_panel_mode_gpio_4k1k144") == 0))
    {
        ret = ini_parse_handler_gpio(user, section, name, value);
    }
    else if(strcmp(section, "hsr_vrr_mode") == 0)
    {
        ret = ini_parse_handler_vrr(user, section, name, value);
    }
    else if((strcmp(section, "hsr_panel_parameter_120") == 0) || (strcmp(section, "hsr_panel_parameter_144") == 0)
        || (strcmp(section, "hsr_panel_parameter_240") == 0) || (strcmp(section, "hsr_panel_parameter_288") == 0)
        || (strcmp(section, "hsr_panel_parameter_4k1k_144") == 0))
    {
        ret = ini_parse_handler_parameter(user, section, name, value);
    }
    else if(strcmp(section, "PANEL") == 0)
    {
        ret = ini_parse_handler_pmic(user, section, name, value);
    }
    else
    {
        ret = 0;
    }

    return ret;
}

static int ini_parse(char *filestart, unsigned int filelen,
    int (*handler)(void *, char *, char *, char *), void *user)
{
    /* Uses a fair bit of stack (use heap instead if you need to) */
    char line[MAX_LINE];
    char section[MAX_SECTION] = "";
    char prev_name[MAX_NAME] = "";

    char *curmem = filestart;
    char *start;
    char *end;
    char *name;
    char *value;
    size_t memleft = filelen;
    int lineno = 0;
    int error = 0;

    /* Scan through file line by line */
    while (memgets(line, sizeof(line), &curmem, &memleft) != NULL) {
        lineno++;
        start = lskip(rstrip(line));

        if (*start == ';' || *start == '#') {
            /*
             * Per Python ConfigParser, allow '#' comments at start
             * of line
             */
        }
//#if CONFIG_INI_ALLOW_MULTILINE
        else if (*prev_name && *start && start > line) {
            /*
             * Non-blank line with leading whitespace, treat as
             * continuation of previous name's value (as per Python
             * ConfigParser).
             */
            if (!handler(user, section, prev_name, start) && !error)
                error = lineno;
        }
//#endif
        else if (*start == '[') {
            /* A "[section]" line */
            end = find_char_or_comment(start + 1, ']');
            if (*end == ']') {
                *end = '\0';
                strncpy0(section, start + 1, sizeof(section));
                *prev_name = '\0';
            } else if (!error) {
                /* No ']' found on section line */
                error = lineno;
            }
        } else if (*start && *start != ';') {
            /* Not a comment, must be a name[=:]value pair */
            end = find_char_or_comment(start, '=');
            if (*end != '=')
                end = find_char_or_comment(start, ':');
            if (*end == '=' || *end == ':') {
                *end = '\0';
                name = rstrip(start);
                value = lskip(end + 1);
                end = find_char_or_comment(value, '\0');
                if (*end == ';')
                    *end = '\0';
                rstrip(value);
                /* Strip double-quotes */
                if (value[0] == '"' &&
                    value[strlen(value)-1] == '"') {
                    value[strlen(value)-1] = '\0';
                    value += 1;
                }

                /*
                 * Valid name[=:]value pair found, call handler
                 */
                strncpy0(prev_name, name, sizeof(prev_name));
                if (!handler(user, section, name, value) &&
                     !error){
                    error = lineno;
                    DLG_PRINT_PARSE(":Y: handler error=%d ,lineno=%d \n",error,lineno);
                }
            } else if (!error)
                /* No '=' or ':' found on name[=:]value line */
                error = lineno;
        }
    }

    return error;
}

static int ini_parse_wp_info(void *user, char *section, char *name, char *value)
{
    long result = 0;
    result = simple_strtoul(value, NULL, 0);

    if(strcmp(name, "wp_enable") == 0)
    {
        s_hsr_switch_panel_info_i2c.wp_enable = result;
        s_hsr_vrr_info.wp_enable = result;
    }
    else if(strcmp(name, "wp_hsr_before_write_data_delay") == 0)
    {
        s_hsr_switch_panel_info_i2c.wp_hsr_before_write_data_delay = result;
    }
    else if(strcmp(name, "wp_hsr_after_write_data_delay") == 0)
    {
        s_hsr_switch_panel_info_i2c.wp_hsr_after_write_data_delay = result;
    }
    else if(strcmp(name, "wp_vrr_before_mute_delay") == 0)
    {
        s_hsr_vrr_info.wp_vrr_before_mute_delay = result;
    }
    else if(strcmp(name, "wp_vrr_before_on_off_delay") == 0)
    {
        s_hsr_vrr_info.wp_vrr_before_on_off_delay = result;
    }
    else if(strcmp(name, "wp_vrr_before_unmute_delay") == 0)
    {
        s_hsr_vrr_info.wp_vrr_before_unmute_delay = result;
    }
    else if(strcmp(name, "wp_vrr_after_unmute_delay") == 0)
    {
        s_hsr_vrr_info.wp_vrr_after_unmute_delay = result;
    }

    return 1;
}


