/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright 2012 by Chuck Chen <ycchen@realtek.com>
 *
 * Code common to all OMAP2+ machines.
 */
#include <linux/module.h>

/*ML7BU-1580*/
#include <linux/arm-smccc.h>
#define OEM_SVC_FN_BASE                 0x8300ff00
#define OEM_SVC_FN(n)                   (OEM_SVC_FN_BASE + (n))
#define OEM_SVC_BOOT_KERNEL_64          OEM_SVC_FN(0)
#define OEM_SVC_BOOT_KERNEL_32          OEM_SVC_FN(1)
#define OEM_SVC_RESUME_KERNEL_64        OEM_SVC_FN(2)
#define OEM_SVC_RESUME_KERNEL_32        OEM_SVC_FN(3)
#define OEM_SVC_POWER_OFF_QUICK_SHOW_CPU OEM_SVC_FN(4)
#define OEM_SVC_GIC_DIST_INIT		OEM_SVC_FN(5)

#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>

#include <asm/sections.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/memblock.h>
#include <linux/reboot.h>
#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
#include <linux/dma-map-ops.h>
#else
#include <linux/dma-contiguous.h>
#endif
#ifndef CONFIG_REALTEK_MEMORY_MANAGEMENT_MODULE
#include <linux/rtkblueprint.h>
#include <linux/auth.h>
#endif
#ifdef CONFIG_OF
#include <linux/irqchip.h>
#endif
#include <mach/pcbMgr.h>
#include <rtk_kdriver/rtk-kdrv-common.h>
#include <mach/common.h>
#include <mach/rtk_platform.h>
#include <mach/system.h>
#include <rtk_kdriver/io.h>
#include <mach/timex.h>
#ifdef CONFIG_LG_SNAPSHOT_BOOT
#include "power.h"
#endif
#include <asm/cacheflush.h>
#include <linux/delay.h>
#include <rbus/stb_reg.h>
#include <rbus/emmc_wrap_reg.h>
#include <rtd_log/rtd_module_log.h>
#include <mach/compile.h>
#include <upgrade_ipc.h>

#ifndef CONFIG_ARM64
#include <asm/mach/arch.h>


#include <linux/kernel.h>
#include <linux/init.h>

#include <asm/tlb.h>
#include <asm/mach/map.h>
#include <mach/iomap.h>
#include <mach/platform.h>

static struct map_desc rtd299s_io_desc[] __initdata = {
#if !defined (CONFIG_ARCH_RTK6748)
    {
        .virtual    = GIC_BASE_VIRT,
        .pfn        = __phys_to_pfn(GIC_BASE_PHYS),
        .length     = GIC_BASE_SIZE,
        .type       = MT_DEVICE,
    },
#endif
    {
        .virtual    = RBUS_BASE_VIRT,
        .pfn        = __phys_to_pfn(RBUS_BASE_PHYS),
        .length     = RBUS_BASE_SIZE,
        .type       = MT_DEVICE,
    },
#if 1//defined(CONFIG_REALTEK_RPC) ||defined(CONFIG_RTK_KDRV_RPC)
    {
        .virtual    = RPC_BASE_VIRT,
        .pfn        = __phys_to_pfn(RPC_BASE_PHYS),
        .length     = RPC_BASE_SIZE,
        .type       = MT_UNCACHED,
    },
#endif
};


void __init gic_init_irq(void)
{
    irqchip_init();
}

void __init rtk_map_io(void)
{
	rtd_pr_mach_rtk_info("%s:%d\n", __func__, __LINE__);
	iotable_init(rtd299s_io_desc, ARRAY_SIZE(rtd299s_io_desc));
}

static const char * const rtk_board_compat[] = {
#ifdef CONFIG_ARCH_RTK2851A
	"rtk,rtd2841a",
#elif defined(CONFIG_ARCH_RTK2885P)
	"rtk,rtd2885p",
#elif defined(CONFIG_ARCH_RTK2851F)
	"rtk,rtd2851f",
#elif defined(CONFIG_ARCH_RTK6748)
	"rtk,rtd6748",
#endif
	NULL
};

#ifdef CONFIG_ARCH_RTK2851A
DT_MACHINE_START(RTK2851A, "rtd2841a")
#elif defined(CONFIG_ARCH_RTK2885P)
DT_MACHINE_START(RTK2885P, "rtd2885p")
#elif defined(CONFIG_ARCH_RTK2851F)
DT_MACHINE_START(RTK2851F, "rtd2851f")
#elif defined(CONFIG_ARCH_RTK6748)
DT_MACHINE_START(RTK6748, "rtd6748")
#endif
#ifdef CONFIG_SMP
//   .smp          = smp_ops(rtk_smp_ops), //FIXME: need check
#endif
//      .nr_irqs      = 0,      //FIXME: need check
#if (defined(RTK_MEM_LAYOUT_DEVICETREE) || defined(CONFIG_REALTEK_MEMORY_MANAGEMENT_MODULE)) && !defined(CONFIG_ARCH_RTK2851A)
        .reserve      = NULL,
#else
        .reserve      = rtk_reserve,
#endif
        .map_io       = rtk_map_io,
//   .init_early   = rtk_init_early, //NO need on current arm32
        .init_irq     = gic_init_irq,
//   .init_machine = rtk_init_machine,
//   .init_late    = rtk_init_late,
//  .restart      = rtk_machine_restart,
        .dt_compat    = rtk_board_compat,
MACHINE_END


