#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <asm/memory.h>
#include "rtk_kdriver/mcp/rtk_mcp_aux.h"
#include "mcp/rtk_mcp_dbg.h"



#ifdef MODULE 
static struct vm_area_struct * find_vma_ex (struct mm_struct * mm, unsigned long addr)
{
    struct vm_area_struct* vm_area = NULL;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0))
       down_read(& mm->mmap_lock);
#else
    down_read(& mm->mmap_sem);
#endif
    vm_area = find_vma(mm, addr);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0))
       up_read(& mm->mmap_lock);
#else 
    up_read(& mm->mmap_sem);
#endif
    return vm_area;
}
#else 
#define  find_vma_ex  find_vma
#endif

int mcp_is_pli_address(unsigned int addr)
{
    return pli_to_kernel(addr) ? 1 : 0;
}


unsigned long pli_to_kernel(unsigned int addr)
{
    struct vm_area_struct* vm_area = find_vma_ex(current->mm, (unsigned long) addr);
    unsigned long pfn;
    unsigned long virt_addr;

    if (vm_area == NULL || follow_pfn(vm_area, (unsigned long) addr, &pfn))
        return 0;

    virt_addr = __phys_to_virt(__pfn_to_phys(pfn)) + (((unsigned long)addr) % PAGE_SIZE);

    mcp_debug("addr=%08lx, pfn=%08lx, kernel addr=%08lx\n", (unsigned long)addr, pfn, virt_addr);
    return virt_addr;
}
