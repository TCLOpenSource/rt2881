
#ifndef CONFIG_CRYPTO_RTK_ZACC
#include <rtd_log/rtd_module_log.h>
#define zacc_fmt(fmt) "rtk_zacc:  " fmt
#define printk_always(fmt,...)  do{int loglevel=console_loglevel;console_loglevel=8;rtd_pr_misc_err(fmt,##__VA_ARGS__);console_loglevel=loglevel;}while(0)

#include <linux/module.h>

#include <linux/kernel.h>

#include <linux/dma-mapping.h>
#include <linux/highmem.h>
#include <linux/list.h>
#include <linux/mm.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/smp.h>
#include <rbus/lzma_reg.h>
#include <rtk_kdriver/io.h>
#include <linux/delay.h>
#include <linux/smp.h>

#if 0
static void dmac_flush_range_ex (unsigned long start, unsigned long end)
{
    unsigned long ctr_el0;
    unsigned int cacheline_size;
#ifdef CONFIG_ARM64
    asm volatile("mrs %0, ctr_el0" : "=r" (ctr_el0) :: "memory");
#else
    asm volatile("MRC p15,0,%0,c0,c0,1" : "=r" (ctr_el0) :: "memory");
#endif
    cacheline_size=4<<((ctr_el0>>16)&0xF);

    for(;start<end;start=start+cacheline_size)
    {
#ifdef CONFIG_ARM64
        asm volatile("dc civac, %0" : : "r" (start) : "memory");
#else
        asm volatile("MCR p15,0,%0,c7,c14,1" : : "r" (start) : "memory");
#endif
    }
}
static void dmac_inv_range_ex (unsigned long start, unsigned long end)
{
    unsigned long ctr_el0;
    unsigned int cacheline_size;
#ifdef CONFIG_ARM64
    asm volatile("mrs %0, ctr_el0" : "=r" (ctr_el0) :: "memory");
#else
    asm volatile("MRC p15,0,%0,c0,c0,1" : "=r" (ctr_el0) :: "memory");
#endif
    cacheline_size=4<<((ctr_el0>>16)&0xF);

    for(;start<end;start=start+cacheline_size)
    {
#ifdef CONFIG_ARM64
        asm volatile("dc ivac, %0" : : "r" (start) : "memory");
#else
        asm volatile("MCR p15,0,%0,c7,c6,1" : : "r" (start) : "memory");
#endif
    }
}
#undef dmac_flush_range
#define dmac_flush_range dmac_flush_range_ex
#undef dmac_inv_range
#define dmac_inv_range dmac_inv_range_ex
#endif

//#define ZACC_CRC32_ENABLE
#define ZACC_PERF_ENABLE

#ifdef CONFIG_ARCH_RTK2819A
#define ZACC_8G_DRAM_SUPPORT
#define ZACC_ADDR_ALIGN 5
#else
#define ZACC_ADDR_ALIGN 4
#endif

#ifdef CONFIG_ARCH_RTK6702
#define ZACC_HW_DONE_EN        0
#else
#define ZACC_HW_DONE_EN        1
#endif

#define ZACC_DEC_MAX_RETRY_COUNT    3
#define ZACC_DEC_TIMEOUT_COUNT1     4000
#define ZACC_ENC_TIMEOUT_COUNT1 4000

#define ZACC_MAGIC_1 0xaa55aa55
#define  ZACC_MAGIC_2_1  0x2378beaf
#define  ZACC_MAGIC_2_2  0x2379beaf
#define  ZACC_MAGIC_3  0x5A414343

#define ZACC_DESC_SIZE      0x10
#define ZACC_QUEUE_SIZE 4
unsigned int zacc_hw_resource[ZACC_QUEUE_SIZE] = {0, 0, 0, 0};

const unsigned int reg_offset[ZACC_QUEUE_SIZE]= {0, 0x24, 0x4c, 0x70};

spinlock_t zacc_hw_lock;

#define CRC32_POLY  0x04c11db7      /* AUTODIN II, Ethernet, & FDDI */
unsigned int crc32_table[256];
static void init_crc32 (void)
{
    int i, j;
    unsigned int c;
    for (i = 0; i < 256; ++i)
    {
        for (c = i << 24, j = 8; j > 0; --j)
        {
            c = c & 0x80000000 ? (c << 1) ^ CRC32_POLY : (c << 1);
        }
        crc32_table[i] = c;
    }
}
unsigned int calc_crc32 (unsigned char *buf, u32 len)
{
    unsigned char   *p;
    unsigned int    crc;
    crc = 0xffffffff;       /* preload shift register, per CRC-32 spec */
    for (p = buf; len > 0; p++)
    {
        crc = (crc << 8) ^ crc32_table[(crc >> 24) ^ *(p)];
        if (len > 0)
        {
            len --;
        }
        else
        {
            len = 0;
        }
    }
    return(crc);
}
#ifdef CONFIG_ARM64
unsigned long par_64 (unsigned long addr, int user)
{
    unsigned long phys_addr;
    if(user)
    {
        asm volatile("at s1e0r, %0; isb; dsb ish" : : "r" (addr));
    }
    else
    {
        asm volatile("at s1e1r, %0; isb; dsb ish" : : "r" (addr));
    }

    asm volatile("mrs %0, par_el1; dsb ish" : "=r" (phys_addr));
    //console_loglevel=6;
    //rtd_pr_misc_err("%s %d   %016llx   %016llx\n",__func__,__LINE__,addr,phys_addr);
    return phys_addr;
}
#else
unsigned long par_64 (unsigned long addr, int user)
{
    unsigned long ret;
 
    __asm__ __volatile__ (
        "cmp  %2, #0 \n\t"
        "MCReq p15, 0, %1, c7, c8, 0  @; ATS1CPR operation \n\t"
        "MCRne p15, 0, %1, c7, c8, 2  @; ATS1CUR operation \n\t"
        "isb \n\t"
        "dsb \n\t"
        "MRC p15, 0, %0, c7, c4, 0    @PAR \n\t"
        "dsb \n\t"
        :"=r"(ret)
        :"r"(addr), "r"(user));
    return ret;
}
#endif
#if 0
#include <linux/pageremap.h>
#include <linux/rtkrecord.h>
#else
//#include <rtk_kdriver/rmm/pageremap.h>
//#include <rtk_kdriver/rmm/rtkrecord.h>
void rtk_record_list_dump(void);
#endif
//void dump_decode_data (void const *src, unsigned int size, unsigned int crc32, unsigned int crc32_orig, unsigned int line )
void dump_decode_data (void const *src, unsigned int size, unsigned int line )
{
    console_loglevel = 7;

#if 0
    list_all_rtk_memory_allocation_sort(list_mem_generic,NULL,NULL);
    show_cma_avaliable();
#else
    rtk_record_list_dump();
#endif

    print_hex_dump(KERN_ERR, "rtlzma data input : ", DUMP_PREFIX_ADDRESS,16, 1, src, size, true);
#ifdef CONFIG_ARM64
    rtd_pr_misc_err("\n\n\nrtlzma data dump again  %d,    src: 0x%016llx|0x%016llx\n",line,src,par_64((unsigned long)src,0));
#else
    rtd_pr_misc_err("\n\n\nrtlzma data dump again  %d,    src: 0x%08lx|0x%08lx\n",    line,src,par_64((unsigned long)src,0));
#endif

#ifdef CONFIG_ARM64
    print_hex_dump(KERN_ERR, "rtlzma data input : ", DUMP_PREFIX_ADDRESS,16, 1, (void *)((unsigned long)src&0xfffffffffffff000ULL), PAGE_SIZE, true);
#else
    print_hex_dump(KERN_ERR, "rtlzma data input : ", DUMP_PREFIX_ADDRESS,16, 1, (void *)((unsigned long)src&0xfffff000), PAGE_SIZE, true);
#endif

#ifdef CONFIG_RTK_FEATURE_FOR_GKI
    dump_page(virt_to_page(src), "dump lzma decode buffer page");
#endif

    //panic("rtlzma data error! crc32: 0x%x|0x%x, src: 0x%016llx|0x%016llx\n", crc32, crc32_orig, src,par_64(src,0));
#ifdef CONFIG_ARM64
    panic("rtlzma data error! src: 0x%016llx|0x%016llx\n", src,par_64((unsigned long)src,0));
#else
    panic("rtlzma data error! src: 0x%08lx|0x%08lx\n",     src,par_64((unsigned long)src,0));
#endif
}