#endif


#if 0
struct kobject *realtek_boards_kobj;
EXPORT_SYMBOL(realtek_boards_kobj);
#endif

#ifndef CONFIG_REALTEK_MEMORY_MANAGEMENT_MODULE
extern int is_ddr_swap(void);

#if IS_ENABLED(CONFIG_REALTEK_LOGBUF)
__attribute__((weak)) unsigned long rtdlog_get_buffer_size(void)
{
        return 0;
}
#endif
#endif

#if defined(RTK_MEM_LAYOUT_DEVICETREE) || defined(CONFIG_REALTEK_MEMORY_MANAGEMENT_MODULE)

    // no need.  

#else

#ifdef CONFIG_OF_FLATTREE

#ifndef MIN_MEMBLOCK_ADDR
#define MIN_MEMBLOCK_ADDR	__pa(PAGE_OFFSET)
#endif
#ifndef MAX_MEMBLOCK_ADDR
#define MAX_MEMBLOCK_ADDR	((phys_addr_t)~0)
#endif
// weak symbol
void __init early_init_dt_add_memory_arch(u64 base, u64 size)
{
	const u64 phys_offset = MIN_MEMBLOCK_ADDR;
    static int cnt = 0;

	if (size < PAGE_SIZE - (base & ~PAGE_MASK)) {
		rtd_pr_mach_rtk_warn("Ignoring memory block 0x%llx - 0x%llx\n",
			base, base + size);
		return;
	}

	if (!PAGE_ALIGNED(base)) {
		size -= PAGE_SIZE - (base & ~PAGE_MASK);
		base = PAGE_ALIGN(base);
	}
	size &= PAGE_MASK;

	if (base > MAX_MEMBLOCK_ADDR) {
		rtd_pr_mach_rtk_warn("Ignoring memory block 0x%llx - 0x%llx\n",
				base, base + size);
		return;
	}

	if (base + size - 1 > MAX_MEMBLOCK_ADDR) {
		rtd_pr_mach_rtk_warn("Ignoring memory range 0x%llx - 0x%llx\n",
				((u64)MAX_MEMBLOCK_ADDR) + 1, base + size);
		size = MAX_MEMBLOCK_ADDR - base + 1;
	}

	if (base + size < phys_offset) {
		rtd_pr_mach_rtk_warn("Ignoring memory block 0x%llx - 0x%llx\n",
			   base, base + size);
		return;
	}
	if (base < phys_offset) {
		rtd_pr_mach_rtk_warn("Ignoring memory range 0x%llx - 0x%llx\n",
			   base, phys_offset);
		size -= phys_offset - base;
		base = phys_offset;
	}

    rtd_pr_mach_rtk_info("mem=%lldMB @ 0x%08llx\n", size / 1024 / 1024, base);
    if (!cnt++)
        add_memory_size(GFP_DCU1, size);
    else
        add_memory_size(GFP_DCU2, size);

    DRAM_size += (size >> PAGE_SHIFT);
	memblock_add(base, size);
}
#endif


#include <linux/of_fdt.h>
int __initdata          rtk_dt_cma_addr_cells = 2;
int __initdata          rtk_dt_cma_size_cells = 2;
phys_addr_t __initdata  rtk_dt_cma1_addr;
phys_addr_t __initdata  rtk_dt_cma1_size;

static int __init rtk_dt_scan_cma(unsigned long node, const char *uname, int depth, void *data)
{
    static int found;
    int t_len = (rtk_dt_cma_addr_cells + rtk_dt_cma_size_cells) * sizeof(__be32);
    int len;
    const __be32 *prop;
    const char* nodename = "cma1_memory";
    int nl;
    phys_addr_t addr, size;

    // we scan only child nodes of rtk_cma
    if (!found && depth == 1 && strcmp(uname, "rtk_cma") == 0) {
        rtd_pr_mach_rtk_debug("rtk_cma --found! %d[%d] %s \n", node, depth, uname);

        found = 1;
        return 0; // scan next node
    }
    else if (!found) {
        return 0; // scan next node
    }
    else if (found && depth < 2) {
        rtd_pr_mach_rtk_debug("rtk_cma --finished! %d[%d] %s\n", node, depth, uname);

        return 1; // rtk_cma scanning finished
    }

    /*
     * once found, parse 'reg' in child node
     */

    prop = of_get_flat_dt_prop(node, "reg", &len);
    if (!prop) {
        return 0;
    }

    if (len && (len % t_len != 0)) {
        rtd_pr_mach_rtk_err("rtk_cma: invalid reg property in '%s'\n", uname);
        return 0;
    }
    // usually this while-loop only enter once, because only reg < addr size > is acceptable in rtk_cma, 
    //   len can do more validation with #address-cells and #size-cells props.
    while (len >= t_len) {
        addr = of_read_number(prop, rtk_dt_cma_addr_cells); prop += rtk_dt_cma_addr_cells;
        size = of_read_number(prop, rtk_dt_cma_size_cells); prop += rtk_dt_cma_size_cells;

        rtd_pr_mach_rtk_debug("rtk_cma: node '%s': base %pa, size %ld MB\n", uname, &addr, (unsigned long)size / SZ_1M);

        /*
         * could extend for handling specified memory region once dts list full CMA memory layout.
         * but now we only need cma1_memory base/size to update back to carvedout_buf
         *
         */
        nl = strlen(nodename);
        if ((strlen(uname) >= nl) && (('\0' == uname[nl]) || ('@' == uname[nl]))) {
            if (strncmp(uname, nodename, nl) == 0) { // matched
                rtk_dt_cma1_addr = addr;
                rtk_dt_cma1_size = size;

                rtd_pr_mach_rtk_info("rtk_cma: get cma1 region from '%s': base %pa, size %ld MB\n", uname, &addr, (unsigned long)size / SZ_1M);
                break; // currently we only need cma1
            }
        }

        len -= t_len;
    }

    return 0; // scan next node
}

