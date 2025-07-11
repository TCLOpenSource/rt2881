#ifndef _AD82120_H
#define _AD82120_H


#define AD82120_ADDR_SIZE                1
#define AD82120_DATA_SIZE                1

#define MVOL                             0x03
#define C1VOL                            0x04
#define C2VOL                            0x05

#define CFUD                             0x32

#define AD82120_REGISTER_COUNT           134
#define AD82120_RAM_TABLE_COUNT          128

#define AD82120_SCTL1                    0x00
#define AD82120_SCTL2                    0x01
#define AD82120_SCTL3                    0x02
#define AD82120_MVOL                     0x03
#define AD82120_C1VOL                    0x04
#define AD82120_C2VOL                    0x05
#define AD82120_C3VOL                    0x06
#define AD82120_C4VOL                    0x07
#define AD82120_C5VOL                    0x08
#define AD82120_C6VOL                    0x09
#define AD82120_BTONE                    0x0A
#define AD82120_TTONE                    0x0B
#define AD82120_SCTL4                    0x0C
#define AD82120_C1CFG                    0x0D
#define AD82120_C2CFG                    0x0E
#define AD82120_C3CFG                    0x0F
#define AD82120_C4CFG                    0x10
#define AD82120_C5CFG                    0x11
#define AD82120_C6CFG                    0x12
#define AD82120_C7CFG                    0x13
#define AD82120_C8CFG                    0x14
#define AD82120_LAR1                     0x15
#define AD82120_LAR2                     0x16
#define AD82120_LAR3                     0x17
#define AD82120_LAR4                     0x18
#define AD82120_ERDLY                    0x19
#define AD82120_SCTL5                    0x1A
#define AD82120_SCTL6                    0x1B
#define AD82120_SCTL7                    0x1C
#define AD82120_CFADDR                   0x1D
#define AD82120_A1CF1                    0x1E
#define AD82120_A1CF2                    0x1F
#define AD82120_A1CF3                    0x20
#define AD82120_A1CF4                    0x21
#define AD82120_A2CF1                    0x22
#define AD82120_A2CF2                    0x23
#define AD82120_A2CF3                    0x24
#define AD82120_A2CF4                    0x25
#define AD82120_B1CF1                    0x26
#define AD82120_B1CF2                    0x27
#define AD82120_B1CF3                    0x28
#define AD82120_B1CF4                    0x29
#define AD82120_B2CF1                    0x2A
#define AD82120_B2CF2                    0x2B
#define AD82120_B2CF3                    0x2C
#define AD82120_B2CF4                    0x2D
#define AD82120_A0CF1                    0x2E
#define AD82120_A0CF2                    0x2F
#define AD82120_A0CF3                    0x30
#define AD82120_A0CF4                    0x31
#define AD82120_CFRW                     0x32
#define AD82120_SCTL8                    0x33
#define AD82120_SCTL9                    0x34
#define AD82120_VFT1                     0x35
#define AD82120_VFT2                     0x36
#define AD82120_ID                       0x37
#define AD82120_LMC                      0x38
#define AD82120_PMC                      0x39
#define AD82120_1STC1LM                  0x3A
#define AD82120_2NDC1LM                  0x3B
#define AD82120_3RDC1LM                  0x3C
#define AD82120_4THC1LM                  0x3D
#define AD82120_1STC2LM                  0x3E
#define AD82120_2NDC2LM                  0x3F
#define AD82120_3RDC2LM                  0x40
#define AD82120_4THC2LM                  0x41
#define AD82120_1STC3LM                  0x42
#define AD82120_2NDC3LM                  0x43
#define AD82120_3RDC3LM                  0x44
#define AD82120_4THC3LM                  0x45
#define AD82120_1STC4LM                  0x46
#define AD82120_2NDC4LM                  0x47
#define AD82120_3RDC4LM                  0x48
#define AD82120_4THC4LM                  0x49
#define AD82120_1STC5LM                  0x4A
#define AD82120_2NDC5LM                  0x4B
#define AD82120_3RDC5LM                  0x4C
#define AD82120_4THC5LM                  0x4D
#define AD82120_1STC6LM                  0x4E
#define AD82120_2NDC6LM                  0x4F
#define AD82120_3RDC6LM                  0x50
#define AD82120_4THC6LM                  0x51
#define AD82120_1STC7LM                  0x52
#define AD82120_2NDC7LM                  0x53
#define AD82120_3RDC7LM                  0x54
#define AD82120_4THC7LM                  0x55
#define AD82120_1STC8LM                  0x56
#define AD82120_2NDC8LM                  0x57
#define AD82120_3RDC8LM                  0x58
#define AD82120_4THC8LM                  0x59
#define AD82120_I2S_OUT                  0x5A
#define AD82120_MKHB                     0x5B
#define AD82120_MKLB                     0x5C
#define AD82120_HI_RES                   0x5D
#define AD82120_ANA_GAIN                 0x5E

#define AD82120_TDM_W                    0x69
#define AD82120_TDM_O                    0x6A
#define AD82120_FSW                      0x6B
#define AD82120_PMF_FS_R                 0x6C
#define AD82120_OC                       0x6D
#define AD82120_DTC                      0x6E
#define AD82120_TMR0                     0x6F
#define AD82120_TMR1                     0x71
#define AD82120_TMR2                     0x72
#define AD82120_Dither                   0x73
#define AD82120_EDL                      0x74
#define AD82120_MBIST_1ST_E              0x75
#define AD82120_MBIST_2ND_E              0x76
#define AD82120_MBIST_3RD_E              0x77
#define AD82120_MBIST_4TH_E              0x78
#define AD82120_MBIST_1ST_O              0x79
#define AD82120_MBIST_2ND_O              0x7A
#define AD82120_MBIST_3RD_O              0x7B
#define AD82120_MBIST_4TH_O              0x7C
#define AD82120_ERR_REG                  0x7D
#define AD82120_ERR_RECORD               0x7E
#define AD82120_ERR_CLEAR                0x7F


#define I2C_RETRY_DELAY 5 /* ms */  
#define I2C_RETRIES 3

typedef struct
{
    unsigned char bAddr;
    unsigned char bArray;
    unsigned char *reg_name;
}AD82120_REG;


void ad82120_func(int amp_i2c_id,unsigned short addr);
void ad82120m_func(int amp_i2c_id,unsigned short addr);
int ad82120_dump_all(const char *buf, int amp_i2c_id,unsigned short slave_addr);
int ad82120_mute_set(int on_off, int amp_i2c_id, unsigned short slave_addr);
int ad82120_param_get(unsigned char *reg, int amp_i2c_id, unsigned short slave_addr);
int ad82120_param_set(unsigned char *data_wr, int amp_i2c_id, unsigned short slave_addr);
void ad82120_amp_phase_reset(int amp_i2c_id);

#endif