#pragma pack(push, 1)
struct zacc_encode_desc
{
    u32 encode:             1;      /* 0      - Encode mode (SW) */
    u32 decode:             1;      /* 1      - Decode mode (SW) */
    u32 bypss:              1;      /* 2      - Bypass mode (SW) */
    u32 cmd_err:            1;      /* 3      - Cmd error (HW) */
    u32 src_addr:           28;     /* 31:4   - Data source address (SW) */

    u32 hw_done:            1;      /* 32     - HW done (HW) */
    u32 over_4k:            1;      /* 33     - Size over 4KB (HW) */
    u32 unused_2:           2;      /* 35:34  - Unused */
    u32 dst_addr:           28;     /* 63:36  - Data destination address (SW) */

    u32 comp_size:          13;     /* 76:64  - Compressed size (HW) */
#ifdef ZACC_8G_DRAM_SUPPORT
    u32 src_dec_unit_sel:   1;      /* 77     - 1 for 32byte align, 0 for 16 byte align */
    u32 unused_1:           2;      /* 79:78  - Unused */
#else
    u32 unused_1:           3;      /* 79:77  - Unused */
#endif
    u32 crc:                16;     /* 95:80  - Crc result (HW) */

    u32 unused_0:           32;     /* 127:96 - Unused */
};

struct zacc_decode_desc
{
    u32 encode:             1;      /*0       - Encode mode (SW) */
    u32 decode:             1;      /*1       - Decode mode (SW) */
    u32 bypass:             1;      /*2       - Bypass mode (SW) */
    u32 cmd_err:            1;      /*3       - Cmd error (HW) */
    u32 src_addr:           28;     /*31:4    - Data source address (SW) */

    u32 hw_done:            1;      /*32      - HW done (HW) */
    u32 size_err:           1;      /*33      - Size error (HW) */
    u32 decode_err:         1;      /*34      - Decode error (HW) */
    u32 dist_err:           1;      /*35      - Distance error (HW) */
    u32 dst_addr:           28;     /*63:36   - Data destination address (SW) */

    u32 comp_size:          23;     /*86:64   - Compressed size, 16bytes alignmen (SW) */
    u32 crc_err:            1;      /*87      - CRC error (HW) */
    u32 crc_msb:            8;      /*95:88   - Golden crc msb 8 bit (SW) */

    u32 decomp_size:        23;     /*118:96  - Decompressed size, byte (SW) */
#ifdef ZACC_8G_DRAM_SUPPORT
    u32 src_dec_unit_sel:   1;      /*119     - 1 for 32byte align, 0 for 16 byte align */
#else
    u32 unused_0:           1;      /*119     - Unused */
#endif
    u32 crc_lsb:            8;      /*127:120 - Golden crc lsb 8 bit (SW) */
};

struct zacc_desc
{
    u32 data[64];
};

#pragma pack(pop)

static void *desc_uncached;
static struct page *desc_page;
static dma_addr_t desc_phys;

static struct zacc_desc *desc;
static struct page *dst_pg[ZACC_QUEUE_SIZE];
static struct page *src_pg[ZACC_QUEUE_SIZE];
static struct page *src_tmp_pg[ZACC_QUEUE_SIZE];


unsigned int zacc_get_hw_resource (void)
{
    unsigned long flags;
    unsigned int index = 0xffffffff;
    unsigned int i;

    do
    {
        spin_lock_irqsave(&zacc_hw_lock, flags);

        for (i = 0; i < ZACC_QUEUE_SIZE; i++)
        {
            if (zacc_hw_resource[i] == 0)
            {
                index = i;
                zacc_hw_resource[i] = 1;
                break;
            }
        }

        spin_unlock_irqrestore(&zacc_hw_lock, flags);
    }
    while (index == 0xffffffff);
    return index;
}

void zacc_put_hw_resource(unsigned int index)
{
    unsigned long flags;
    spin_lock_irqsave(&zacc_hw_lock, flags);

    if (zacc_hw_resource[index] == 1)
    {
        zacc_hw_resource[index] = 0;
    }

    spin_unlock_irqrestore(&zacc_hw_lock, flags);
}


static int enc_cmd_parser ( struct zacc_encode_desc *enc_desc, dma_addr_t src_pa, dma_addr_t dst_pa )
{
    if(NULL==enc_desc)
    {
        return 1;
    }

    if(enc_desc->encode!=1)
    {
        printk_always("%s %d  encode=%d\n",__func__,__LINE__,enc_desc->encode);
        return 1;
    }

    if(enc_desc->cmd_err)
    {
        printk_always("%s %d  cmd_err\n",__func__,__LINE__);
        return 1;
    }

    if(enc_desc->over_4k)
    {
        //printk_always("%s %d  over_4k\n",__func__,__LINE__);
        return 0;
    }

    if(enc_desc->unused_1 || enc_desc->unused_2)
    {
        printk_always("%s %d  unused_1=%d unused_2=%d\n",__func__,__LINE__,enc_desc->unused_1, enc_desc->unused_2);
        return 1;
    }

    if(enc_desc->src_addr!=(src_pa >> ZACC_ADDR_ALIGN))
    {
        printk_always("%s %d  src_addr=%x   src_pa>>%d=%x (%x)\n",
                        __func__,__LINE__,enc_desc->src_addr,ZACC_ADDR_ALIGN,(src_pa>>ZACC_ADDR_ALIGN),src_pa);
        return 1;
    }

    if(enc_desc->dst_addr!=(dst_pa >> ZACC_ADDR_ALIGN))
    {
        printk_always("%s %d  dst_addr=%x   dst_pa>>%d=%x (%x)\n",
                        __func__,__LINE__,enc_desc->dst_addr,ZACC_ADDR_ALIGN,(dst_pa>>ZACC_ADDR_ALIGN),dst_pa);
        return 1;
    }

    return 0;
}

static int dec_cmd_parser ( struct zacc_decode_desc *dec_desc, dma_addr_t src_pa, dma_addr_t dst_pa, size_t  comp_size )
{
    if(NULL==dec_desc)
    {
        return 1;
    }

    if(dec_desc->decode!=1)
    {
        printk_always("%s %d  decode=%d\n\n\n",__func__,__LINE__,dec_desc->decode);
        return 1;
    }

    if(dec_desc->cmd_err)
    {
        printk_always("%s %d  cmd_err\n\n\n",__func__,__LINE__);
        return 1;
    }

    if(dec_desc->size_err)
    {
        printk_always("%s %d  size_err\n\n\n",__func__,__LINE__);
        return 1;
    }

    if(dec_desc->decode_err)
    {
        printk_always("%s %d  decode_err\n\n\n",__func__,__LINE__);
        return 1;
    }

    if(dec_desc->dist_err)
    {
        printk_always("%s %d  dist_err\n\n\n",__func__,__LINE__);
        return 1;
    }

    if(dec_desc->crc_err)
    {
        printk_always("%s %d  crc_err\n\n\n",__func__,__LINE__);
        return 1;
    }

#ifndef ZACC_8G_DRAM_SUPPORT
    if(dec_desc->unused_0)
    {
        printk_always("%s %d  unused_0\n\n\n",__func__,__LINE__);
        return 1;
    }
#endif

    if(dec_desc->decomp_size!=PAGE_SIZE)
    {
        printk_always("%s %d  decomp_size=%x\n\n\n",__func__,__LINE__,dec_desc->decomp_size);
        return 1;
    }

    if(dec_desc->src_addr!=(src_pa >> ZACC_ADDR_ALIGN))
    {
        printk_always("%s %d  src_addr=%x   src_pa>>%d=%x (%x)\n",
                        __func__,__LINE__,dec_desc->src_addr,ZACC_ADDR_ALIGN,(src_pa>>ZACC_ADDR_ALIGN),src_pa);
        return 1;
    }

    if(dec_desc->dst_addr!=(dst_pa >> ZACC_ADDR_ALIGN))
    {
        printk_always("%s %d  dst_addr=%x   dst_pa>>%d=%x (%x)\n",
                        __func__,__LINE__,dec_desc->dst_addr,ZACC_ADDR_ALIGN,(dst_pa>>ZACC_ADDR_ALIGN),dst_pa);
        return 1;
    }

    if(dec_desc->comp_size!=comp_size )
    {
        printk_always("%s %d  comp_size=%x  %x\n\n\n",__func__,__LINE__,dec_desc->comp_size,comp_size);
        return 1;
    }

    return 0;
}