extern struct rtkcma rtkcma_list[];
extern int __init dvr_declare_contiguous(struct device *dev, phys_addr_t size,
				       phys_addr_t base, phys_addr_t limit, const char *name);
void __init rtk_reserve_cma(phys_addr_t dma_limit, phys_addr_t lowmem_limit)
{
    unsigned long size, addr;
	unsigned int layout_idx = carvedout_buf_get_layout_idx();

	rtd_pr_mach_rtk_info("rtk_reserve_cma, lowmem_limit = 0x%lx\n", lowmem_limit);

    if (lowmem_limit >= 0x20000000) {
        /*
         * Dynamically get CARVEDOUT_CMA_LOW from fdt
         * In setup_arch(), unflatten_device_tree() is invoked after arm64_memblock_init().
         * To leave native setup_arch() untouhced, we don't leverage of_find_node_by_xxx() util
         *   for unflatten devcetree but iterating fdt instead.
         */
        // iterator callback for fdt parsing
        of_scan_flat_dt(rtk_dt_scan_cma, NULL);
        if (rtk_dt_cma1_size) {
            /*
             * here directly access to update rmm carvedout_buf CMA_LOW from fdt
             * but once the rmm to be loadable module or rtk_kdriver/rmm will be isolated from mach-rtk/common.c
             * it'd be better to have another way to update it or get rid of carevedout_buf for good.
             */
            carvedout_buf[layout_idx][CARVEDOUT_CMA_LOW][0] = (unsigned long)rtk_dt_cma1_addr;
            carvedout_buf[layout_idx][CARVEDOUT_CMA_LOW][1] = (unsigned long)rtk_dt_cma1_size;
        }

        size = carvedout_buf_query(CARVEDOUT_CMA_LOW, (void *)&addr);
        dvr_declare_contiguous(NULL, (unsigned int)size, (unsigned int)addr, 0, rtkcma_list[DEV_CMA1_DEFAULT].name);

	} else {
        addr = 0x02800000;
        size = lowmem_limit - 72*_MB_;
		dvr_declare_contiguous(NULL, size, addr, min(dma_limit, lowmem_limit), rtkcma_list[DEV_CMA4_CMA1_LIMIT].name);
		carvedout_buf[layout_idx][CARVEDOUT_CMA_LOW_LIMIT][1] = 0;
	}
    rtd_pr_mach_rtk_info("reserve %08lx - %08lx for low memory cma...\n", addr, addr + size);

	size = carvedout_buf_query(CARVEDOUT_CMA_LOW_LIMIT, (void *)&addr);
	if (size) {
		memblock_reserve(addr, size);
		rtd_pr_mach_rtk_info("reserve %08lx - %08lx for low-limit memory cma...\n", addr, addr + size);
	}

#ifdef CONFIG_RTK_KDRV_PCIE
	size = carvedout_buf_query(CARVEDOUT_PCIE_BUS, (void *)&addr);
	if (size)
	{
		if(size > DISP_8K_PCIE_DMA_SIZE){
			rtd_pr_mach_rtk_info("reserve range(0x%x) big than HW limit(0x%x). (0x%08lx\n", size, DISP_8K_PCIE_DMA_SIZE);
			BUG_ON(1);
		}
		if (size) {
			memblock_reserve(addr, size);
			memblock_remove(addr + size, DISP_8K_PCIE_DMA_SIZE - size);
			rtd_pr_mach_rtk_info("reserve %08lx - %08lx for PCIE memory cma...\n", addr, addr + size - 1);
			rtd_pr_mach_rtk_info("remove  %08lx - %08lx for PCIE dma coherent...\n", addr + size, addr + DISP_8K_PCIE_DMA_SIZE - 1);
		}
	}
	else
	{
		rtd_pr_mach_rtk_info("pcie disabled, skip pcie memeory reservation\n");
	}
#endif
}

