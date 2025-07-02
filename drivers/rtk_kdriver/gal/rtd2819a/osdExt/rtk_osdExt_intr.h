

#ifndef __RTK_OSDEXT_INTR_H__
#define __RTK_OSDEXT_INTR_H__


typedef irqreturn_t (*GDMA_IRQ_handler_funcp)(int irq, void *dev_id);


irqreturn_t GDMA_OSD4_irq_handler(int irq, void *dev_id);

irqreturn_t GDMA_OSD5_irq_handler(int irq, void *dev_id);



#endif// __RTK_OSDEXT_INTR_H__
