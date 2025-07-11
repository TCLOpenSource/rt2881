#ifndef __RTK_RCP_REG_H__
#define __RTK_RCP_REG_H__
#include <rbus/cp_reg.h>


// Register definition
#define REG_SET_WR_EN5                BIT(17)
#define REG_SET_MODE(x)               (((x) & 0xF) << 13)
#define REG_SET_MODE_AES                        REG_SET_MODE(0x0)
#define REG_SET_MODE_TDES                      REG_SET_MODE(0x1)
#define REG_SET_MODE_DES                        REG_SET_MODE(0x2)
#define REG_SET_MODE_SHA_256                REG_SET_MODE(0x3)
#define REG_SET_MODE_AES_192                REG_SET_MODE(0x4)
#define REG_SET_MODE_AES_256                REG_SET_MODE(0x7)
#define REG_SET_MODE_CAM                       REG_SET_MODE(0x8)
#define REG_SET_MODE_CAM_192                REG_SET_MODE(0x9)
#define REG_SET_MODE_CAM_256                REG_SET_MODE(0xa)
#define REG_SET_MODE_CW_LOGIC             REG_SET_MODE(0xF)

#define REG_SET_WR_EN4                BIT(11)
#define REG_SET_ENDIAN_SWAP(x)   (((x) & 0x1) << 10)
#define REG_SET_BIG_ENDIAN          REG_SET_ENDIAN_SWAP(0)
#define REG_SET_LITTLE_ENDIAN          REG_SET_ENDIAN_SWAP(1)


#define REG_SET_WR_EN3                BIT(9)
#define REG_CW_LOGIC_OPSEL(x)    (((x) & 0x3) << 7)
#define REG_CW_LOGIC_AND            REG_CW_LOGIC_OPSEL(0x0)
#define REG_CW_LOGIC_OR            REG_CW_LOGIC_OPSEL(0x1)
#define REG_CW_LOGIC_XOR            REG_CW_LOGIC_OPSEL(0x2)

#define REG_SET_WR_EN2                BIT(6)
#define REG_SET_CTR                   (0x2<<4)
#define REG_SET_CBC                   (0x1<<4)
#define REG_SET_ECB                   0x0
#define REG_SET_WR_EN1                BIT(3)
#define REG_SET_FIRST_128             BIT(2)

#define REG_SET_WR_EN0                BIT(1)
#define REG_SET_ENC                   BIT(0)
#define REG_SET_DEC                   0x0

#define RCP_DES_ECB_ENC               (REG_SET_WR_EN5 | REG_SET_MODE_DES | REG_SET_WR_EN2 | REG_SET_ECB | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_DES_ECB_DEC               (REG_SET_WR_EN5 | REG_SET_MODE_DES | REG_SET_WR_EN2 | REG_SET_ECB | REG_SET_WR_EN0 | REG_SET_DEC)
#define RCP_DES_CBC_ENC               (REG_SET_WR_EN5 | REG_SET_MODE_DES | REG_SET_WR_EN2 | REG_SET_CBC | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_DES_CBC_DEC               (REG_SET_WR_EN5 | REG_SET_MODE_DES | REG_SET_WR_EN2 | REG_SET_CBC | REG_SET_WR_EN0 | REG_SET_DEC)
#define RCP_TDES_ECB_ENC              (REG_SET_WR_EN5 | REG_SET_MODE_TDES | REG_SET_WR_EN2 | REG_SET_ECB | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_TDES_ECB_DEC              (REG_SET_WR_EN5 | REG_SET_MODE_TDES | REG_SET_WR_EN2 | REG_SET_ECB | REG_SET_WR_EN0 | REG_SET_DEC)
#define RCP_TDES_CBC_ENC              (REG_SET_WR_EN5 | REG_SET_MODE_TDES | REG_SET_WR_EN2 | REG_SET_CBC | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_TDES_CBC_DEC              (REG_SET_WR_EN5 | REG_SET_MODE_TDES | REG_SET_WR_EN2 | REG_SET_CBC | REG_SET_WR_EN0 | REG_SET_DEC)