void __init __weak rtk_reserve(void)
{
	unsigned int ret = 0;
	unsigned int layout_idx = carvedout_buf_get_layout_idx();
    unsigned long size = 0, addr = 0;

	rtd_pr_mach_rtk_err("[MEM]rtk_reserve, DRAM_size = 0x%x, layout_idx = 0x%x\n", (unsigned int)DRAM_size, layout_idx);

#ifndef CONFIG_ARM64
	/* M6PRTANOM-100, Reserved to avoid STR resume secondary_core text section be allocated.*/
	memblock_reserve(__pa(&_text), PAGE_SIZE);
#endif

	/* for bootcode data */
	memblock_reserve(carvedout_buf[layout_idx][CARVEDOUT_BOOTCODE][0], carvedout_buf[layout_idx][CARVEDOUT_BOOTCODE][1]);
#if defined(CONFIG_REALTEK_RPC) ||IS_ENABLED(CONFIG_RTK_KDRV_RPC)
	/* for RPC temporarily */
	size = carvedout_buf_query(CARVEDOUT_MAP_RPC, (void *)&addr);
	if (size > 0)
		memblock_reserve(addr, size);
#endif
	/* for demod fw */
	ret |= add_ban_info(carvedout_buf[layout_idx][CARVEDOUT_DEMOD][0], carvedout_buf[layout_idx][CARVEDOUT_DEMOD][1], BAN_NOT_USED);

	/* for rbus & nor flash*/
	memblock_remove(carvedout_buf[layout_idx][CARVEDOUT_MAP_RBUS][0], carvedout_buf[layout_idx][CARVEDOUT_MAP_RBUS][1]);

#ifdef CONFIG_REALTEK_SECURE_DDK
	/* for GPU FW */
	memblock_reserve(carvedout_buf[layout_idx][CARVEDOUT_GPU_FW][0], carvedout_buf[layout_idx][CARVEDOUT_GPU_FW][1]);
#endif//

	/* for A & V fw */
	memblock_reserve(carvedout_buf[layout_idx][CARVEDOUT_V_OS][0], carvedout_buf[layout_idx][CARVEDOUT_V_OS][1]);
	memblock_reserve(carvedout_buf[layout_idx][CARVEDOUT_A_OS][0], carvedout_buf[layout_idx][CARVEDOUT_A_OS][1]);

#if IS_ENABLED(CONFIG_REALTEK_LOGBUF)
	carvedout_buf[layout_idx][CARVEDOUT_LOGBUF][1] = rtdlog_get_buffer_size();
	size = carvedout_buf_query(CARVEDOUT_LOGBUF, (void *)&addr);
	if (size > 0)
		memblock_reserve(addr, size);
#endif

	/* for rom code */
	memblock_reserve(carvedout_buf[layout_idx][CARVEDOUT_ROMCODE][0], carvedout_buf[layout_idx][CARVEDOUT_ROMCODE][1]);

#ifdef CONFIG_PSTORE
	/* for ramoops */
	memblock_reserve(carvedout_buf[layout_idx][CARVEDOUT_RAMOOPS][0], carvedout_buf[layout_idx][CARVEDOUT_RAMOOPS][1]);
#endif

	/* for ib boundary */
	if (carvedout_buf[layout_idx][CARVEDOUT_IR_TABLE][1])
		memblock_reserve(carvedout_buf[layout_idx][CARVEDOUT_IR_TABLE][0], carvedout_buf[layout_idx][CARVEDOUT_IR_TABLE][1]);

	/* for ddr boundary */
	size = carvedout_buf_query(CARVEDOUT_DDR_BOUNDARY, (void *)&addr);
	if (size > 0) {
		if (is_ddr_swap()) {
			// Reserve a page at the end of lowmem
			memblock_remove(addr, size);
		} else {
			// Reserve 4MB in CMA
			ret |= add_ban_info(addr, size, BAN_NOT_USED);
		}
	}

	/* for ddr boundary 2 */
	if ((DRAM_size == 0x80000) && (carvedout_buf[layout_idx][CARVEDOUT_DDR_BOUNDARY_2][1]))
		ret |= add_ban_info(carvedout_buf[layout_idx][CARVEDOUT_DDR_BOUNDARY_2][0], carvedout_buf[layout_idx][CARVEDOUT_DDR_BOUNDARY_2][1], BAN_NOT_USED);

	/* for audio & video DMEM */
	if (carvedout_buf[layout_idx][CARVEDOUT_AV_DMEM][1])
		ret |= add_ban_info(carvedout_buf[layout_idx][CARVEDOUT_AV_DMEM][0], carvedout_buf[layout_idx][CARVEDOUT_AV_DMEM][1], BAN_NORMAL);

	if (carvedout_buf[layout_idx][CARVEDOUT_VDEC_COMEM][1])
		memblock_reserve(carvedout_buf[layout_idx][CARVEDOUT_VDEC_COMEM][0], carvedout_buf[layout_idx][CARVEDOUT_VDEC_COMEM][0]);

	/* need content protection */
	memblock_remove(carvedout_buf[layout_idx][CARVEDOUT_VDEC_RINGBUF][0], carvedout_buf[layout_idx][CARVEDOUT_VDEC_RINGBUF][1]);

#ifdef CONFIG_REALTEK_SECURE
	/* for secure boot */
	memblock_reserve(carvedout_buf[layout_idx][CARVEDOUT_K_BOOT][0], carvedout_buf[layout_idx][CARVEDOUT_K_BOOT][1]);
	/* for secure OS */
	size = carvedout_buf_query(CARVEDOUT_K_OS, (void *)&addr);
	if (size > 0)
		memblock_remove(addr, size);
#endif

#if 0
    ret |= add_ban_info(carvedout_buf[layout_idx][CARVEDOUT_KASAN][0], carvedout_buf[layout_idx][CARVEDOUT_KASAN][1], BAN_NOT_USED);
#endif


#ifdef CONFIG_CUSTOMER_TV006
	ret |= add_ban_info(carvedout_buf[layout_idx][CARVEDOUT_GAL][0], carvedout_buf[layout_idx][CARVEDOUT_GAL][1], BAN_NOT_USED);
#endif

#ifdef CONFIG_LG_SNAPSHOT_BOOT
	if (snapshot_enable) {
		if (!add_ban_info(carvedout_buf[layout_idx][CARVEDOUT_SNAPSHOT][0], carvedout_buf[layout_idx][CARVEDOUT_SNAPSHOT][1], BAN_NOT_USED))
			reserve_boot_memory = 1;
	}
#endif

#ifdef BOOT_MEMORY_END
	BUG_ON(SCLAER_MODULE_START != BOOT_MEMORY_END);
#endif

	/* for scaler (memc/mdomain/vip/OD) */
	size = carvedout_buf_query(CARVEDOUT_SCALER, (void *)&addr);
	if (size)
		memblock_remove(addr, size);

    /* for vdec framebuffer */
	size = carvedout_buf_query(CARVEDOUT_VDEC_VBM, (void *)&addr);
	if (size)
		memblock_remove(addr, size);

	/* for demux */
	size = carvedout_buf_query(CARVEDOUT_TP, (void *)&addr);
	if (size)
		memblock_remove(addr, size);

	if (DRAM_size * PAGE_SIZE > RTK_SPI_ADDR) {
		rtd_pr_mach_rtk_info("remove rtk_spi start(%lx)..(%lx), size(%ld_MB))\n", RTK_SPI_ADDR, RTK_SPI_ADDR + RTK_SPI_SIZE, (RTK_SPI_SIZE/1024/1024));
		memblock_remove(RTK_SPI_ADDR, RTK_SPI_SIZE);
	}

	if (ret)
		panic("add_ban_info fail\n");

#if 1 //debug
    {
        unsigned long size, addr;
        size = carvedout_buf_query(CARVEDOUT_VDEC_COMEM, (void *)&addr);
        rtd_pr_mach_rtk_info("carvedout vdec-comem start(%lx)..(%lx), size(%ld_KB))\n", addr, addr + size, (size/1024));
        size = carvedout_buf_query(CARVEDOUT_SCALER_MEMC, (void *)&addr);
        rtd_pr_mach_rtk_info("carvedout memc start(%lx)..(%lx), size(%ld_MB))\n", addr, addr + size, (size/1024/1024));
        size = carvedout_buf_query(CARVEDOUT_SCALER_MDOMAIN, (void *)&addr);
        rtd_pr_mach_rtk_info("carvedout mdomain start(%lx)..(%lx), size(%ld_MB))\n", addr, addr + size, (size/1024/1024));
        size = carvedout_buf_query(CARVEDOUT_SCALER_DI_NR, (void *)&addr);
        rtd_pr_mach_rtk_info("carvedout di_nr start(%lx)..(%lx), size(%ld_MB))\n", addr, addr + size, (size/1024/1024));
        size = carvedout_buf_query(CARVEDOUT_SCALER_NN, (void *)&addr);
        rtd_pr_mach_rtk_info("carvedout nn start(%lx)..(%lx), size(%ld_MB))\n", addr, addr + size, (size/1024/1024));
        size = carvedout_buf_query(CARVEDOUT_SCALER_VIP, (void *)&addr);
        rtd_pr_mach_rtk_info("carvedout vip start(%lx)..(%lx), size(%ld_KB))\n", addr, addr + size, (size/1024));
        size = carvedout_buf_query(CARVEDOUT_SCALER_OD, (void *)&addr);
        rtd_pr_mach_rtk_info("carvedout od start(%lx)..(%lx), size(%ld_KB))\n", addr, addr + size, (size/1024));
        size = carvedout_buf_query(CARVEDOUT_VDEC_VBM, (void *)&addr);
        rtd_pr_mach_rtk_info("carvedout vbm start(%lx)..(%lx), size(%ld_KB))\n", addr, addr + size, (size/1024));
        size = carvedout_buf_query(CARVEDOUT_TP, (void *)&addr);
        rtd_pr_mach_rtk_info("carvedout tp start(%lx)..(%lx), size(%ld_MB))\n", addr, addr + size, (size/1024/1024));
    }
#endif
}
#endif