ssize_t zacc_encode (void *dst, void const *src)
{
    struct zacc_encode_desc *encode_desc;
    //struct zacc_encode_desc tmp_encode_desc;
    dma_addr_t dst_pa, src_pa, desc_pa ;
    //dma_addr_t _dst, _src, ;
    unsigned int err, index;
    unsigned int comp_size, aligned_size, crc, err_count, timeout, cmd_status;
    bool need_recovery = false;
    unsigned int *ptr;

#if (ZACC_HW_DONE_EN == 1)
    unsigned int hw_done_count;
#endif

    if (!dst || !src)
    {
        return -EINVAL;
    }

    preempt_disable();

    //rtd_pr_misc_err("%s %d\n",__func__,__LINE__);
    index = zacc_get_hw_resource();
    //desc_pa = desc_uncached+index;
    desc_pa = desc_phys+index*sizeof(struct zacc_desc);

#ifdef ZACC_8G_DRAM_SUPPORT
    dst_pa=par_64((unsigned long)dst,0)&0x1FFFFF000ULL+((unsigned long)dst&0xFFF);//page_to_phys(src_pg);
    src_pa=par_64((unsigned long)src,0)&0x1FFFFF000ULL+((unsigned long)src&0xFFF);//page_to_phys(src_pg);
#else
    dst_pa=par_64((unsigned long)dst,0)&0xFFFFF000+((unsigned long)dst&0xFFF);//page_to_phys(src_pg);
    src_pa=par_64((unsigned long)src,0)&0xFFFFF000+((unsigned long)src&0xFFF);//page_to_phys(src_pg);
#endif
    dmac_flush_range(src,src+PAGE_SIZE);
    dmac_inv_range(dst,dst+PAGE_SIZE);

    encode_desc = (struct zacc_encode_desc *)((struct zacc_desc*)desc+index);
    memset((void *)encode_desc, 0, ZACC_DESC_SIZE);
    encode_desc->encode = 1;

#ifdef ZACC_8G_DRAM_SUPPORT
    encode_desc->src_dec_unit_sel = 1;
#endif
    encode_desc->dst_addr = dst_pa >> ZACC_ADDR_ALIGN;
    encode_desc->src_addr = src_pa >> ZACC_ADDR_ALIGN;
    dmac_flush_range((void*)encode_desc,(void*)((struct zacc_desc*)encode_desc+1));

#ifdef ZACC_8G_DRAM_SUPPORT
    rtd_outl(LZMA_CMD_0_BASE_reg +  reg_offset[index], (desc_pa)|((desc_pa>>32)&0x1));
    rtd_outl(LZMA_CMD_0_LIMIT_reg + reg_offset[index], (desc_pa + 0x20)|(((desc_pa + 0x20)>>32)&0x1));
    rtd_outl(LZMA_CMD_0_RPTR_reg +  reg_offset[index], (desc_pa)|((desc_pa>>32)&0x1));
    rtd_outl(LZMA_CMD_0_WPTR_reg +  reg_offset[index], (desc_pa + 0x10)|(((desc_pa + 0x10)>>32)&0x1));
#else
    rtd_outl(LZMA_CMD_0_BASE_reg +  reg_offset[index], desc_pa);
    rtd_outl(LZMA_CMD_0_LIMIT_reg + reg_offset[index], (desc_pa + 0x20));
    rtd_outl(LZMA_CMD_0_RPTR_reg +  reg_offset[index], desc_pa);
    rtd_outl(LZMA_CMD_0_WPTR_reg +  reg_offset[index], (desc_pa + 0x10));
#endif

#if (ZACC_HW_DONE_EN == 1)
    hw_done_count = rtd_inl(LZMA_CMD_0_dec_reg + reg_offset[index]) & 0x0f;
#endif
    wmb();

    rtd_outl(LZMA_CMD_GO_SWAP_reg, 0x04000000 | (0xf << (0x6 * index)));
    wmb();

    err_count = 0;
    timeout = ZACC_ENC_TIMEOUT_COUNT1;
    while (1)
    {
        dmac_inv_range((void*)encode_desc,(void*)((struct zacc_desc*)encode_desc+1));

        cmd_status = rtd_inl(LZMA_CMD_GO_SWAP_reg);
        if (((cmd_status & (0x1 << (0x6 * index))) == 0) && encode_desc->hw_done
#if (ZACC_HW_DONE_EN == 1)
                && (hw_done_count != (rtd_inl(LZMA_CMD_0_dec_reg + reg_offset[index]) & 0x0f))
#endif
           )
        {
            break;
        }

        if (++err_count >= timeout)
        {
            need_recovery = true;
            printk_always("%s %d  timeout 111    go bit status =%x  index=%d   0x1 0x40 0x1000 0x40000\n",__func__,__LINE__,rtd_inl(LZMA_CMD_GO_SWAP_reg),index);
#if (ZACC_HW_DONE_EN == 1)
            printk_always("%s %d  timeout 111    hw_done_count =%x  %x\n",__func__,__LINE__,hw_done_count,(rtd_inl(LZMA_CMD_0_dec_reg + reg_offset[index]) & 0x0f));
#endif
            break;
        }
        udelay(1);
    }

    //memcpy((void *)&tmp_encode_desc, (void *)encode_desc, ZACC_DESC_SIZE);
    if(enc_cmd_parser(encode_desc, src_pa, dst_pa))
    {
        printk_always("%s %d  error\n\n\n",__func__,__LINE__);
        need_recovery = true;
    }

    comp_size = encode_desc->comp_size;
    aligned_size = ((comp_size + 0xf) >> 4) << 4;
    if((encode_desc->over_4k)||( (aligned_size+16) >= PAGE_SIZE))
    {
        aligned_size = PAGE_SIZE;
    }

    if (need_recovery)
    {
#ifdef ZACC_ENABLE_RECOVERY
        zacc_enc_recovery();
#else
        printk_always("src_pa = %lx, dst_pa = %lx\n", (unsigned long)src_pa, (unsigned long)dst_pa);
        panic("rtk-zacc encode error !! desc(0x%lx): 0x%x, 0x%x, 0x%x, 0x%x\n",
              (unsigned long)encode_desc, ((struct zacc_desc*)encode_desc)->data[0], ((struct zacc_desc*)encode_desc)->data[1],
              ((struct zacc_desc*)encode_desc)->data[2], ((struct zacc_desc*)encode_desc)->data[3]);
#endif
    }

#if (ZACC_HW_DONE_EN == 0)
    udelay(1);
#endif

    //memset(desc, 0, ZACC_DESC_SIZE << 2);
    //dma_unmap_page(dev, dst_pa, PAGE_SIZE, DMA_FROM_DEVICE);
    //dma_unmap_page(dev, src_pa, PAGE_SIZE, DMA_TO_DEVICE);
    dmac_inv_range(dst,dst+PAGE_SIZE);

    rmb();

    if (aligned_size == PAGE_SIZE)
    {
        memcpy(dst, src, PAGE_SIZE);
        err = PAGE_SIZE;
    }
    else
    {
        crc = encode_desc->crc;
        ((unsigned char *)dst)[aligned_size + 3] = 0xFF;
        ((unsigned char *)dst)[aligned_size + 2] = aligned_size - comp_size;
        ((unsigned char *)dst)[aligned_size + 1] = (crc >> 8) & 0xff;
        ((unsigned char *)dst)[aligned_size] = crc & 0xff;

        ptr=dst+aligned_size+4;
#ifdef ZACC_CRC32_ENABLE
        *ptr=calc_crc32(dst, aligned_size);
#else
        *ptr=ZACC_MAGIC_1;
#endif

        ptr=dst+aligned_size+8;
        *ptr=ZACC_MAGIC_2_1;
        *(++ptr)=ZACC_MAGIC_3;

        aligned_size += 16;
        err = aligned_size;
    }

    zacc_put_hw_resource(index);
    preempt_enable();
    return err;
}


