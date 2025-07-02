
#include <linux/of_irq.h>

#include "rtk_osdExt_main.h"


#include "rtk_osdExt_driver.h"
#include "rtk_osdExt_linux.h"
#include "rtk_osdExt_debug.h"


int gdmaExt_major = 0;
int gdmaExt_minor = 0;
int gdmaExt_nr_devs = 1;

extern gdma_dev *gdma_devices;


extern irqreturn_t GDMAExt_irq_handler(int irq, void *dev_id);

extern GDMAExt_device_cb g_osd_cb[GDMA_PLANE_ALL_MAXNUM];



#define DRV_NAME	"gdmaExt"

#ifdef CONFIG_PM

static int gdmaExt_suspend (struct device *p_dev);
static int gdmaExt_resume (struct device *p_dev);


//static struct platform_device *gdmaExt_devs; //gdma_devs

static const struct dev_pm_ops gdmaExt_pm_ops = {
	.suspend    = gdmaExt_suspend,
	.resume     = gdmaExt_resume,
  #ifdef CONFIG_HIBERNATION
	.freeze     = gdmaExt_suspend_std,
	.thaw       = gdmaExt_resume_std_thraw,
	.poweroff   = gdmaExt_suspend_std,
	.restore    = gdmaExt_resume_std,
  #endif
};
#endif//CONFIG_PM


#if 0
static const char  drv_name[] = DRV_NAME;

static const struct of_device_id rtk_gdmaExt_match[] = {
	{
		.compatible = "realtek,gdmaExt",
		.type = "osd",
	},
    {/* sentinel */ }
};
MODULE_DEVICE_TABLE(of, rtk_gdma_match);


static struct platform_driver gdmaExt_driver = {
	.probe			= GDMAExt_probe,
	.driver = {
		.name         = (char *)drv_name,
		.of_match_table = of_match_ptr(rtk_gdmaExt_match),
		.bus          = &platform_bus_type,
#ifdef CONFIG_PM
		.pm           = &gdmaExt_pm_ops,
#endif

	},
};
#endif//

#if 0
struct file_operations gdmaExt_fops
	= {
	.owner    =    THIS_MODULE,
	.llseek   =    GDMA_llseek,
	.read     =    GDMA_read,
	.write    =    GDMA_write,
	.unlocked_ioctl    =    GDMA_ioctl,
	.open     =    GDMA_open,
	.release  =    GDMA_release,
#ifdef CONFIG_COMPAT
	.compat_ioctl   = compat_GDMA_ioctl,
#endif
};
#endif//


#ifdef CONFIG_PM

static int gdmaExt_suspend(struct device *p_dev) 
{
	rtd_pr_gdma_crit("func=%s line=%d p_dev=%x\n",__FUNCTION__,__LINE__,p_dev);


	return 0;

}




static int gdmaExt_resume (struct device *p_dev) 
{

	rtd_pr_gdma_crit("func=%s line=%d p_dev=%x\n",__FUNCTION__,__LINE__,p_dev);


	return 0;
}

#endif//CONFIG_PM