#ifndef CONFIG_REALTEK_MEMORY_MANAGEMENT_MODULE
void rtk_flush_range(const void *virt_start, const void *virt_end){
	dmac_flush_range(virt_start, virt_end);
//   outer_flush_range(phys_addr, phys_addr+size);
}

void rtk_inv_range(const void *virt_start, const void *virt_end){
	dmac_inv_range(virt_start, virt_end);
//	outer_inv_range(phys_addr, phys_addr+size);
}

EXPORT_SYMBOL(rtk_flush_range);
EXPORT_SYMBOL(rtk_inv_range);
#endif // CONFIG_REALTEK_MEMORY_MANAGEMENT_MODULE

#if IS_ENABLED(CONFIG_RTK_KDRV_COMMON)
/* compare the input string from DTS bootargs
	return true when match, otherwise return false.
	If return is true and match size < string_size, then output parameter will put string after equal sign(=).
	For exampel, if bootargs has XXX=YY, return true and output string YY if input string is XXX.
*/
bool rtk_parse_commandline_equal(const char *string, char *output_string, int string_size)
{
	const char *bootargs;
	char *cmdline, *token, *buf, *value;
	bool ret = false;
	int bootargs_size;

	struct device_node *np = of_find_node_by_name(NULL, "chosen");
	if (!np)
	{
		rtd_pr_mach_rtk_err("Error : rtk_parse_commandline can't get bootargs from DT(%d)\n",__LINE__);
		return false;
	}
	if (of_property_read_string(np, "bootargs", &bootargs))
	{
		rtd_pr_mach_rtk_err("Error : rtk_parse_commandline can't get bootargs from DT(%d)\n",__LINE__);
		return false;
	}

	bootargs_size = strlen(bootargs);
	buf = kmalloc(bootargs_size + 1, GFP_ATOMIC);
	if (!buf)
	{
		rtd_pr_mach_rtk_err("Error : rtk_parse_commandline can't kmalloc(%d)\n",__LINE__);
		return false;
	}
	strncpy(buf, bootargs, bootargs_size);
	buf[bootargs_size] = '\0';
	cmdline = buf;

	while (string != NULL)
	{
		token = strsep(&cmdline, " ");
		if(token != NULL)
		{
			//rtd_pr_mach_rtk_debug("counting %s\n", token);
			value = strsep(&token, "=");
			if(strcmp(value, string) == 0)
			{
				/* M7PCETOP-2008, not have equal string but use rtk_parse_commandline_equal */
				if(token == NULL)
				{
					rtd_pr_mach_rtk_err("Error : rtk_parse_commandline(%s) don't have =, please use rtk_parse_commandline\n", string);
					return false;
				}
				if((output_string) && (strlen(token) < string_size))
				{
					strcpy(output_string, token);
					ret = true;
					//rtd_pr_mach_rtk_debug("counting true\n");
				}
				else
					rtd_pr_mach_rtk_err("Error : rtk_parse_commandline(%s) input(%d) < token(%d)\n", string, string_size, strlen(token));
				break;
			}
		}
		else
			break;
	}

	kfree(buf);
	return ret;
}
EXPORT_SYMBOL(rtk_parse_commandline_equal);
/* compare the input string from DTS bootargs
	return true when match, otherwise return false.
*/
bool rtk_parse_commandline(const char *string)
{
	const char *bootargs;
	char *cmdline, *token, *buf;
	bool ret = false;
	int bootargs_size;

	struct device_node *np = of_find_node_by_name(NULL, "chosen");
	if (!np)
	{
		rtd_pr_mach_rtk_err("Error : rtk_parse_commandline can't get bootargs from DT(%d)\n",__LINE__);
		return false;
	}
	if (of_property_read_string(np, "bootargs", &bootargs))
	{
		rtd_pr_mach_rtk_err("Error : rtk_parse_commandline can't get bootargs from DT(%d)\n",__LINE__);
		return false;
	}

	bootargs_size = strlen(bootargs);
	buf = kmalloc(bootargs_size + 1, GFP_ATOMIC);
	if (!buf)
	{
		rtd_pr_mach_rtk_err("Error : rtk_parse_commandline can't kmalloc(%d)\n",__LINE__);
		return false;
	}
	strncpy(buf, bootargs, bootargs_size);
	buf[bootargs_size] = '\0';
	cmdline = buf;

	while (string != NULL)
	{
		token = strsep(&cmdline, " ");
		if(token != NULL)
		{
			//rtd_pr_mach_rtk_debug("counting %s\n", token);
			if(strcmp(token, string) == 0)
			{
				ret = true;
				//rtd_pr_mach_rtk_debug("counting true\n");
				break;
			}
		}
		else
			break;
	}

	kfree(buf);
	return ret;
}
EXPORT_SYMBOL(rtk_parse_commandline);
#endif //CONFIG_RTK_KDRV_COMMON