#define RCP_AES_128_ECB_ENC           (REG_SET_WR_EN5 | REG_SET_MODE_AES | REG_SET_WR_EN2 | REG_SET_ECB | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_AES_128_ECB_DEC           (REG_SET_WR_EN5 | REG_SET_MODE_AES | REG_SET_WR_EN2 | REG_SET_ECB | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_DEC)
#define RCP_AES_128_CBC_ENC           (REG_SET_WR_EN5 | REG_SET_MODE_AES | REG_SET_WR_EN2 | REG_SET_CBC | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_AES_128_CBC_DEC           (REG_SET_WR_EN5 | REG_SET_MODE_AES | REG_SET_WR_EN2 | REG_SET_CBC | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_DEC)
#define RCP_AES_128_CTR_ENC           (REG_SET_WR_EN5 | REG_SET_MODE_AES | REG_SET_WR_EN2 | REG_SET_CTR | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_AES_128_CTR_DEC           (REG_SET_WR_EN5 | REG_SET_MODE_AES | REG_SET_WR_EN2 | REG_SET_CTR | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_DEC)

#define RCP_AES_192_ECB_ENC           (REG_SET_WR_EN5 | REG_SET_MODE_AES_192 | REG_SET_WR_EN2 | REG_SET_ECB | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_AES_192_ECB_DEC           (REG_SET_WR_EN5 | REG_SET_MODE_AES_192 | REG_SET_WR_EN2 | REG_SET_ECB | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_DEC)
#define RCP_AES_192_CBC_ENC           (REG_SET_WR_EN5 | REG_SET_MODE_AES_192 | REG_SET_WR_EN2 | REG_SET_CBC | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_AES_192_CBC_DEC           (REG_SET_WR_EN5 | REG_SET_MODE_AES_192 | REG_SET_WR_EN2 | REG_SET_CBC | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_DEC)
#define RCP_AES_192_CTR_ENC           (REG_SET_WR_EN5 | REG_SET_MODE_AES_192 | REG_SET_WR_EN2 | REG_SET_CTR | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_AES_192_CTR_DEC           (REG_SET_WR_EN5 | REG_SET_MODE_AES_192 | REG_SET_WR_EN2 | REG_SET_CTR | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_DEC)

#define RCP_AES_256_ECB_ENC           (REG_SET_WR_EN5 | REG_SET_MODE_AES_256 | REG_SET_WR_EN2 | REG_SET_ECB | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_AES_256_ECB_DEC           (REG_SET_WR_EN5 | REG_SET_MODE_AES_256 | REG_SET_WR_EN2 | REG_SET_ECB | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_DEC)
#define RCP_AES_256_CBC_ENC           (REG_SET_WR_EN5 | REG_SET_MODE_AES_256 | REG_SET_WR_EN2 | REG_SET_CBC | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_AES_256_CBC_DEC           (REG_SET_WR_EN5 | REG_SET_MODE_AES_256 | REG_SET_WR_EN2 | REG_SET_CBC | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_DEC)
#define RCP_AES_256_CTR_ENC           (REG_SET_WR_EN5 | REG_SET_MODE_AES_256 | REG_SET_WR_EN2 | REG_SET_CTR | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_AES_256_CTR_DEC           (REG_SET_WR_EN5 | REG_SET_MODE_AES_256 | REG_SET_WR_EN2 | REG_SET_CTR | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_DEC)


#define RCP_CAM_128_ECB_ENC           (REG_SET_WR_EN5 | REG_SET_MODE_CAM | REG_SET_WR_EN2 | REG_SET_ECB | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_CAM_128_ECB_DEC           (REG_SET_WR_EN5 | REG_SET_MODE_CAM | REG_SET_WR_EN2 | REG_SET_ECB | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_DEC)
#define RCP_CAM_128_CBC_ENC           (REG_SET_WR_EN5 | REG_SET_MODE_CAM | REG_SET_WR_EN2 | REG_SET_CBC | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_CAM_128_CBC_DEC           (REG_SET_WR_EN5 | REG_SET_MODE_CAM | REG_SET_WR_EN2 | REG_SET_CBC | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_DEC)
#define RCP_CAM_128_CTR_ENC           (REG_SET_WR_EN5 | REG_SET_MODE_CAM | REG_SET_WR_EN2 | REG_SET_CTR | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_CAM_128_CTR_DEC           (REG_SET_WR_EN5 | REG_SET_MODE_CAM | REG_SET_WR_EN2 | REG_SET_CTR | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_DEC)