ssize_t zacc_decode (void *dst, void const *src, size_t size)
{
    struct zacc_decode_desc *decode_desc;
    dma_addr_t dst_pa, src_pa, _src, desc_pa;
    //dma_addr_t _dst;
    int err=PAGE_SIZE, index;
    unsigned int padding_size, err_count, timeout, cmd_status, retry_count=0;
    //bool do_retry = false, do_recovery = false;
    //unsigned long flags;
    unsigned int *ptr;

#if (ZACC_HW_DONE_EN == 1)
    unsigned int hw_done_count;
#endif

    if (!dst || !src || ((unsigned long)src&0x3) || ((unsigned long)dst&0x3) )
    {
        return -EINVAL;
    }

    if (size == PAGE_SIZE)
    {
        memcpy(dst, src, PAGE_SIZE);
        return PAGE_SIZE;
    }

    preempt_disable();

    //rtd_pr_misc_err("%s %d\n",__func__,__LINE__);
    index = zacc_get_hw_resource();
    //desc_pa = (struct zacc_desc *)desc_uncached+index;
    desc_pa = desc_phys+index*sizeof(struct zacc_desc);

#if 0
    dmac_flush_range(src,src+size);
    src_pa=par_64(src,0)+((unsigned int)(src)&0xFFF);//page_to_phys(src_pg);
#else
    _src=(dma_addr_t)page_address(src_tmp_pg[index]);
    memcpy((void*)_src,src,size);

    dmac_flush_range((void*)_src,(void*)(_src+size));
    src_pa= page_to_phys(src_tmp_pg[index]);
#endif

retry:
    dmac_inv_range(dst,dst+PAGE_SIZE);

#ifdef ZACC_8G_DRAM_SUPPORT
    dst_pa=par_64((unsigned long)dst,0)&0x1FFFFF000ULL+((unsigned long)dst&0xFFF);//page_to_phys(src_pg);
#else
    dst_pa=par_64((unsigned long)dst,0)&0xFFFFF000+((unsigned long)dst&0xFFF);//page_to_phys(src_pg);
#endif

    decode_desc = (struct zacc_decode_desc *)((struct zacc_desc*)desc+index);
    memset((void *)decode_desc, 0, ZACC_DESC_SIZE);
    decode_desc->decode = 1;
#ifdef ZACC_8G_DRAM_SUPPORT
    decode_desc->src_dec_unit_sel = 1;
#endif
    decode_desc->dst_addr = dst_pa >> ZACC_ADDR_ALIGN;
    decode_desc->src_addr = src_pa >> ZACC_ADDR_ALIGN;

    padding_size = 16 + ((unsigned char *)src)[size - 14];
    decode_desc->comp_size = size - padding_size;
    decode_desc->decomp_size = PAGE_SIZE;
    decode_desc->crc_msb = ((unsigned char *)src)[size - 15];
    decode_desc->crc_lsb = ((unsigned char *)src)[size - 16];
    dmac_flush_range((void*)decode_desc,(void*)((struct zacc_desc*)decode_desc+1));

    ptr=(unsigned int*)(((unsigned char *)src)+size-12);

#ifdef ZACC_CRC32_ENABLE
    if(ptr[0]!=calc_crc32((unsigned char *)src, size-16))
    {
        printk_always("%s %d.  enc crc check fail!   0x%08x 0x%08x\n",__func__,__LINE__,ptr[0],calc_crc32((unsigned char *)src, size-16));
        dump_decode_data(src,size,__LINE__);
    }
#endif

    if(ptr[1]!=ZACC_MAGIC_2_1 || ptr[2]!=ZACC_MAGIC_3
#ifndef ZACC_CRC32_ENABLE
            || ptr[0]!=ZACC_MAGIC_1
#endif
      )
    {
        printk_always("%s %d.  magic check fail!   0x%08x 0x%08x 0x%08x\n",__func__,__LINE__,ptr[0],ptr[1],ptr[2]);
        dump_decode_data(src,size,__LINE__);
    }

#ifdef ZACC_8G_DRAM_SUPPORT
    rtd_outl(LZMA_CMD_0_BASE_reg +  reg_offset[index], desc_pa | ((desc_pa>>32)&0x1) );
    rtd_outl(LZMA_CMD_0_LIMIT_reg + reg_offset[index], (desc_pa + 0x20) | (((desc_pa + 0x20)>>32)&0x1) );
    rtd_outl(LZMA_CMD_0_RPTR_reg +  reg_offset[index], desc_pa | ((desc_pa>>32)&0x1) );
    rtd_outl(LZMA_CMD_0_WPTR_reg +  reg_offset[index], (desc_pa + 0x10) | (((desc_pa + 0x10)>>32)&0x1) );
#else
    rtd_outl(LZMA_CMD_0_BASE_reg + reg_offset[index], desc_pa);
    rtd_outl(LZMA_CMD_0_LIMIT_reg + reg_offset[index], (desc_pa + 0x20));
    rtd_outl(LZMA_CMD_0_RPTR_reg + reg_offset[index], desc_pa);
    rtd_outl(LZMA_CMD_0_WPTR_reg + reg_offset[index], (desc_pa + 0x10));
#endif

#if (ZACC_HW_DONE_EN == 1)
    hw_done_count = rtd_inl(LZMA_CMD_0_dec_reg + reg_offset[index]) & 0x0f;
#endif

    wmb();
    rtd_outl(LZMA_CMD_GO_SWAP_reg, 0x04000000 | (0xf << (0x6 * index)));
    wmb();

    err_count = 0;
    timeout = ZACC_DEC_TIMEOUT_COUNT1;

    while (1)
    {
        dmac_inv_range((void*)decode_desc,(void*)((struct zacc_desc*)decode_desc+1));

        cmd_status = rtd_inl(LZMA_CMD_GO_SWAP_reg);
        if (((cmd_status & (0x1 << (0x6 * index))) == 0) && (decode_desc->hw_done)
#if (ZACC_HW_DONE_EN == 1)
                && (hw_done_count != (rtd_inl(LZMA_CMD_0_dec_reg + reg_offset[index]) & 0x0f))
#endif
           )
        {
            break;
        }

        if (++err_count >= timeout)
        {
            console_loglevel=6;
            printk_always("%s %d  timeout 111    go bit status =%x  index=%d   0x1 0x40 0x1000 0x40000\n",__func__,__LINE__,rtd_inl(LZMA_CMD_GO_SWAP_reg),index);
#if (ZACC_HW_DONE_EN == 1)
            printk_always("%s %d  timeout 111    hw_done_count =%x  %x\n",__func__,__LINE__,hw_done_count,(rtd_inl(LZMA_CMD_0_dec_reg + reg_offset[index]) & 0x0f));
#endif
            break;
        }

        udelay(1);
    }

    //memcpy((void *)&tmp_decode_desc, (void *)decode_desc, ZACC_DESC_SIZE);
    if (dec_cmd_parser(decode_desc,src_pa,dst_pa,(size - padding_size)))
    {
        if (retry_count<ZACC_DEC_MAX_RETRY_COUNT)
        {
            retry_count++;
            printk_always("%s %d    %d-%d,  retry %d ... \n",    __func__,__LINE__,err_count,timeout,retry_count);
            goto retry;
        }
        else
        {
            printk_always("%s %d    %d-%d,  retry %d error ! \n",__func__,__LINE__,err_count,timeout,retry_count);
            dump_decode_data(src,size,__LINE__);
        }
    }

#if (ZACC_HW_DONE_EN == 0)
    udelay(1);
#endif

    dmac_inv_range(dst,dst+PAGE_SIZE);
    rmb();

    zacc_put_hw_resource(index);
    preempt_enable();

    return err;
}


//#include <linux/module.h>
//#include <linux/kernel.h>

#include <linux/crypto.h>
#include <linux/init.h>
//#include <linux/zacc.h>
static int zacc_alg_init(struct crypto_tfm *tfm)
{
    return 0;
}

static void zacc_alg_exit(struct crypto_tfm *tfm)
{
}

static int zacc_alg_compress (struct crypto_tfm *tfm, u8 const *src, unsigned int slen, u8 *dst, unsigned int *dlen)
{
    ssize_t rv = zacc_encode(dst, src);
    /*
     * On any error-return from zacc_encode, just pretend not to be compressed at all.
     * This may invoke zcomp to memcpy the page.
     */
    *dlen = (rv < 0) ? PAGE_SIZE : (unsigned int)rv;
    return 0;
}

static int zacc_alg_decompress (struct crypto_tfm *tfm, u8 const *src, unsigned int slen, u8 *dst, unsigned int *dlen)
{
    ssize_t rv = zacc_decode(dst, src, slen);
    /* Return 0 on success, or pass any error-return. */
    return (rv < 0) ? (int)rv : 0;
}

