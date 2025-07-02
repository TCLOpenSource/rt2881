#include <linux/version.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <rtd_log/rtd_module_log.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/cma.h>
#include <linux/pageremap.h>
#include <linux/rtkblueprint.h>
#include <linux/mm.h>
#include <linux/pageremap.h>
#include <linux/page_owner.h>
#include <linux/sched/signal.h>

#include "cma.h"



#ifdef CONFIG_ARM64
/****************************************set pte attribute******************************/
struct page_change_data {
    pgprot_t set_mask;
    pgprot_t clear_mask;
};


static struct mm_struct *record_mm_from_init_task = NULL;
static void   *kernel_pgd_addr = NULL;

static inline uint64_t read_ttbr1(void)
{ 
	uint64_t val64 = 0;

	asm volatile("mrs %0, ttbr1_el1" : "=r" (val64));
	//use phy value
	val64  =  val64 & 0xFFFFFFFFFFFE;
	return val64;
}

int rtk_set_attribute_init(void)
{
#if  (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 13, 0))
	/*for rtk_set_memory_valid record init_mm.pgd va*/
	record_mm_from_init_task = init_task.active_mm; 
 #endif 
	//rtd_pr_rmm_err("KIMAGE_VADDR=%llx v1=%llx v2=%llx\n", KIMAGE_VADDR, phys_to_virt(read_ttbr1()), read_ttbr1());
	kernel_pgd_addr = (void *)((read_ttbr1() + KIMAGE_VADDR));
	rtd_pr_rmm_err("rmm_svp: swapper_pgd_addr = %llx\n", kernel_pgd_addr);
	//rtd_pr_rmm_err("rmm_svp: init_mm.pgd = %llx\n", init_mm.pgd);
	return 0;
}

static int change_page_range(pte_t *ptep, unsigned long addr, void *data)
{
	struct page_change_data *cdata = data;
	pte_t pte = READ_ONCE(*ptep);

	pte = clear_pte_bit(pte, cdata->clear_mask);
	pte = set_pte_bit(pte, cdata->set_mask);

	set_pte(ptep, pte);
	return 0;
}
#if  (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 13, 0))
static int rtk_apply_to_page_range(struct mm_struct *mm, unsigned long saddr,
				unsigned long size, pte_fn_t fn,
				void *data)
{
	pgd_t *pgdp;
	pgd_t *pgd_h;
	pgd_t pgd;
	unsigned long addr = saddr;

	pgd_h  = (pgd_t *)kernel_pgd_addr;


	do{
		if(addr == saddr + size)
				break;
		pgdp = pgd_offset_pgd(pgd_h, addr);
		pgd = READ_ONCE(*pgdp);
		do {
			p4d_t *p4dp, p4d;
			pud_t *pudp, pud;
			pmd_t *pmdp, pmd;
			pte_t *ptep, pte;

			if (pgd_none(pgd) || pgd_bad(pgd))
					break;

			p4dp = p4d_offset(pgdp, addr);
			p4d = READ_ONCE(*p4dp);
			if (p4d_none(p4d) || p4d_bad(p4d))
					break;

			pudp = pud_offset(p4dp, addr);
			pud = READ_ONCE(*pudp);
			if (pud_none(pud) || pud_bad(pud))
					break;

			pmdp = pmd_offset(pudp, addr);
			pmd = READ_ONCE(*pmdp);
			if (pmd_none(pmd) || pmd_bad(pmd))
					break;

			ptep = pte_offset_map(pmdp, addr);
			pte = READ_ONCE(*ptep);
			change_page_range(ptep, addr, data);
			pte_unmap(ptep);
		} while(0);

		addr += PAGE_SIZE;
	}while(1);

	return 0;
}

#else
static int rtk_apply_to_page_range(struct mm_struct *mm, unsigned long saddr,
                                unsigned long size, pte_fn_t fn,
                                void *data){
	return 0;
}

#endif