#define RCP_CAM_192_ECB_ENC           (REG_SET_WR_EN5 | REG_SET_MODE_CAM_192 | REG_SET_WR_EN2 | REG_SET_ECB | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_CAM_192_ECB_DEC           (REG_SET_WR_EN5 | REG_SET_MODE_CAM_192 | REG_SET_WR_EN2 | REG_SET_ECB | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_DEC)
#define RCP_CAM_192_CBC_ENC           (REG_SET_WR_EN5 | REG_SET_MODE_CAM_192 | REG_SET_WR_EN2 | REG_SET_CBC | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_CAM_192_CBC_DEC           (REG_SET_WR_EN5 | REG_SET_MODE_CAM_192 | REG_SET_WR_EN2 | REG_SET_CBC | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_DEC)
#define RCP_CAM_192_CTR_ENC           (REG_SET_WR_EN5 | REG_SET_MODE_CAM_192 | REG_SET_WR_EN2 | REG_SET_CTR | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_CAM_192_CTR_DEC           (REG_SET_WR_EN5 | REG_SET_MODE_CAM_192 | REG_SET_WR_EN2 | REG_SET_CTR | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_DEC)

#define RCP_CAM_256_ECB_ENC           (REG_SET_WR_EN5 | REG_SET_MODE_CAM_256 | REG_SET_WR_EN2 | REG_SET_ECB | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_CAM_256_ECB_DEC           (REG_SET_WR_EN5 | REG_SET_MODE_CAM_256 | REG_SET_WR_EN2 | REG_SET_ECB | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_DEC)
#define RCP_CAM_256_CBC_ENC           (REG_SET_WR_EN5 | REG_SET_MODE_CAM_256 | REG_SET_WR_EN2 | REG_SET_CBC | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_CAM_256_CBC_DEC           (REG_SET_WR_EN5 | REG_SET_MODE_CAM_256 | REG_SET_WR_EN2 | REG_SET_CBC | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_DEC)
#define RCP_CAM_256_CTR_ENC           (REG_SET_WR_EN5 | REG_SET_MODE_CAM_256 | REG_SET_WR_EN2 | REG_SET_CTR | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_ENC)
#define RCP_CAM_256_CTR_DEC           (REG_SET_WR_EN5 | REG_SET_MODE_CAM_256 | REG_SET_WR_EN2 | REG_SET_CTR | REG_SET_WR_EN1 | REG_SET_FIRST_128 | REG_SET_WR_EN0 | REG_SET_DEC)
 

#define RCP_CW_LOGIC_OPSEL_AND            (REG_SET_WR_EN5 | REG_SET_MODE_CW_LOGIC | REG_SET_WR_EN3 | REG_CW_LOGIC_AND)
#define RCP_CW_LOGIC_OPSEL_OR            (REG_SET_WR_EN5 | REG_SET_MODE_CW_LOGIC | REG_SET_WR_EN3 | REG_CW_LOGIC_OR)
#define RCP_CW_LOGIC_OPSEL_XOR            (REG_SET_WR_EN5 | REG_SET_MODE_CW_LOGIC | REG_SET_WR_EN3 | REG_CW_LOGIC_XOR)


#define REG_SET1_DES_KEY_ORDER        BIT(9)

#define REG_SET1_KEY_MODE(x)            (((x) & 0x3)<<6)
#define REG_SET1_KEY_MODE_REGISTER      REG_SET1_KEY_MODE(0)
#define REG_SET1_KEY_MODE_CW            REG_SET1_KEY_MODE(1)
#define REG_SET1_KEY_MODE_OTP           REG_SET1_KEY_MODE(2)
#define REG_SET1_KEY_MODE_MASK          REG_SET1_KEY_MODE(3)