static struct crypto_alg zacc_alg =
{
    .cra_list       = LIST_HEAD_INIT(zacc_alg.cra_list),
    .cra_name       = "zacc",
    .cra_driver_name = "rtk-zacc",
    .cra_flags      = CRYPTO_ALG_TYPE_COMPRESS,
    .cra_compress.coa_compress      = zacc_alg_compress,
    .cra_compress.coa_decompress    = zacc_alg_decompress,
    .cra_init       = zacc_alg_init,
    .cra_exit       = zacc_alg_exit,
    .cra_module     = THIS_MODULE,
};

static int rtk_zacc_init (void)
{
    rtd_maskl(LZMA_LZMA_DMA_RD_Ctrl_reg, 0xfff0fcfe, (0x2 << 18) | (0x3 << 16) | (0x3 << 8) | (0x1 << 0));
    rtd_maskl(LZMA_LZMA_DMA_WR_Ctrl_reg, 0xfffcfcfe, (0x2 << 16) | (0x3 << 8) | (0x1 << 0));
    rtd_maskl(LZMA_INT_STATUS_reg, 0x3fffffdf, (ZACC_HW_DONE_EN << 31) | (0x0 << 30) | (0x1 << 5));
    rtd_maskl(LZMA_LZMA_DUMMY_reg, 0xbfffffff, 0x1 << 30);
    return 0;
}


#ifdef ZACC_PERF_ENABLE
#include <linux/cdev.h>

#define NUM_LOOPS       128
#define BUFF_OFFS       4

enum lzo_perf_minor
{
#if 0
    LZO_0 = 0,
    LZO_1,
    LZO_2,
    LZO_3,
    LZ4_0,
    LZ4_1,
    LZ4_2,
    LZ4_3,
#endif
    ZACC_0,
    ZACC_1,
    ZACC_2,
    ZACC_3,
    NUM_MINOR
};

struct lzo_perf_drv
{
    char const *name;

    size_t work_size;

    int (*compress)(u8 const *, size_t, u8 *, size_t *, void *);
    int (*decompress)(u8 const *, size_t, u8 *, size_t *);
};

struct lzo_perf_stat
{
    char const *name;

    u64 bytes_comp;
    u64 bytes_decomp;

    u64 nsecs_comp;
    u64 nsecs_decomp;

    bool busy;

    struct mutex mutex;
};

static struct lzo_perf_stat lzo_perf_stat[NUM_MINOR] =
{
#if 0
    { .name = "lzo-perf0", },
    { .name = "lzo-perf1", },
    { .name = "lzo-perf2", },
    { .name = "lzo-perf3", },
    { .name = "lz4-perf0", },
    { .name = "lz4-perf1", },
    { .name = "lz4-perf2", },
    { .name = "lz4-perf3", },
#endif
    { .name = "zacc-perf0", },
    { .name = "zacc-perf1", },
    { .name = "zacc-perf2", },
    { .name = "zacc-perf3", },
};

static int zacc_compress (u8 const *src, size_t ssz, u8 *dst, size_t *dsz, void *priv)
{
    ssize_t rv;
    rv = zacc_encode(dst, src);
    if (rv < 0)
    {
        return (int)rv;
    }

    *dsz = (size_t)rv;
    return 0;
}

static int zacc_decompress (u8 const *src, size_t ssz, u8 *dst, size_t *dsz)
{
    ssize_t rv;
    rv = zacc_decode(dst, src, ssz);
    if (rv < 0)
    {
        return (int)rv;
    }

    *dsz = (size_t)rv;
    return 0;
}

static struct lzo_perf_drv lzo_perf_drv[NUM_MINOR] =
{
#if 0
    {
        .name           = "lzo",
        .work_size      = LZO1X_1_MEM_COMPRESS,
        .compress       = lzo1x_1_compress,
        .decompress     = lzo1x_decompress_safe,
    }, {
        .name           = "lzo",
        .work_size      = LZO1X_1_MEM_COMPRESS,
        .compress       = lzo1x_1_compress,
        .decompress     = lzo1x_decompress_safe,
    }, {
        .name           = "lzo",
        .work_size      = LZO1X_1_MEM_COMPRESS,
        .compress       = lzo1x_1_compress,
        .decompress     = lzo1x_decompress_safe,
    }, {
        .name           = "lzo",
        .work_size      = LZO1X_1_MEM_COMPRESS,
        .compress       = lzo1x_1_compress,
        .decompress     = lzo1x_decompress_safe,
    }, {
        .name           = "lz4",
        .work_size      = LZ4_MEM_COMPRESS,
        .compress       = lz4_compress,
        .decompress     = lz4_decompress_unknownoutputsize,
    }, {
        .name           = "lz4",
        .work_size      = LZ4_MEM_COMPRESS,
        .compress       = lz4_compress,
        .decompress     = lz4_decompress_unknownoutputsize,
    }, {
        .name           = "lz4",
        .work_size      = LZ4_MEM_COMPRESS,
        .compress       = lz4_compress,
        .decompress     = lz4_decompress_unknownoutputsize,
    }, {
        .name           = "lz4",
        .work_size      = LZ4_MEM_COMPRESS,
        .compress       = lz4_compress,
        .decompress     = lz4_decompress_unknownoutputsize,
    },
#endif
    {
        .name           = "zacc",
        .work_size      = 0,
        .compress       = zacc_compress,
        .decompress     = zacc_decompress,
    }, {
        .name           = "zacc",
        .work_size      = 0,
        .compress       = zacc_compress,
        .decompress     = zacc_decompress,
    }, {
        .name           = "zacc",
        .work_size      = 0,
        .compress       = zacc_compress,
        .decompress     = zacc_decompress,
    }, {
        .name           = "zacc",
        .work_size      = 0,
        .compress       = zacc_compress,
        .decompress     = zacc_decompress,
    },
};

struct lzo_perf_desc
{
    struct lzo_perf_drv *drv;
    void *work_buf;

    struct page *data_page;
    struct page *comp_page;

    void *data_buf;
    void *comp_buf;

    spinlock_t lock;
};

static ssize_t lzo_perf_read (struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    struct lzo_perf_stat *stat = &lzo_perf_stat[iminor(file->f_inode)];
    char str[85];
    ssize_t size;

    if (*ppos)
    {
        return 0;
    }

    mutex_lock(&stat->mutex);

    size = sprintf(str, "%s %llu %llu %llu %llu\n", stat->name,
                   stat->bytes_comp, stat->bytes_decomp,
                   stat->nsecs_comp, stat->nsecs_decomp);

    mutex_unlock(&stat->mutex);

    if (count < size)
    {
        return -EINVAL;
    }
    if (copy_to_user(buf, str, size))
    {
        return -EFAULT;
    }

    *ppos += size;

    return size;
}