/* Panic hook for android boot reason */
#define EMMC_BLOCK_SIZE 512
#define EMMC_ADDR_SIZE 10
#define HEADER_SIZE 32
static unsigned long rtk_panic_reason_addr = 0;
extern struct atomic_notifier_head panic_notifier_list;
static s32 (*emmc_write_func_ptr)(u8 *buffer, u64 address, u32 size) = NULL;

static unsigned int get_checksum(unsigned char *p, unsigned int len) {
	unsigned int checksum = 0;
	unsigned int i;

	for(i = 0; i < len; i++) {
		checksum += *(p+i);
	}
	return checksum;
}

/* emmc driver would register "export_raw_dw_em_read" */
void emmc_write_api_register(s32 (*func)(u8*, u64, u32))
{
	emmc_write_func_ptr = func;
}
EXPORT_SYMBOL(emmc_write_api_register);

/* kfault boot reason */
#define KFAULT_BOOT_REASON_KERNEL_PANIC       0x1a3b5c7d
void write_kfault_panic_reason(void)
{
	rtd_outl(STB_SW_DATA10_reg, KFAULT_BOOT_REASON_KERNEL_PANIC);
}

/* Kernel panic callback function */
static int boot_reason_panic(struct notifier_block *nb, unsigned long event, void *data)
{
	int err = 0;
	unsigned int checksum = 0;
	unsigned int magic_num = 0x11223344;
	u8* buf = kmalloc(EMMC_BLOCK_SIZE, __GFP_HIGH | __GFP_DMA | __GFP_ZERO);
	u8* reason = buf + HEADER_SIZE;

	if (!buf)
	{
		rtd_pr_mach_rtk_err("Error : %s can't kmalloc\n", __FUNCTION__);
		return NOTIFY_BAD;
	}

	strncpy(reason, data, EMMC_BLOCK_SIZE-HEADER_SIZE);
	buf[EMMC_BLOCK_SIZE - 1] = '\0';
	checksum = get_checksum(reason, strlen(reason));
	*(unsigned int *)(buf) = magic_num;
	*(unsigned int *)(buf+4) = checksum;

	// print kernel panic reason
	rtd_pr_mach_rtk_emerg("Panic reason: %s\n", reason);

	//add for kfault reboot reason
	write_kfault_panic_reason();

	// write panic reason into emmc
	if (rtk_panic_reason_addr != 0) {
		if (emmc_write_func_ptr) {
			// set emmc_scpu_sel bit[0]: scpu_sel -> 0, make sure we can access emmc
			rtd_clearbits(EMMC_WRAP_emmc_scpu_sel_reg, EMMC_WRAP_emmc_scpu_sel_scpu_sel_mask);

			err = emmc_write_func_ptr((u8 *)buf, rtk_panic_reason_addr, EMMC_BLOCK_SIZE);

			if (err) {
				rtd_pr_mach_rtk_emerg("[%s] %d Failed to write panic reason into emmc\n", __func__, err);
			}
		}
		else {
			rtd_pr_mach_rtk_emerg("[%s] Failed to write panic reason into emmc: emmc write function pointer is NULL\n", __func__);
		}
	}
	else {
		rtd_pr_mach_rtk_emerg("[%s] Failed to write panic reason into emmc: emmc target address not found\n", __func__);
	}

	kfree(buf);

    return NOTIFY_OK;
}

