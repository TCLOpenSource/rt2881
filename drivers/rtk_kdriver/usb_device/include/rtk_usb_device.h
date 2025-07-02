#ifndef __RTK_USB_DEVICE_H__
#define __RTK_USB_DEVICE_H__

#include <linux/mutex.h>

typedef struct {
        UINT8 hardware_reseted;

        /* saved allocated memory pointer */
        void *info_pack_buf_cached;
        UINT8 *section_data_buf_of_pid_filter;

        void *start_code_pack_buf_cached;


        /* tp input rate detect related */
        struct task_struct * tpi_rate_cnt_t;
        UINT8 tpi_rate_cnt_t_start;

        /* ring buffer status */
        struct task_struct *thread_dumpdata;
        UINT8 dumpdata_start;

        struct task_struct *sec_assemble;
        UINT8 sec_asm_start;

        struct task_struct *tp_dbg_t;
        UINT8 tp_dbg_t_start;
        /* tp out status */


        int hw_reset;
        UINT8 tp_irq_enable;
        int irq_number;

        UINT32 tpo_pre_div;        
        UINT32 tpo_post_div; 
			
} RTK_USB_DEVICE_DRV_T;

extern RTK_USB_DEVICE_DRV_T* pUsb_device_drv;

#endif      /* __RTK_USB_DEVICE_H__ */
