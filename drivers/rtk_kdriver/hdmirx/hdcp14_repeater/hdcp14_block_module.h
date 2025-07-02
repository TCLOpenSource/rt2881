#ifndef __HDCP_1_4_BLOCK_MODULE_H__
#define __HDCP_1_4_BLOCK_MODULE_H__

void hdcp_14_round_function_b(u32 bx, u32 by, u32 bz, u32 ky,
				u32 *bxo, u32 *byo, u32 *bzo);
void hdcp_14_round_function_k(u32 kx, u32 ky, u32 kz,
				u32 *kxo, u32 *kyo, u32 *kzo);
u32 hdcp_14_output_function(u32 ky, u32 kz, u32 by, u32 bz);

#endif
