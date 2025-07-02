#include <asm/neon.h>
#include <asm/simd.h>
#include <asm/unaligned.h>
#include <crypto/internal/hash.h>
#include <crypto/internal/simd.h>
#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
#include <crypto/sha1.h>
#else
#include <crypto/sha.h>
#endif
#include <crypto/sha1_base.h>
#include <linux/cpufeature.h>
#include <linux/crypto.h>


struct sha1_ce_state {
	struct sha1_state	sst;
	u32			finalize;
};

asmlinkage void rtk_sha1_ce_transform(struct sha1_ce_state *sst, u8 const *src,
				  int blocks);

static void __sha1_ce_transform(struct sha1_state *sst, u8 const *src,
				int blocks)
{
	return rtk_sha1_ce_transform(container_of(sst, struct sha1_ce_state, sst),
				 src, blocks);
}

const u32 rtk_sha1_ce_offsetof_count = offsetof(struct sha1_ce_state, sst.count);
const u32 rtk_sha1_ce_offsetof_finalize = offsetof(struct sha1_ce_state, finalize);


static inline int sw_sha1_base_init(struct sha1_state *sctx)
{
	sctx->state[0] = SHA1_H0;
	sctx->state[1] = SHA1_H1;
	sctx->state[2] = SHA1_H2;
	sctx->state[3] = SHA1_H3;
	sctx->state[4] = SHA1_H4;
	sctx->count = 0;

	return 0;
}

static inline int sw_sha1_base_do_update(struct sha1_state *sctx,
				      const u8 *data,
				      unsigned int len,
				      sha1_block_fn *block_fn)
{
	unsigned int partial = sctx->count % SHA1_BLOCK_SIZE;

	sctx->count += len;

	if (unlikely((partial + len) >= SHA1_BLOCK_SIZE)) {
		int blocks;

		if (partial) {
			int p = SHA1_BLOCK_SIZE - partial;

			memcpy(sctx->buffer + partial, data, p);
			data += p;
			len -= p;

			block_fn(sctx, sctx->buffer, 1);
		}

		blocks = len / SHA1_BLOCK_SIZE;
		len %= SHA1_BLOCK_SIZE;

		if (blocks) {
			block_fn(sctx, data, blocks);
			data += blocks * SHA1_BLOCK_SIZE;
		}
		partial = 0;
	}
	if (len)
		memcpy(sctx->buffer + partial, data, len);

	return 0;
}

static inline int sw_sha1_base_do_finalize(struct sha1_state *sctx,
					sha1_block_fn *block_fn)
{
	const int bit_offset = SHA1_BLOCK_SIZE - sizeof(__be64);
	__be64 *bits = (__be64 *)(sctx->buffer + bit_offset);
	unsigned int partial = sctx->count % SHA1_BLOCK_SIZE;

	sctx->buffer[partial++] = 0x80;
	if (partial > bit_offset) {
		if(partial < SHA1_BLOCK_SIZE)
			memset(sctx->buffer + partial, 0x0, SHA1_BLOCK_SIZE - partial);
		partial = 0;

		block_fn(sctx, sctx->buffer, 1);
	}

	memset(sctx->buffer + partial, 0x0, bit_offset - partial);
	*bits = cpu_to_be64(sctx->count << 3);
	block_fn(sctx, sctx->buffer, 1);

	return 0;
}

static inline int sw_sha1_base_finish(struct sha1_state *sctx, u8 *out)
{
	__be32 *digest = (__be32 *)out;
	int i;

	for (i = 0; i < SHA1_DIGEST_SIZE / sizeof(__be32); i++)
		put_unaligned_be32(sctx->state[i], digest++);

	*sctx = (struct sha1_state){};
	return 0;
}

static int sha1_ce_update(struct sha1_ce_state *sctx, const u8 *data,
			  unsigned int len)
{

	if (!crypto_simd_usable())
		return -1;

	sctx->finalize = 0;
	kernel_neon_begin();
	sw_sha1_base_do_update(&sctx->sst, data, len, __sha1_ce_transform);
	kernel_neon_end();

	return 0;
}
#if 0
static int sha1_ce_finup(struct sha1_ce_state *sctx, const u8 *data,
			 unsigned int len, u8 *out)
{
	bool finalize = !sctx->sst.count && !(len % SHA1_BLOCK_SIZE) && len;

	if (!crypto_simd_usable())
		return -1;

	/*
	 * Allow the asm code to perform the finalization if there is no
	 * partial data and the input is a round multiple of the block size.
	 */
	sctx->finalize = finalize;

	kernel_neon_begin();
	sw_sha1_base_do_update(&sctx->sst, data, len, __sha1_ce_transform);
	if (!finalize)
		sw_sha1_base_do_finalize(&sctx->sst, __sha1_ce_transform);
	kernel_neon_end();
	return sw_sha1_base_finish(&sctx->sst, out);
}
#endif
static int sha1_ce_final(struct sha1_ce_state *sctx, bool need_padding, u8 *out)
{

	if (!crypto_simd_usable())
		return -1;

	sctx->finalize = 0;
	if(need_padding) {
		kernel_neon_begin();
		sw_sha1_base_do_finalize(&sctx->sst, __sha1_ce_transform);
		kernel_neon_end();
	}
	return sw_sha1_base_finish(&sctx->sst, out);
}

int sw_sha1_ce(bool need_padding, unsigned int *iv, 
		unsigned char *msg, unsigned int msg_len, unsigned char *hash)
{
	struct sha1_ce_state ce_state CRYPTO_MINALIGN_ATTR;
	struct sha1_state *sctx = &ce_state.sst;
	if(!need_padding && (msg_len & 0x3F))
		return -1;
	if(!msg || !msg_len || !hash)
		return -1;
	
	if(!iv) {
		sctx->state[0] = SHA1_H0;
		sctx->state[1] = SHA1_H1;
		sctx->state[2] = SHA1_H2;
		sctx->state[3] = SHA1_H3;
		sctx->state[4] = SHA1_H4;
	} else {
		sctx->state[0] = iv[0];
		sctx->state[1] = iv[1];
		sctx->state[2] = iv[2];
		sctx->state[3] = iv[3];
		sctx->state[4] = iv[4];
	}
	sctx->count = 0;

	if(sha1_ce_update(&ce_state, msg, msg_len) != 0)
		return -1;
	return sha1_ce_final(&ce_state, need_padding, hash);
}
