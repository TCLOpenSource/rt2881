#include <crypto/internal/hash.h>
#include <crypto/internal/simd.h>
#include <crypto/sha2.h>
#include <crypto/sha256_base.h>
#include <linux/cpufeature.h>
#include <linux/crypto.h>
#include <linux/module.h>

#include <asm/hwcap.h>
#include <asm/simd.h>
#include <asm/neon.h>
#include <asm/unaligned.h>

asmlinkage void rtk_sha2_ce_transform(struct sha256_state *sst, u8 const *src,
				  int blocks);

static inline int sw_sha256_base_do_update(struct sha256_state *sctx,
					const u8 *data,
					unsigned int len,
					sha256_block_fn *block_fn)
{
	unsigned int partial = sctx->count % SHA256_BLOCK_SIZE;

	sctx->count += len;

	if (unlikely((partial + len) >= SHA256_BLOCK_SIZE)) {
		int blocks;

		if (partial) {
			int p = SHA256_BLOCK_SIZE - partial;

			memcpy(sctx->buf + partial, data, p);
			data += p;
			len -= p;

			block_fn(sctx, sctx->buf, 1);
		}

		blocks = len / SHA256_BLOCK_SIZE;
		len %= SHA256_BLOCK_SIZE;

		if (blocks) {
			block_fn(sctx, data, blocks);
			data += blocks * SHA256_BLOCK_SIZE;
		}
		partial = 0;
	}
	if (len)
		memcpy(sctx->buf + partial, data, len);

	return 0;
}

static inline int sw_sha256_base_do_finalize(struct sha256_state *sctx,
					  sha256_block_fn *block_fn)
{
	const int bit_offset = SHA256_BLOCK_SIZE - sizeof(__be64);
	__be64 *bits = (__be64 *)(sctx->buf + bit_offset);
	unsigned int partial = sctx->count % SHA256_BLOCK_SIZE;

	sctx->buf[partial++] = 0x80;
	if (partial > bit_offset) {
		if(partial != SHA256_BLOCK_SIZE)
			memset(sctx->buf + partial, 0x0, SHA256_BLOCK_SIZE - partial);
		partial = 0;

		block_fn(sctx, sctx->buf, 1);
	}

	memset(sctx->buf + partial, 0x0, bit_offset - partial);
	*bits = cpu_to_be64(sctx->count << 3);
	block_fn(sctx, sctx->buf, 1);

	return 0;
}

static inline int sw_sha256_base_finish(struct sha256_state *sctx, u8 *out)
{
	int digest_size = 32;
	__be32 *digest = (__be32 *)out;
	int i;

	for (i = 0; digest_size > 0; i++, digest_size -= sizeof(__be32))
		put_unaligned_be32(sctx->state[i], digest++);

	*sctx = (struct sha256_state){};
	return 0;
}

static int sw_sha256_ce_update(struct sha256_state *sctx , const u8 *data,
			  unsigned int len)
{
	if (!crypto_simd_usable() ||
	    (sctx->count % SHA256_BLOCK_SIZE) + len < SHA256_BLOCK_SIZE)
		return -1;

	kernel_neon_begin();
	sw_sha256_base_do_update(sctx, data, len,
			      (sha256_block_fn *)rtk_sha2_ce_transform);
	kernel_neon_end();

	return 0;
}

#if 0
static int sw_sha256_ce_finup(struct sha256_state *sctx, const u8 *data,
			 unsigned int len, u8 *out)
{
	if (!crypto_simd_usable())
		return -1;

	kernel_neon_begin();
	if (len)
		sw_sha256_base_do_update(sctx, data, len,
				      (sha256_block_fn *)rtk_sha2_ce_transform);
	sw_sha256_base_do_finalize(sctx, (sha256_block_fn *)rtk_sha2_ce_transform);
	kernel_neon_end();

	return sw_sha256_base_finish(sctx, out);
}
#endif

static int sw_sha256_ce_final(struct sha256_state *sctx, bool need_padding, u8 *out)
{
	if (!crypto_simd_usable())
		return -1;
	if(need_padding) {
		kernel_neon_begin();
		sw_sha256_base_do_finalize(sctx, (sha256_block_fn *)rtk_sha2_ce_transform);
		kernel_neon_end();
	}

	return sw_sha256_base_finish(sctx, out);
}

int sw_sha256_ce(bool need_padding, unsigned int *iv, 
		unsigned char *msg, unsigned int msg_len, unsigned char *hash)
{
	struct sha256_state sctx CRYPTO_MINALIGN_ATTR;
	if(!need_padding && (msg_len & 0x3F))
		return -1;
	if(!msg || !msg_len || !hash)
		return -1;
	
	if(!iv) {
		sctx.state[0] = SHA256_H0;
		sctx.state[1] = SHA256_H1;
		sctx.state[2] = SHA256_H2;
		sctx.state[3] = SHA256_H3;
		sctx.state[4] = SHA256_H4;
		sctx.state[5] = SHA256_H5;
		sctx.state[6] = SHA256_H6;
		sctx.state[7] = SHA256_H7;
	} else {
		sctx.state[0] = iv[0];
		sctx.state[1] = iv[1];
		sctx.state[2] = iv[2];
		sctx.state[3] = iv[3];
		sctx.state[4] = iv[4];
		sctx.state[5] = iv[5];
		sctx.state[6] = iv[6];
		sctx.state[7] = iv[7];
	}
	sctx.count = 0;

	if(sw_sha256_ce_update(&sctx, msg, msg_len) != 0)
		return -1;
	return sw_sha256_ce_final(&sctx, need_padding, hash);
}