static int __change_memory_common(unsigned long start, unsigned long size,
                pgprot_t set_mask, pgprot_t clear_mask)
{
    struct page_change_data data;
    int ret;

    struct mm_struct *mm =NULL;

	mm = record_mm_from_init_task ;
	data.set_mask = set_mask;
	data.clear_mask = clear_mask;

#if 0
#ifndef MODULE
	if(mm)
			ret = apply_to_page_range(mm, start, size, change_page_range,
							&data);
	else
#endif
#endif
			ret = rtk_apply_to_page_range(mm, start, size, change_page_range,
							&data);

    flush_tlb_kernel_range(start, start + size);
    return ret;
}

int rtk_set_memory_valid(unsigned long addr, unsigned long size, int enable)
{
    if (enable)
        return __change_memory_common(addr, size,
                    __pgprot(PTE_VALID),
                    __pgprot(0));
    else
        return __change_memory_common(addr, size,
                    __pgprot(0),
                    __pgprot(PTE_VALID));
}

EXPORT_SYMBOL(rtk_set_memory_valid);

#if  (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 13, 0))

static int scan_pte(struct mm_struct *smm, unsigned long addr, unsigned long phy_start, unsigned long phy_end)
{
        struct mm_struct *mm;
        pgd_t *pgdp;
        pgd_t pgd;
	    pgd_t *pgd_h;

        unsigned long phy_addr = 0;

        if(!smm){
	        pgd_h  = (pgd_t *)kernel_pgd_addr;
            pgdp = pgd_offset_pgd(pgd_h, addr);
            pgd = READ_ONCE(*pgdp);
        }
        else {
                mm = smm;
                pgdp = pgd_offset(mm, addr);
                pgd = READ_ONCE(*pgdp);
        }

        do {
                p4d_t *p4dp, p4d;
                pud_t *pudp, pud;
                pmd_t *pmdp, pmd;
                pte_t *ptep, pte;

                if (pgd_none(pgd) || pgd_bad(pgd))
                        break;

                p4dp = p4d_offset(pgdp, addr);
                p4d = READ_ONCE(*p4dp);
                if (p4d_none(p4d) || p4d_bad(p4d))
                        break;

                pudp = pud_offset(p4dp, addr);
                pud = READ_ONCE(*pudp);
                if (pud_none(pud) || pud_bad(pud))
                        break;

                pmdp = pmd_offset(pudp, addr);
                pmd = READ_ONCE(*pmdp);
                if (pmd_none(pmd) || pmd_bad(pmd))
                        break;

                ptep = pte_offset_map(pmdp, addr);
                pte = READ_ONCE(*ptep);
                phy_addr = __pte_to_phys(pte);
                pte_unmap(ptep);
        } while(0);

        phy_addr  = phy_addr|(addr & 0xFFF);

        if(phy_addr >= phy_start  &&  phy_addr<=phy_end)
        {
                //rtd_pr_rmm_err("VA=%lx,PA=%lx\n",addr,  phy_addr);
                return phy_addr;
        }
        else
                return 0;   
    
}


#else

static int scan_pte(struct mm_struct *smm, unsigned long addr, unsigned long phy_start, unsigned long phy_end)
{
        struct mm_struct *mm  = NULL;
        pgd_t *pgdp;
        pgd_t pgd;
        unsigned long phy_addr = 0;
	    pgd_t *pgd_h;

        if(!smm){
	        pgd_h  = (pgd_t *)kernel_pgd_addr;
            pgdp = pgd_offset_raw(pgd_h, addr);
            pgd = READ_ONCE(*pgdp);
        }
        else {
                mm = smm;
                pgdp = pgd_offset(mm, addr);
                pgd = READ_ONCE(*pgdp);
        }


        do {
                pud_t *pudp, pud;
                pmd_t *pmdp, pmd;
                pte_t *ptep, pte;

                if (pgd_none(pgd) || pgd_bad(pgd))
                        break;

                pudp = pud_offset(pgdp, addr);
                pud = READ_ONCE(*pudp);
                if (pud_none(pud) || pud_bad(pud))
                        break;

                pmdp = pmd_offset(pudp, addr);
                pmd = READ_ONCE(*pmdp);
                if (pmd_none(pmd) || pmd_bad(pmd))
                        break;

                ptep = pte_offset_map(pmdp, addr);
                pte = READ_ONCE(*ptep);
                phy_addr = __pte_to_phys(pte);
                pte_unmap(ptep);
        } while(0);

        phy_addr  = phy_addr|(addr & 0xFFF);

        if(phy_addr >= phy_start  &&  phy_addr<=phy_end)
        {
                //rtd_pr_rmm_err("VA=%lx,PA=%lx\n",addr,  phy_addr);
                return phy_addr;
        }
        else
                return 0;   
}