#define REG_SET1_IV_MODE(x)             (((x) & 0x3)<<4)
#define REG_SET1_IV_MODE_REG            REG_SET1_IV_MODE(0)
#define REG_SET1_IV_MODE_CW             REG_SET1_IV_MODE(1)

#define REG_SET1_INPUT_MODE(x)          (((x) & 0x3)<<2)
#define REG_SET1_INPUT_MODE_REG         REG_SET1_INPUT_MODE(0)
#define REG_SET1_INPUT_MODE_CW          REG_SET1_INPUT_MODE(1)

#define REG_SET1_OUTPUT_MODE(x)         (((x) & 0x3))
#define REG_SET1_OUTPUT_MODE_REG        REG_SET1_OUTPUT_MODE(0)
#define REG_SET1_OUTPUT_MODE_CW         REG_SET1_OUTPUT_MODE(1)

#define REG_SET2_OUTPUT_ENTRY(x)        (((x) & 0x7F)<<24)
#define REG_SET2_INPUT_ENTRY(x)         (((x) & 0x7F)<<16)
#define REG_SET2_KEY_ENTRY(x)           (((x) & 0x7F)<<8)
#define REG_SET2_IV_ENTRY(x)            (((x) & 0x7F))


#define CP_REG_MOD_KEY_0_REG                    CP_CP_REG_KEY_0_reg
#define CP_REG_MOD_KEY1_0_REG                   CP_CP_REG_KEY1_0_reg
//CP_CP_REG_DATAIN_reg

#define CP_REG_MOD_DATAIN_0_REG                 CP_CP_REG_DATAIN_reg
#define CP_REG_MOD_DATAOUT_0_REG                CP_CP_REG_DATAOUT_reg
#define CP_REG_MOD_IV_0_REG                     CP_CP_REG_IV_0_reg
#define CP_REG_MOD_SET_REG                      CP_CP_REG_SET_reg
#define CP_REG_MOD_SET1_REG                     CP_CP_REG_SET1_reg
#define CP_REG_MOD_SET2_REG                     CP_CP_REG_SET2_reg


#define SET_CP_REG_MOD_KEY(index, val)          rtd_outl(CP_REG_MOD_KEY_0_REG + (index) * 4, val)
#define SET_CP_REG_MOD_KEY1(index, val)          rtd_outl(CP_REG_MOD_KEY1_0_REG + (index) * 4, val)
#define SET_CP_REG_MOD_DATAIN(index, val)       rtd_outl(CP_REG_MOD_DATAIN_0_REG, val)
#define SET_CP_REG_MOD_DATAOUT(index, val)       rtd_outl(CP_REG_MOD_DATAOUT_0_REG, val)
#define SET_CP_REG_MOD_IV(index, val)           rtd_outl(CP_REG_MOD_IV_0_REG + (index) * 4, val)
#define SET_CP_REG_MOD_SET(val)                 rtd_outl(CP_REG_MOD_SET_REG, val)
#define SET_CP_REG_MOD_SET1(val)                rtd_outl(CP_REG_MOD_SET1_REG, val)
#define SET_CP_REG_MOD_SET2(val)                rtd_outl(CP_REG_MOD_SET2_REG, val)

#define GET_CP_REG_MOD_KEY(index)               rtd_inl(CP_REG_MOD_KEY_0_REG + (index) * 4)
#define GET_CP_REG_MOD_KEY1(index)               rtd_inl(CP_REG_MOD_KEY1_0_REG + (index) * 4)
#define GET_CP_REG_MOD_DATAIN(index)            rtd_inl(CP_REG_MOD_DATAIN_0_REG)
#define GET_CP_REG_MOD_DATAOUT(index)            rtd_inl(CP_REG_MOD_DATAOUT_0_REG)
#define GET_CP_REG_MOD_IV(index)                rtd_inl(CP_REG_MOD_IV_0_REG + (index) * 4)
#define GET_CP_REG_MOD_SET()                    rtd_inl(CP_REG_MOD_SET_REG)
#define GET_CP_REG_MOD_SET1()                   rtd_inl(CP_REG_MOD_SET1_REG)
#define GET_CP_REG_MOD_SET2()                   rtd_inl(CP_REG_MOD_SET2_REG)


#endif