#if 0
static void GDMAExt_setup_cdev(gdmaExt_dev *dev, int index)
{
    #if 0
	int err, devno = MKDEV(gdma_major, gdma_minor + index);

	cdev_init(&dev->cdev, &gdma_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops   = &gdma_fops;
	err = cdev_add (&dev->cdev, devno, 1);

	if (err)
		DBG_PRINT(KERN_EMERG "Error %d adding se%d", err, index);

	device_create(gdma_class, NULL, MKDEV(gdma_major, index), NULL, "gdma%d", index);
    #endif//0

}


static char *GDMAExt_devnode(struct device *dev, umode_t *mode)
{
	return NULL;
}
#endif//

void GDMAExt_cleanup_module(void)
{
    GDMA_EXT_LOG( GDMA_EXT_LOG_ALL, "gdmaEx clean module gdma_major = %d\n", gdmaExt_major);

    #if 0
	int i;
	dev_t devno = MKDEV(gdma_major, gdma_minor);


	GDMA_uninit_debug_proc();

	if (gdma_devices) {
		for (i = 0; i < gdma_nr_devs; i++) {
			cdev_del      (&gdma_devices[i].cdev);
			device_destroy(gdma_class, MKDEV(gdma_major, i));
		}
		kfree(gdma_devices);
	}

	class_destroy(gdma_class);
	/* cleanup_module is never called if registering failed */
	unregister_chrdev_region(devno, gdma_nr_devs);
    #endif//

}


/*

@param:
  idx:  should be at least 1 

 */
int GDMAExt_init_os_interrupt( struct platform_device* pdev, int idx, GDMA_IRQ_handler_funcp funcp  )
{
    int err = 0;
	int irq_no = 0;
	struct device *dev = &(pdev->dev); 
 	gdma_dev *gdma = &gdma_devices[0];
	const char* drv_name[] = {"osd4", "osd5"};


	if ( idx ==0 ) {
		GDMAEXT_ERROR("[%s] not support osd 1's intr(idx 0) \n", __func__);
		return -ENODEV;
	}

  #ifdef GDMA_CONFIG_ENABLE_INTERRUPT

		if ( irq_of_parse_and_map(pdev->dev.of_node, idx) )  {
			irq_no = irq_of_parse_and_map( dev->of_node, idx );

			GDMAEXT_DEBUG(" idx:%d get irq= %d\n", idx, irq_no );

			if ( ! irq_no ) {
				GDMAEXT_ERROR("[%s] No gdma irq found!\n", __func__);
				return -ENODEV;
			}

			if ( request_irq(irq_no, funcp, IRQF_SHARED, drv_name[idx -1], (void*)gdma)) {
				GDMAEXT_ERROR("[%s] request irq %d fail!\n", __func__, irq_no);
				return -ENODEV;
			}

		} else {
			/*if can't get irq , don't set below settings*/
			GDMAEXT_ERROR("[%s] request irq idx %d fail!\n", __func__, idx);
			return -ENODEV;
		}



 	#endif//


    return err;
}

#if 0
//static int GDMAExt_probe(struct platform_device* pdev);

static int GDMAExt_probe(struct platform_device* pdev)
{
    gdmaExt_dev *gdma = NULL;

	if(irq_of_parse_and_map(pdev->dev.of_node , 0)) {
		gdma->dev = &(pdev->dev);
		gdma->irq_no = irq_of_parse_and_map(gdma->dev->of_node , 0);
		rtd_pr_gdma_err("[%s]get irq = %d\n",__func__,gdma->irq_no);

		if (!gdma->irq_no ) {
			rtd_pr_gdma_err("[%s] No gdma irq found!\n",__func__);
			return -ENODEV;
		}
#if 1
		if(devm_request_irq(gdma->dev, gdma->irq_no,
			GDMAExt_irq_handler, IRQF_SHARED,
			"gdmaEx", (void*)gdma)) {
			rtd_pr_gdma_err("[%s] request irq fail!\n",__func__);
			return -ENODEV;
		}
#endif
	}else{
		/*if can't get irq , don't set below settings*/
 		return -ENODEV;
	}

	gdma->dev->dma_mask = &gdma->dev->coherent_dma_mask;
	if(dma_set_mask(gdma->dev, DMA_BIT_MASK(32))) {
		rtd_pr_gdma_err("[GDMA] DMA not supported\n");
	}




    return 0;
}
#endif//0

// [TODO] move most code to _probe
int GDMAExt_init( struct platform_device* pdev) 
{
   	//dev_t dev = 0;
    gdma_dev *gdma = &gdma_devices[0];


	sema_init(&gdma->sem_receiveExt, 1);

	//#ifdef ENABLE_GDMA_LINUX_REGISTER

    GDMAExt_init_main(gdma);

 	#ifdef GDMA_CONFIG_ENABLE_INTERRUPT
	

    // osd 4: idx 1,  osd5: idx 2
	
	if( GDMAExt_IS_EXIST(GDMA_PLANE_OSD4 ) ) {

		if( GDMAExt_init_os_interrupt( pdev, 1, GDMA_OSD4_irq_handler) != 0 ) {
			GDMAEXT_ERROR("GDMAExt_init_os_interrupt :%d fail\n", 1);
			return -EINVAL;
		}

		GDMAExt_init_interrupt( GDMA_PLANE_OSD4, 1 );
	}

    if( GDMAExt_IS_EXIST(GDMA_PLANE_OSD5 ) ) {

		if( GDMAExt_init_os_interrupt( pdev, 2, GDMA_OSD5_irq_handler ) != 0 ) {
			GDMAEXT_ERROR("GDMAExt_init_os_interrupt %d fail\n", 2 );
			return -EINVAL;
		}

		GDMAExt_init_interrupt( GDMA_PLANE_OSD5, 1 );
	}
    

    #if 0 //CONFIG_SMP
    	cpu_mask = (struct cpumask *)get_cpu_mask(2);
    	cpumask_set_cpu(2, cpu_mask);
    	if (irq_can_set_affinity(gic_irq_find_mapping(GDMA_IRQ)))
    		irq_set_affinity(gic_irq_find_mapping(GDMA_IRQ), cpu_mask);
    #endif

    

 	#endif//GDMA_CONFIG_ENABLE_INTERRUPT

   
	//dev = MKDEV(gdmaExt_major, gdmaExt_minor + gdmaExt_nr_devs);
    
	#if 0 //#if defined(CONFIG_REALTEK_PCBMGR)
		rtd_pr_gdma_debug("[GDMA] panel vflip=%d \n", panel_parameter->iVFLIP);
		if (panel_parameter->iVFLIP)	{
			/*  SFG_CTRL_0 */
			rtd_outl(SFG_SFG_CTRL_0_reg, rtd_inl(SFG_SFG_CTRL_0_reg) | SFG_SFG_CTRL_0_h_flip_en(1));
			/*if bootcode has already enable vfip bit ,
			we don't have to reset it again */
			if(GDMA_OSD1_CTRL_get_rotate(rtd_inl(GDMA_OSD1_CTRL_reg)) ==0) {
			GDMA_ConfigVFlip(1);
		}
			gdma->ctrl.enableVFlip = 1;
		}
	#endif //	#endif


   #if 0 //#ifdef CONFIG_PM
	gdmaExt_devs = platform_device_register_simple(DRV_NAME, -1, NULL, 0);
	result = platform_driver_register(&gdmaExt_driver);
	if ((result) != 0) {
		rtd_pr_gdma_crit("Can't register GDMAExt device driver...\n");
	} else {
		//rtd_pr_gdma_(KERN_EMERG"register GDMA device driver...\n");
		GDMA_EXT_LOG( GDMA_EXT_LOG_VERBOSE, "register GDMAExt device ok\n");
	}

  #endif



	//device_enable_async_suspend(&gdma_devs->dev);   //[FIXME] CONFIG_PM

  


	//gdma_total_ddr_size = get_memory_size_total();
	//rtd_pr_gdma_debug("[GDMA]get total memsize = %08x\n", (unsigned int)gdma_total_ddr_size);


//#if defined(CONFIG_REALTEK_PCBMGR)
#if 0
	gdma->iCONFIG_PANEL_TYPE = panel_parameter->iCONFIG_PANEL_TYPE;
	gdma->iCONFIG_PANEL_CUSTOM_INDEX = panel_parameter->iCONFIG_PANEL_CUSTOM_INDEX;
	rtd_pr_gdma_debug("[GDMA]gdma->iCONFIG_PANEL_TYPE=%d\n", gdma->iCONFIG_PANEL_TYPE);
    
	rtd_pr_gdma_debug("[GDMA]gdma->iCONFIG_PANEL_CUSTOM_INDEX=%d\n", gdma->iCONFIG_PANEL_CUSTOM_INDEX);
	/*  3d support? PR or SG mode? */
	rtd_pr_gdma_debug("[GDMA]gdma->i3D_DISPLAY_SUPPORT=%d\n", panel_parameter->i3D_DISPLAY_SUPPORT);
	rtd_pr_gdma_debug("[GDMA]gdma->i3D_LINE_ALTERNATIVE_SUPPORT=%d\n", panel_parameter->i3D_LINE_ALTERNATIVE_SUPPORT);

    if (panel_parameter->i3D_DISPLAY_SUPPORT)
		gdma->ctrl.enable3D_PR_SGMode = panel_parameter->i3D_LINE_ALTERNATIVE_SUPPORT;
#endif

	GDMA_EXT_LOG( GDMA_EXT_LOG_ALL, "%s finish.\n", __FUNCTION__);

	return 0;
}