#endif

typedef struct
{
        unsigned long start;
        unsigned long end ;
        char *name;
}scan_range_t;

int foreach_process_scan_phyaddr(unsigned long phy_start, unsigned long phy_end)
 {
         unsigned long long vva = 0;
         unsigned long va = 0;
         unsigned long pa;
         unsigned long start, end;
         int i = 0, cnt= 0;
         struct task_struct *p;
         struct mm_struct *mm;
         struct vm_area_struct *vma;     

         scan_range_t rangs[] = {
                 {PAGE_OFFSET, PAGE_END, "line mapping"},
                 {MODULES_VADDR, MODULES_END, "Module"},
                 {VMALLOC_START, VMALLOC_END, "vmalloc"},
                 {FIXADDR_START, FIXADDR_TOP, "FXIADDR"},
                 {PCI_IO_START, PCI_IO_END, "PCI"}
         };

         for(i=0; i<5; i++)
         {
                 start = rangs[i].start;
                 end = rangs[i].end;
                 for(vva=start; vva<end; vva+=PAGE_SIZE)
                 {       
                         va = (unsigned long)vva;
                         if((pa = scan_pte(NULL, va, phy_start, phy_end)))
                         {       
                                 rtd_pr_rmm_err("Kernel va=%lx pa =%lx phy_addr<%lx-%lx>\n", va, pa, phy_start, phy_end);
                         }

                 }
                rtd_pr_rmm_err("------------------scan kernel [%s] 0x%lx-0x%lx-------------------\n", rangs[i].name, start, end);
         }


         rtd_pr_rmm_err("------------------scan kernel DONE!-------------------\n");

    for_each_process(p)
    {
        
        cnt ++;
        mm = p->mm;
        if(!mm){
            continue;
            //mm = &init_mm;          
        } 

        for (vma = mm->mmap ; vma ; vma = vma->vm_next) {
            for(va=vma->vm_start; va<vma->vm_end; va+=PAGE_SIZE){
                if((pa = scan_pte(mm, va, phy_start, phy_end))){
                    rtd_pr_rmm_err("PID=%d task=%s va=%lx pa=%lx  phy_addr<%lx-%lx>\n", p->pid, p->comm, va, pa, phy_start, phy_end);
                }   

            }
        }
    }
    rtd_pr_rmm_err("------------------scan user done  task_num=%d-------------------\n", cnt);
    return 0;
}
EXPORT_SYMBOL(foreach_process_scan_phyaddr);

#else



int rtk_set_attribute_init(void)
{
	return 0;
}

int foreach_process_scan_phyaddr(unsigned long phy_start, unsigned long phy_end)
{
        rtd_pr_rmm_err("------------------not support for ARM32!-------------------\n");     
        return 0;
}
EXPORT_SYMBOL(foreach_process_scan_phyaddr);


int rtk_set_memory_valid(unsigned long vaddr, unsigned long size, int enable)
{
    void *ptr  = (void *)vaddr; 
    
    if(!enable){
        vunmap_range(vaddr, vaddr+size);
    }else {
        ioremap_page_range(vaddr, vaddr+size, virt_to_phys(ptr), PAGE_KERNEL);
    }
 
    return 0;
}
EXPORT_SYMBOL(rtk_set_memory_valid);
#endif



MODULE_AUTHOR("Realtek.com");
MODULE_LICENSE("Dual BSD/GPL");