static ssize_t lzo_perf_write (struct file *file, char const __user *buf, size_t count, loff_t *ppos)
{
    struct lzo_perf_stat *stat = &lzo_perf_stat[iminor(file->f_inode)];
    struct lzo_perf_desc *desc = file->private_data;
    ktime_t comp_time, decomp_time;
    ssize_t written = min(count, (size_t)PAGE_SIZE);
    size_t data_size, comp_size;
    int err, i;

    //rtd_pr_misc_err("[lzo-perf] %s: written size = %d\n", stat->name, written);
    if (copy_from_user(desc->data_buf, buf, written))
    {
        return -EFAULT;
    }

    if (written < PAGE_SIZE)
    {
        memset(desc->data_buf + written, 0, PAGE_SIZE - written);
    }

    spin_lock_irq(&desc->lock);

    memcpy(desc->comp_buf+PAGE_SIZE, desc->data_buf, PAGE_SIZE);

    /* loop compression */
    comp_time = ktime_get();
    for (i = 0; i < NUM_LOOPS; i++)
    {
        comp_size = PAGE_SIZE * 2;
        err = desc->drv->compress(desc->data_buf, PAGE_SIZE,
                                  desc->comp_buf, &comp_size,
                                  desc->work_buf);
        if (err < 0)
        {
            panic("lzma compress err! %d\n",err);

            /* compression failed -- memcpy */
            memcpy(desc->comp_buf, desc->data_buf, PAGE_SIZE);
            comp_size = PAGE_SIZE;
        }
    }
    comp_time = ktime_sub(ktime_get(), comp_time);

    spin_unlock_irq(&desc->lock);

#if 0
    if (comp_size != PAGE_SIZE)
    {
        memmove(desc->comp_buf + BUFF_OFFS, desc->comp_buf, comp_size);
    }
#endif

    spin_lock_irq(&desc->lock);

    /* loop decompression */
    decomp_time = ktime_get();
    for (i = 0; i < NUM_LOOPS; i++)
    {
        if (comp_size == PAGE_SIZE)
        {
            /* compression failed -- memcpy */
            memcpy(desc->data_buf, desc->comp_buf, PAGE_SIZE);
            data_size = PAGE_SIZE;
            continue;
        }

        data_size = PAGE_SIZE;
        //err = desc->drv->decompress(desc->comp_buf + BUFF_OFFS, comp_size,
        err = desc->drv->decompress(desc->comp_buf, comp_size, desc->data_buf, &data_size);
        if (err < 0)
        {
            panic("failed to decompress data: %d\n", err);
            break;
        }
    }
    decomp_time = ktime_sub(ktime_get(), decomp_time);

    if(memcmp(desc->data_buf,desc->comp_buf+PAGE_SIZE,PAGE_SIZE))
    {
        console_loglevel=6;
        print_hex_dump(KERN_ERR, "data_buf: ", DUMP_PREFIX_ADDRESS, 16, 1, desc->data_buf, 0x100, true);
        print_hex_dump(KERN_ERR, "data_buf+PAGE_SIZE: ", DUMP_PREFIX_ADDRESS, 16, 1, desc->comp_buf+PAGE_SIZE, 0x100, true);
        panic("decompress data error!\n");
    }

    spin_unlock_irq(&desc->lock);

    mutex_lock(&stat->mutex);

    /* update statistics */
    stat->bytes_comp += PAGE_SIZE * NUM_LOOPS;
    stat->bytes_decomp += comp_size * NUM_LOOPS;
    stat->nsecs_comp += ktime_to_ns(comp_time);
    stat->nsecs_decomp += ktime_to_ns(decomp_time);

    mutex_unlock(&stat->mutex);

    *ppos += written;

    return written;
}

static int lzo_perf_init_desc (struct lzo_perf_desc *desc, int minor)
{
    desc->drv = &lzo_perf_drv[minor];

    desc->work_buf = kmalloc(desc->drv->work_size, GFP_KERNEL);
    if (!desc->work_buf)
    {
        return -ENOMEM;
    }

    desc->data_page = alloc_page(GFP_KERNEL);
    if (!desc->data_page)
    {
        return -ENOMEM;
    }
    desc->data_buf = page_address(desc->data_page);

    desc->comp_page = alloc_pages(GFP_KERNEL, 1);
    if (!desc->comp_page)
    {
        return -ENOMEM;
    }
    desc->comp_buf = page_address(desc->comp_page);

    spin_lock_init(&desc->lock);

    return 0;
}

static int lzo_perf_open (struct inode *inode, struct file *file)
{
    struct lzo_perf_stat *stat = &lzo_perf_stat[iminor(inode)];
    struct lzo_perf_desc *desc;
    int err = 0;

    if (!(file->f_mode & FMODE_WRITE))
    {
        goto _return_;
    }

    mutex_lock(&stat->mutex);

    if (stat->busy)
    {
        err = -EBUSY;
        goto _unlock_;
    }

    desc = vzalloc(sizeof(*desc));
    if (!desc)
    {
        err = -ENOMEM;
        goto _unlock_;
    }

    lzo_perf_init_desc(desc, iminor(inode));

    file->private_data = desc;

    /* reset statistics */
    stat->bytes_comp = 0;
    stat->bytes_decomp = 0;
    stat->nsecs_comp = 0;
    stat->nsecs_decomp = 0;

    stat->busy = true;
_unlock_:
    mutex_unlock(&stat->mutex);
_return_:
    return err;
}

static int lzo_perf_release (struct inode *inode, struct file *file)
{
    struct lzo_perf_stat *stat = &lzo_perf_stat[iminor(inode)];
    struct lzo_perf_desc *desc = file->private_data;

    if (!(file->f_mode & FMODE_WRITE))
    {
        goto _return_;
    }

    mutex_lock(&stat->mutex);

    stat->busy = false;

    mutex_unlock(&stat->mutex);

    __free_page(desc->data_page);
    __free_pages(desc->comp_page, 1);
    kfree(desc->work_buf);
    vfree(desc);
_return_:
    return 0;
}

static struct file_operations const lzo_perf_fops =
{
    .owner          = THIS_MODULE,
    .read           = lzo_perf_read,
    .write          = lzo_perf_write,
    .open           = lzo_perf_open,
    .release        = lzo_perf_release,
};

static struct cdev lzo_perf_cdev;
static dev_t lzo_perf_dev;
static int lzo_perf_init (void)
{
    int err, i;

    rtd_pr_misc_err("%s %d\n",__func__,__LINE__);
    err = alloc_chrdev_region(&lzo_perf_dev, 0, NUM_MINOR, "lzo-perf");
    if (err < 0)
    {
        goto _return_;
    }

    cdev_init(&lzo_perf_cdev, &lzo_perf_fops);
    lzo_perf_cdev.owner = THIS_MODULE;

    err = cdev_add(&lzo_perf_cdev, lzo_perf_dev, NUM_MINOR);
    if (err < 0)
    {
        goto _unregister_;
    }

    rtd_pr_misc_err("[lzo-perf] major = %d, NUM_MINOR = %d\n",
           MAJOR(lzo_perf_dev), NUM_MINOR);

    for (i = 0; i < NUM_MINOR; i++)
    {
        mutex_init(&lzo_perf_stat[i].mutex);
    }

    rtd_pr_misc_err("%s %d\n",__func__,__LINE__);
    return 0;

_unregister_:
    unregister_chrdev_region(lzo_perf_dev, NUM_MINOR);
_return_:
    return err;
}

#if 0
static void lzo_perf_exit(void)
{
    cdev_del(&lzo_perf_cdev);
    unregister_chrdev_region(lzo_perf_dev, NUM_MINOR);
}
#endif

#endif


int zacc_suspend (void)
{
    //zacc_stop(zdev);
    return 0;
}

void zacc_resume (void)
{
    //struct zacc_dev *zdev = dev_get_drvdata(dev);
    rtk_zacc_init();
    return ;
}

#include <linux/syscore_ops.h>
static struct syscore_ops zacc_syscore_ops =
{
    .suspend = zacc_suspend,
    .resume = zacc_resume,
};

static int __init zacc_init (void)
{
    int i;
    int err;

#if 0
    static int zacc_init_flag=0;
    if(zacc_init_flag)
    {
        rtd_pr_misc_err("%s %d 22\n",__func__,__LINE__);
        return 0;
    }

    rtd_pr_misc_err("%s %d 11\n",__func__,__LINE__);
#endif

    spin_lock_init(&zacc_hw_lock);
    rtk_zacc_init();
    init_crc32();
    register_syscore_ops(&zacc_syscore_ops);

    err=crypto_register_alg(&zacc_alg);
    if (err)
    {
        rtd_pr_misc_err("%s %d.  zacc register alg error %d!\n",__func__,__LINE__,err);
        return -1;
    }

    //desc=dvr_malloc_uncached(sizeof(struct zacc_desc)*ZACC_QUEUE_SIZE, (void **)&desc_uncached);
    desc_page=(void*)alloc_pages(GFP_DMA32, 0);
    if(NULL==desc_page)
    {
        rtd_pr_misc_err("%s %d.  descriptor alloc fail!\n",__func__,__LINE__);
        goto failed;
    }

    desc=page_address(desc_page);
    desc_phys=page_to_phys(desc_page);

    for (i = 0; i < ZACC_QUEUE_SIZE; i++)
    {
        dst_pg[i] = alloc_pages(GFP_DMA32, 1);
        if (!dst_pg[i])
        {
            goto failed;
        }

        src_pg[i] = alloc_pages(GFP_DMA32, 0);
        if (!src_pg[i])
        {
            goto failed;
        }

        src_tmp_pg[i] = alloc_pages(GFP_DMA32, 0);
        if (!src_tmp_pg[i])
        {
            goto failed;
        }
    }

#ifdef ZACC_PERF_ENABLE
    lzo_perf_init();
#endif

    //zacc_init_flag=1;
    rtd_pr_misc_err("%s %d success!\n",__func__,__LINE__);
    return 0;

failed:
    if(desc_page)
    {
        __free_page(desc_page);
        desc=NULL;
        desc_uncached=NULL;
    }

    for (i = 0; i < ZACC_QUEUE_SIZE; i++)
    {
        if (dst_pg[i])
        {
            __free_pages(dst_pg[i], 1);
            dst_pg[i]=0;
        }

        if (!src_pg[i])
        {
            __free_pages(src_pg[i], 0);
            src_pg[i]=0;
        }

        if (!src_tmp_pg[i])
        {
            __free_pages(src_tmp_pg[i], 0);
            src_tmp_pg[i]=0;
        }
    }

    return -1;
}