/* Setup callback function as panic occurs */
static struct notifier_block boot_reason_panic_block = {
	.notifier_call = boot_reason_panic,
};

#ifndef MODULE
static int __init early_rtk_panic_reason_addr(char *str)
{	
	if (str) {
		rtk_panic_reason_addr = simple_strtoull(str, NULL, 16);
		rtd_pr_mach_rtk_emerg("panic_reason_addr = 0x%lx\n", rtk_panic_reason_addr);
    }
    return 0;
}
early_param("panic_reason_addr", early_rtk_panic_reason_addr);
#endif

/* For writing panic reason into emmc, get reserved partition start address from kernel cmd line params */
void prep_emmc_addr(void)
{
#ifdef MODULE
	char addr[EMMC_ADDR_SIZE] = {0};

	if (rtk_parse_commandline_equal("panic_reason_addr", addr, EMMC_ADDR_SIZE) == 1) {
		rtk_panic_reason_addr = simple_strtoull(addr, NULL, 16);
	}
	else {
		rtd_pr_mach_rtk_emerg("[%s] Failed to get emmc reserved address\n", __func__);
	}
#endif
}

/* Register notifier block to panic_notifier_list */
static int hook_boot_reason_panic(void)
{
	prep_emmc_addr();
	return atomic_notifier_chain_register(&panic_notifier_list, &boot_reason_panic_block);
}

extern int kill_watchdog (void);
#define WDOG_STATUS_MAGIC_NUM       0x12348765

static int rtk_machine_restart(struct notifier_block *nb, unsigned long action, void *data)
{
	volatile unsigned int iRegVal;
#ifdef CONFIG_RTK_KDRV_WATCHDOG
	kill_watchdog ();
#endif
	rtd_pr_mach_rtk_emerg("reboot action=%lu\n", action);

	rtd_outl(0xb8060110, 0);

	//rtd_pr_mach_rtk_emerg("wait...(0/2)\n");
	//mdelay(1000);
	//rtd_pr_mach_rtk_emerg("wait...(1/2)\n");
	//mdelay(1000);
	rtd_pr_mach_rtk_emerg("wait finish, reboot...(2/2)\n");

#ifdef CONFIG_CUSTOMER_TV043
        rtd_outl(STB_WDOG_DATA9_reg,WDOG_STATUS_MAGIC_NUM);
#endif

	// disable all interrupt, ?
	// retart system
	// enable the dog
	rtd_clearbits(WDOG_TCWCR_reg, 0xff);

	iRegVal = rtd_inl(WDOG_TCWCR_reg);
	iRegVal |= (1<<WDOG_TCWCR_im_wdog_rst_shift); // bit 31 in rtd294x
	rtd_outl(WDOG_TCWCR_reg, iRegVal);

	// KWarning: checked ok by eaton.chiou@realtek.com
	while(1) {
		rtd_pr_mach_rtk_err("while 1\n");
		;
	}
}


static struct notifier_block rtk_nb = {
    .notifier_call = rtk_machine_restart,
};


static int hook_rtk_machine_restart(void)
{
    return register_restart_handler(&rtk_nb);
}

/* Android U changes metadata filesystem from ext4 to f2fs
* which results in bootcode can not read boot reason from persist.sys.boot.reason anymore
* Bootcode is responsible for setting backlight off as reboot quiescent
* Therefore, we use dummy register to notify bootcode reboot quiescent occurs
*/
#define CTS_QUIESCENT_MAGIC 	0x901efef7   //for CTS authentication
static int rtk_notify_quiescent(struct notifier_block *nb, unsigned long action, void *data)
{
	if (data != NULL) {
		// pr_info("[%s] %s\n", __FUNCTION__, (char *)data);
		if(strncmp((char *)data, "userrequested,quiescent", strlen("userrequested,quiescent")) == 0) {
			rtd_outl(UPGRADE_STATUS_reg, CTS_QUIESCENT_MAGIC);
		}
		else if (strstr((char *)data, "quiescent") != NULL)
			rtd_outl(UPGRADE_STATUS_reg, UPGRADE_QUIESCENT_MAGIC);
	}

	return NOTIFY_OK;
}

static struct notifier_block rtk_quiescent_nb = {
	.notifier_call = rtk_notify_quiescent,
	// just higher than psci_sys_reset_nb's priority in drivers/firmware/psci/psci.c
	// make sure rtk_notify_quiescent could be executed before psci triggers reboot
	.priority = 130,
};

