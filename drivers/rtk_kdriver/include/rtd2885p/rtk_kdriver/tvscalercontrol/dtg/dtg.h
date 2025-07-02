#ifndef _DTG_H_
#define _DTG_H_

typedef enum
{
    DOUBLE_BUFFER_D0,
    DOUBLE_BUFFER_D1,
    DOUBLE_BUFFER_D2,
    DOUBLE_BUFFER_D3,
    DOUBLE_BUFFER_D4,
    DOUBLE_BUFFER_D5,
    DOUBLE_BUFFER_D6,
    DOUBLE_BUFFER_D7,
    DOUBLE_BUFFER_D8,
    DOUBLE_BUFFER_D9,
}DTG_DOUBLE_BUFFER_INDEX;

int m8p_dtg_set_double_buffer_enable(DTG_DOUBLE_BUFFER_INDEX index, unsigned char *enable);
int m8p_dtg_get_double_buffer_enable(DTG_DOUBLE_BUFFER_INDEX index, unsigned char *enable);
int m8p_dtg_set_double_buffer_apply(DTG_DOUBLE_BUFFER_INDEX index, unsigned char *apply);
int m8p_dtg_get_double_buffer_apply(DTG_DOUBLE_BUFFER_INDEX index, unsigned char *apply);

#endif