#ifdef MODULE
module_init(zacc_init);
#else
late_initcall(zacc_init);
#endif

MODULE_AUTHOR("Alex-KC Yuan <alexkc.yuan@realtekc.com>");
MODULE_DESCRIPTION("Realtek ZACC HW compressor support");
MODULE_LICENSE("GPL");
MODULE_SOFTDEP("pre: zsmalloc zram");

#endif


#if 0
int zacc_4g_test (void);
void zacc_init1 (void)
{
    rtd_pr_misc_err("%s %d\n",__func__,__LINE__);
    zacc_init();
    rtd_pr_misc_err("%s %d\n",__func__,__LINE__);
    zacc_4g_test();
    rtd_pr_misc_err("%s %d\n",__func__,__LINE__);
}

unsigned char tingyu_16bit_1152 []=
{
0x00,0x7e,0xbf,0xf0,0x00,0x20,0x0c,0xa0,0xc7,0x32,0x2a,0x65,0x0d,0xb0,0x36,0x03,
0x3b,0x6c,0x88,0x69,0xcf,0x63,0xf2,0x15,0xa2,0xb3,0xb1,0x9f,0xac,0x67,0xdd,0xc2,
0xc7,0xb0,0x68,0x9e,0x40,0xbc,0x50,0x63,0x8d,0xfb,0x0a,0x57,0x2d,0xa5,0xce,0x55,
0x27,0x62,0x2d,0x79,0xc6,0x7a,0xc5,0xba,0x17,0x6f,0xa7,0x4d,0x21,0x14,0x33,0x2b,
0x11,0xdf,0x95,0xba,0xaf,0xfe,0xa9,0x6d,0x72,0x15,0x8c,0x1c,0xea,0x40,0x91,0x67,
0x79,0x57,0x74,0x11,0xfe,0x64,0x4a,0x32,0x55,0x35,0x51,0xd0,0x2a,0x42,0x20,0x68,
0x02,0xe7,0xfa,0x49,0x3b,0xb0,0x67,0x3b,0x1b,0xa9,0x2e,0xa8,0x82,0x4f,0x80,0x09,
0x86,0x8e,0xcb,0xf1,0xf2,0x17,0x6b,0x42,0x73,0xcb,0x76,0x0c,0x79,0xf8,0x57,0x04,
0xa9,0x96,0xf2,0xcd,0x36,0x88,0x22,0x33,0x10,0x7a,0x9b,0x53,0x1a,0xe3,0x3c,0xbb,
0x50,0xb4,0x21,0x04,0x4c,0xab,0x8d,0x2a,0xea,0x69,0x5f,0x9f,0xde,0x2d,0xbd,0x87,
0x0b,0xd6,0x25,0x0d,0x16,0xd1,0x57,0x42,0x2f,0x81,0x61,0x71,0x09,0x3e,0x81,0x7c,
0x17,0xc5,0xce,0x1d,0xb5,0xb4,0xd9,0x30,0x38,0x2d,0x0a,0x1a,0xe7,0x65,0xfe,0xae,
0xe5,0x87,0xaf,0x88,0x46,0x55,0xab,0xa8,0x39,0x4d,0x5f,0xa0,0xee,0x9f,0x59,0x19,
0xf7,0x67,0x71,0x92,0x27,0x98,0xf0,0xba,0x51,0x67,0x9c,0x80,0xe9,0xa8,0x7d,0x90,
0xe0,0xb5,0x36,0xe1,0x1e,0x3d,0xee,0x81,0x60,0x25,0x29,0xd4,0x19,0xe0,0xe7,0xca,
0x03,0xfd,0xdd,0x16,0xbd,0x91,0x6a,0x4e,0x6f,0x3f,0x0b,0xad,0xa9,0x00,0x7c,0x80,
0x8d,0xc1,0x07,0x30,0xee,0x9c,0xc0,0x39,0x62,0xf5,0x37,0x8b,0x98,0x02,0x44,0xec,
0x2f,0xec,0x80,0x0d,0x0c,0xc7,0xec,0x9a,0x4d,0xd4,0x49,0xa7,0xf3,0xe2,0xa1,0x86,
0xa3,0x54,0x6c,0x17,0x07,0x73,0x72,0xd5,0xd8,0x16,0x90,0x3d,0x36,0x72,0xa9,0x1c,
0xae,0x2c,0x9f,0xf1,0x07,0x65,0xcb,0x81,0xe5,0x8e,0x8a,0x8d,0xb9,0xc9,0x26,0xeb,
0xa3,0xcd,0x19,0x1d,0xa5,0xa5,0x97,0xe8,0xbc,0x3e,0x3a,0x95,0xa6,0x62,0xa7,0x29,
0x51,0x9b,0x5f,0xda,0xde,0x51,0xe8,0xfa,0x2c,0xc2,0x12,0x7d,0xa9,0xb2,0x80,0x08,
0xa1,0xab,0xec,0x91,0x1c,0x51,0xfe,0xf6,0xbb,0x09,0x01,0xbc,0xff,0x0c,0xc7,0x00,
0xc0,0xae,0x6a,0x01,0xc9,0x08,0x2b,0x37,0x84,0xad,0x3d,0x3b,0x1c,0xd4,0xb2,0x60,
0x4c,0x4d,0xea,0xb9,0x8b,0x2e,0x21,0xd0,0xb7,0xaa,0x23,0x61,0x8d,0x9a,0x57,0x57,
0x3e,0xcc,0xc8,0x12,0x80,0xd8,0xea,0x37,0x59,0x02,0x2d,0x45,0x0a,0xe9,0x1c,0xad,
0x38,0x92,0x2c,0xab,0xfa,0x9b,0x22,0xa7,0xc7,0xc7,0xbd,0xb0,0xc3,0xec,0xd0,0xaa,
0x55,0xdc,0x9d,0xdc,0x03,0xe3,0x0a,0xf1,0xa0,0xd1,0x17,0x1b,0x6d,0xb0,0xb0,0xff,
0x4d,0x2f,0x2a,0x08,0x3e,0xd9,0x09,0x91,0xa7,0x70,0x95,0xfe,0xc1,0xfc,0xa4,0x7f,
0x46,0xcc,0x3a,0x9d,0x3e,0xd0,0x86,0x8e,0xf0,0x09,0x07,0xfa,0x0f,0xf5,0xe7,0x71,
0x32,0xaa,0x8c,0x6a,0x18,0x84,0x65,0xb8,0xd1,0x9f,0x8d,0xfa,0x7a,0x5c,0x7c,0x18,
0x66,0xa9,0x39,0x87,0xd7,0x36,0x57,0x24,0x70,0x7b,0x27,0x9a,0x7f,0xff,0xc5,0xcc,
0x99,0x80,0x1c,0x7a,0x9b,0x81,0xb1,0x96,0x54,0x28,0xfc,0x9a,0x2d,0x54,0xf3,0xeb,
0xe3,0x30,0xf3,0xb1,0xf8,0x0c,0xcc,0x41,0x06,0xc2,0xb3,0xdd,0xd6,0x56,0x7d,0xe7,
0x73,0xe3,0x21,0x70,0x3e,0xfe,0x9c,0xea,0xa6,0x5a,0xbb,0x18,0xcb,0xac,0x05,0xea,
0x89,0x98,0x9e,0x03,0x64,0x9c,0x09,0x08,0xb1,0x58,0xaf,0x93,0x53,0x85,0x19,0x71,
0xd2,0x20,0xad,0x3a,0x3b,0x91,0xa3,0x96,0xc5,0xfe,0x03,0xa7,0x60,0x56,0x49,0x35,
0x12,0xe9,0x89,0x90,0xeb,0x95,0xc3,0x5f,0x59,0x86,0x47,0x84,0xc7,0xb9,0xe9,0x43,
0x74,0xd1,0x1c,0xae,0x7e,0x48,0x1a,0x08,0x82,0xea,0xa9,0x00,0x4d,0xd9,0x1f,0x6f,
0x13,0x15,0xa9,0xca,0xc8,0xd9,0x1d,0x0d,0x0e,0xf5,0x13,0x3a,0x3a,0x9b,0x42,0xde,
0x38,0xf0,0xa7,0xf0,0xb3,0x98,0xf6,0xac,0x38,0xf5,0x07,0x2c,0xd6,0x83,0x76,0xe6,
0xe7,0xd6,0x4a,0xe0,0x26,0x73,0xc6,0x7d,0x64,0x10,0x73,0xf2,0x82,0x2e,0x3f,0xc5,
0x23,0xd2,0x9a,0x9b,0x36,0xcf,0xf8,0xd2,0xc1,0x40,0x83,0x76,0xfe,0xa7,0x82,0x2a,
0xc2,0x1e,0x9d,0x80,0x54,0xf5,0x1f,0x0b,0x7b,0x5f,0x97,0x3e,0x69,0x57,0xfe,0x56,
0x4a,0x51,0x2d,0x96,0xc3,0x13,0xcd,0xcf,0xb6,0x4c,0xf3,0x6b,0x21,0x06,0x64,0xe1,
0x22,0x15,0xa5,0xd2,0x31,0x19,0x45,0x15,0x28,0xdb,0x4b,0xea,0xbf,0xc0,0xeb,0xbd,
0x37,0xeb,0x87,0xbf,0x3b,0x38,0x40,0xc7,0x2f,0x69,0xb7,0xff,0x07,0x42,0xc8,0x00,
0xf7,0xea,0x32,0x31,0xec,0x45,0xcd,0xbb,0xba,0xb8,0xed,0x2c,0xa1,0xa6,0x2d,0x47,
0xf9,0x30,0x5b,0x78,0xee,0x96,0xf5,0x6b,0x39,0x58,0x6b,0x8b,0xd1,0xce,0x8a,0x1e,
0xd6,0x81,0x98,0xbf,0x96,0xc6,0x49,0x7f,0xd9,0x68,0x9d,0xc0,0x68,0xfa,0x5f,0x90,
0x4d,0xca,0x8c,0xef,0x3e,0xff,0xb1,0xb8,0x03,0x0f,0x6f,0x4c,0x8a,0xc2,0xb1,0xcb,
0x76,0xe6,0xde,0xb9,0x16,0x5f,0x53,0x62,0x7f,0xd2,0x80,0xc5,0x96,0x3f,0x77,0x26,
0x14,0x66,0x2c,0xa0,0x26,0x91,0x2b,0x3b,0x62,0x8b,0xd1,0xd2,0x85,0x5a,0xd1,0x04,
0xb5,0x4a,0xdf,0x2c,0xb9,0x1a,0x4f,0xb9,0x54,0xd0,0x7a,0x99,0x99,0xb1,0xfd,0x5f,
0xd4,0x62,0xd5,0xce,0xbb,0xcd,0x07,0x7b,0x13,0xb9,0x07,0xc2,0x40,0xbc,0x42,0xbb,
0xab,0x89,0xb7,0xf6,0xa3,0x0e,0xe5,0x27,0x75,0x28,0xd3,0x1b,0xde,0x63,0x6f,0x4f,
0x57,0xd1,0xac,0x36,0x1f,0xa0,0x2e,0xcb,0x0e,0x7e,0x6a,0xf4,0x8b,0xd8,0xbb,0x4f,
0x6e,0x6a,0xa8,0xff,0x96,0x55,0x72,0x12,0xe7,0xab,0xdb,0x95,0xcf,0xee,0x2c,0x6b,
0x78,0x19,0xde,0x0f,0x9b,0xd3,0x31,0xad,0x2a,0x0d,0xf1,0xe0,0x40,0x97,0x43,0xb3,
0xcd,0xdd,0x2b,0x2f,0x53,0x16,0x32,0xf6,0xb8,0x2d,0x52,0xce,0x61,0xd5,0x79,0x68,
0x20,0x6c,0x1b,0x4f,0xe9,0x79,0x67,0xcb,0xce,0x52,0xff,0x89,0x83,0x29,0x1d,0x74,
0xf1,0x80,0x7d,0x81,0x38,0x5e,0x68,0x21,0x51,0x81,0x81,0xeb,0x76,0x61,0x07,0x30,
0x59,0x5e,0xc0,0x20,0x29,0xe1,0x14,0x0d,0x0d,0x7c,0xa4,0xba,0x91,0x60,0xfd,0x4f,
0xa9,0x67,0x27,0x33,0x56,0x93,0x2d,0x95,0xd2,0x37,0xaf,0xfc,0x8c,0x52,0xfd,0xc7,
0xc6,0x5d,0x76,0x8b,0x69,0x02,0x24,0x88,0x6a,0xf9,0x07,0x52,0xd9,0xfe,0xe9,0xdf,
0x64,0xbe,0x16,0xf2,0xa2,0x8c,0xc6,0x4d,0xf2,0x93,0xf2,0xfa,0x89,0x93,0x0c,0x97,
0xf2,0x1a,0x55,0x7e,0xd8,0x0e,0x8d,0xf4,0x2d,0x97,0x6c,0x03,0x77,0x4c,0x03,0x1f,
0xf3,0x17,0xcb,0xc6,0x20,0xfc,0x19,0x9c,0xc1,0xe2,0x42,0x69,0xc1,0xab,0x8b,0x18,
0x3e,0xda,0x38,0xca,0xd0,0xed,0xa4,0x04,0x23,0xeb,0x9b,0xe4,0x87,0xdc,0x2a,0x0c,
0xee,0x92,0x6a,0x46,0x0c,0x30,0x9c,0xae,0xea,0x6d,0x6b,0x53,0x2a,0x35,0x3a,0x64,
0x14,0x72,0x03,0x58,0xbc,0x14,0x0c,0xd8,0x0b,0x3c,0xf9,0x00,0x00,0x00,0x00,0x00,
0x66,0x25,0x05,0xff,0x55,0xaa,0x55,0xaa,0xaf,0xbe,0x78,0x23,0x43,0x43,0x41,0x5a,
};