static int hook_rtk_notify_quiescent(void)
{
	return register_restart_handler(&rtk_quiescent_nb);
}

#ifdef CONFIG_RTK_KDRV_COMMON_MODULE
void __iomem * rbus_base;
EXPORT_SYMBOL(rbus_base);

int setup_rbus_mapping(void)
{
	struct device_node *np;

	np = of_find_compatible_node(NULL, NULL, "realtek,common");
	if (!np)
		return -1;

	rbus_base = of_iomap(np, 0);
	if (rbus_base == NULL) {
		rbus_base = (void *)ULONG_MAX;
		rtd_pr_mach_rtk_err("common: err: rbus_base fail = %lx\n", (unsigned long)rbus_base);
		return -1;
	}
	rtd_pr_mach_rtk_info("common: rbus_base = %lx\n", (unsigned long)rbus_base);

	return 0;
}
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
#ifdef CONFIG_RTK_LOW_POWER_MODE
LIST_HEAD(rtk_lpm_dev_list);
void rtk_lpm_add_device(struct device *dev)
{
        struct rtk_lpm_dev_info *info;
        info = kzalloc(sizeof(*info), GFP_KERNEL);
        info->dev = dev;
        list_add(&info->list_head, &rtk_lpm_dev_list);
        pr_err("[LPM] add device(%s: %x)\n", dev_name(dev), dev);
}
EXPORT_SYMBOL_GPL(rtk_lpm_add_device);
EXPORT_SYMBOL_GPL(rtk_lpm_dev_list);

static BLOCKING_NOTIFIER_HEAD(lpm_chain_head);
int register_lpm_notifier(struct notifier_block *nb)
{
	pr_info("[LPM] reg(%pf)\n", __builtin_return_address(0));
	return blocking_notifier_chain_register(&lpm_chain_head, nb);
}
EXPORT_SYMBOL_GPL(register_lpm_notifier);

int unregister_lpm_notifier(struct notifier_block *nb)
{
	pr_info("[LPM] unreg(%pf)\n", __builtin_return_address(0));
	return blocking_notifier_chain_unregister(&lpm_chain_head, nb);
}
EXPORT_SYMBOL_GPL(unregister_lpm_notifier);

int lpm_notifier_call_chain(unsigned long val)
{
	int ret = blocking_notifier_call_chain(&lpm_chain_head, val, NULL);
	return notifier_to_errno(ret);
}
EXPORT_SYMBOL_GPL(lpm_notifier_call_chain);
#endif
#endif //LINUX_VERSION_CODE
void rtd_fw_git_version_show(void)
{
	rtd_pr_mach_rtk_err("%s %s %s\n",RTK_LINUX_GIT_VERSION,RTK_KDRIVER_GIT_VERSION,RTK_COMMON_GIT_VERSION);
	return;
}
int __init dvr_common_module_init(void)
{
	rtd_fw_git_version_show();
#ifdef CONFIG_RTK_KDRV_COMMON_MODULE
	if (setup_rbus_mapping()) {
		panic("common: setup rbus mapping fail\n");
	}
#endif

#ifdef CONFIG_RTK_KDRV_COMMON_MODULE
	parse_envp();
	rtk_init_early();
#endif
#ifdef CONFIG_RTK_KDRV_COMMON
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
	//if linux 5.15 built-in also call here
	rtk_init_early();
#endif
#endif

	hook_rtk_notify_quiescent();
	hook_boot_reason_panic();
	hook_rtk_machine_restart();
	interrupt_vendor_hook_init();
	rtk_timer_init();

#if 0//IS_ENABLED(CONFIG_RTK_SEMAPHORE)
	rtd_semaphore_init();
#endif

	rtk_init_machine();

#ifdef CONFIG_RTK_KDRV_COMMON_MODULE
	rtk_init_late();
#endif

#ifdef CONFIG_RTK_KDRV_COMMON
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
	//if linux 5.15 built-in also call here
	rtk_init_late();
#endif
#endif

#ifdef CONFIG_RTK_KDRV_COMMON_MODULE
	/*parse bootargs*/
	parse_platform_model();
	parse_platform();
	parse_product();
	parse_reclaim();
	parse_last_image();
	parse_qs_logo();
#ifdef CONFIG_RTK_KDRV_KILLER
	parse_no_kill_list();
	parse_disable_killer();
#endif /* CONFIG_RTK_KDRV_KILLER */
#endif
	parse_debugMode();

	/*ML7BU-1580: send smc to ATF for setting audio core routing affinity.*/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
#if IS_ENABLED(CONFIG_ARCH_RTK6748) || IS_ENABLED(CONFIG_ARCH_RTK2851C) || IS_ENABLED(CONFIG_ARCH_RTK2851F)
	{
		struct arm_smccc_res res;
		rtd_pr_mach_rtk_warn("RTK_KDRV_COMMON: send smc OEM_SVC_GIC_DIST_INIT.\n");
		arm_smccc_smc(OEM_SVC_GIC_DIST_INIT, 0, 0, 0, 0, 0, 0, 0, &res);
		if(res.a0 != 0)
		{
			rtd_pr_mach_rtk_err("RTK_KDRV_COMMON: send smc OEM_SVC_GIC_DIST_INIT fail(%lu)\n", res.a0);
		}
	}
#endif
#endif

	rtd_pr_mach_rtk_warn("RTK_KDRV_COMMON init finish\n");
	return 0;
}

arch_initcall(dvr_common_module_init);
MODULE_LICENSE("GPL");