#define printk_always(fmt,...)  do{int loglevel=console_loglevel;console_loglevel=8;rtd_pr_misc_err(fmt,##__VA_ARGS__);console_loglevel=loglevel;}while(0)
int zacc_4g_test (void)
{
    ssize_t size=sizeof(tingyu_16bit_1152);
    struct page *page1=alloc_pages(GFP_KERNEL, 2);
    unsigned char* buf1=(unsigned char *)page_address(page1);
    unsigned char* buf2=(unsigned char *)phys_to_virt(0x1f0000000ULL);
    unsigned char* buf[2]={buf1,buf2};

    unsigned char* src;
    unsigned char* dst;
    int i;

    for(i=0;i<2;i++)
    {

    src=buf[i];
    dst=buf[i]+(2*4096);
    memcpy(src,tingyu_16bit_1152,sizeof(tingyu_16bit_1152));
    printk_always("%s %d   tingyu_16bit_1152 size=%d src=%08lx  dst=%08lx\n",__func__,__LINE__,size,(unsigned long)src,(unsigned long)dst);
    size=zacc_decode(dst,src,sizeof(tingyu_16bit_1152));
    printk_always("%s %d   tingyu_16bit_1152 decode \e[1;31mOK\e[0m\n",__func__,__LINE__);

    src=buf[i]+(2*4096);
    dst=buf[i];
    memset(dst,0,4096);
    memcpy(dst,src,size);
    printk_always("%s %d   tingyu_16bit_1152 size=%d src=%08lx  dst=%08lx\n",__func__,__LINE__,size,(unsigned long)src,(unsigned long)dst);
    size=zacc_encode(dst,src);
    if(memcmp(dst,tingyu_16bit_1152,size))
    {
        printk_always("%s %d   tingyu_16bit_1152 encode \e[1;32mNG\e[0m\n",__func__,__LINE__);
    }
    else
    {
        printk_always("%s %d   tingyu_16bit_1152 encode \e[1;31mOK\e[0m\n",__func__,__LINE__);
    }

    }
    return 0;
}

#endif
