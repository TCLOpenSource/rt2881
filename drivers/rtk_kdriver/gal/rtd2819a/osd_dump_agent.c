#include <generated/autoconf.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/dma-mapping.h>
#include <linux/pageremap.h>
#include <linux/kthread.h>
#include <linux/time.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/syscalls.h>
#include <linux/version.h>
#ifdef CONFIG_ARM64
#include <rtk_kdriver/io.h>
#endif
#include <linux/uaccess.h>
#include <asm/barrier.h>
#include <asm/cacheflush.h>
#include <mach/platform.h>
#include <mach/system.h>
#include <rbus/sb2_reg.h>
#include "osd_dump_agent.h"
#include <rbus/gdma_reg.h>

#ifdef CONFIG_RTK_FEATURE_FOR_GKI
static struct file* file_open(const char *path, int flags, int rights) {
	struct file* filp = NULL;
	int err = 0;

	filp = filp_open(path, flags, rights);
	if(IS_ERR(filp)) {
		err = PTR_ERR(filp);
		return NULL;
	}
	return filp;
}

static void file_close(struct file *file) {
	filp_close(file, NULL);
}

static int file_write(unsigned char *data, unsigned int size, unsigned long long offset, struct file *file) {
	int ret;
	ret = kernel_write(file, data, size, &offset);
	return ret;
}

static int file_sync(struct file *file) {
	vfs_fsync(file, 0);
	return 0;
}
#endif
typedef struct {
	unsigned short type;
	unsigned int size;
	unsigned short reserved1, reserved2;
	unsigned int offset;
}__attribute__((packed))  BMP_F_H;

typedef struct {
	unsigned int size;
	int width,height;
	unsigned short planes;
	unsigned short bits;
	unsigned int compression;
	unsigned int imagesize;
	int xresolution,yresolution;
	unsigned int ncolours;
	unsigned int importantcolours;
}__attribute__((packed))  BMP_I_H;

int osd_deme_write_file(osd_dmem_info* p) {
#ifdef CONFIG_RTK_FEATURE_FOR_GKI
	char tmpStr[128];
	struct file *outfp = NULL;
	int ret= 0;
#endif
	BMP_F_H bfh;
	BMP_I_H bih;
	bfh.type = 0x4d42;
	bfh.size = 0x36+p->pitch*p->h;
	bfh.reserved1 = 0;
	bfh.reserved2 = 0;
	bfh.offset = 0x36;
	bih.size = 0x28;
	bih.width = p->w;
	bih.height = (0x100000000 - p->h);
	bih.planes = 1;
	bih.bits = 0x20;
	bih.compression = 0;
	bih.imagesize = p->pitch*p->h;
	bih.xresolution = p->w;
	bih.yresolution = p->h;
	bih.ncolours = 0;
	bih.importantcolours = 0;

#ifdef CONFIG_RTK_FEATURE_FOR_GKI

	snprintf(tmpStr, 128, "%s", p->fname);

	if (outfp== NULL)
		outfp = file_open(tmpStr, O_TRUNC | O_RDWR | O_CREAT, 0777);

	if (outfp == NULL) {
		printk(KERN_ERR"%s -  %s file open failed \n", __FUNCTION__, tmpStr);
		return -1;
	}
	file_write((unsigned char *)&bfh, sizeof(bfh), 0, outfp);
	file_write((unsigned char *)&bih, sizeof(bih),sizeof(bfh), outfp);
	file_write((unsigned char *)p->viraddr, p->w*p->h*4, sizeof(bfh) + sizeof(bih), outfp);

	flush_cache_all();
	file_sync(outfp);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
#if !defined(MODULE)
    vfs_fchmod(outfp, 777);
#endif
#endif
	file_close(outfp);
	flush_cache_all();

#ifndef MODULE
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 17, 0))
	ret = ksys_chmod(tmpStr,777);
#else
	ret = sys_chmod(tmpStr,777);
#endif
#endif
	printk(KERN_ERR"%s - %s finish!, return %d\n", __FUNCTION__, tmpStr, ret);
#endif

	return 0;

}
#if IS_ENABLED(CONFIG_RTK_KDRV_SE)
#include <rtk_kadp_se.h>
#endif
int osd_deme_cpy(osd_dmem_info* src) {
#if IS_ENABLED(CONFIG_RTK_KDRV_SE)
	KGAL_SURFACE_INFO_T ssurf;
	KGAL_SURFACE_INFO_T dsurf;
	KGAL_RECT_T srect;
	KGAL_BLIT_FLAGS_T sflag = KGAL_BLIT_NOFX;
	KGAL_BLIT_SETTINGS_T sblend;
	memset(&ssurf,0, sizeof(KGAL_SURFACE_INFO_T));
	memset(&srect,0, sizeof(KGAL_RECT_T));
	memset(&sblend,0, sizeof(KGAL_BLIT_SETTINGS_T));
	sblend.srcBlend = KGAL_BLEND_ONE;
	sblend.dstBlend = KGAL_BLEND_ZERO;
	ssurf.physicalAddress = src->src_phyaddr;
	ssurf.width = src->w;
	ssurf.height = src->h;
	ssurf.bpp = (src->pitch/src->w)*8;
	ssurf.pitch = src->pitch;
	ssurf.pixelFormat = KGAL_PIXEL_FORMAT_ARGB;
	memcpy(&dsurf,&ssurf,sizeof(KGAL_SURFACE_INFO_T));
	dsurf.physicalAddress = src->dst_phyaddr;
	if(src->fmt == KGAL_PIXEL_FORMAT_RGB888){
		dsurf.pitch = src->w*3;
		dsurf.pixelFormat = KGAL_PIXEL_FORMAT_RGB888;
		dsurf.bpp = 24;
	}else if(src->fmt == KGAL_PIXEL_FORMAT_ARGB4444){
		dsurf.pitch = src->w*2;
		dsurf.pixelFormat = KGAL_PIXEL_FORMAT_ARGB4444;
		dsurf.bpp = 16;
	}else{//src->fmt == KGAL_PIXEL_FORMAT_ARGB
		dsurf.pitch = src->w*4;
		dsurf.pixelFormat = KGAL_PIXEL_FORMAT_ARGB;
		dsurf.bpp = 32;
	}
	srect.x = src->x;
	srect.y = src->y;
	srect.w = ssurf.width;
	srect.h = ssurf.height;

	KGAL_Blit(&ssurf, &srect, &dsurf, 0, 0, &sflag, &sblend);
#else
	printk(KERN_ERR"[%s] need enable CONFIG_RTK_KDRV_SE",__func__);
#endif
	return 0;
}

/*for continue dump stch to small size */
int osd_deme_stch_cpy(osd_dmem_info* src,osd_dmem_info* dst) {
#if IS_ENABLED(CONFIG_RTK_KDRV_SE)
	KGAL_SURFACE_INFO_T ssurf;
	KGAL_SURFACE_INFO_T dsurf;
	KGAL_RECT_T srect;
	KGAL_RECT_T drect;
	KGAL_BLIT_FLAGS_T sflag = KGAL_BLIT_NOFX;
	KGAL_BLIT_SETTINGS_T sblend;
	KGAL_MULTI_SETTINGS_T multi_setting;
	unsigned int buffer_index = 0;
	unsigned int buffer_offset = 0 ;// 6*4
	int multi_en;
	int compress;
	unsigned int top_addr;
	memset(&ssurf,0, sizeof(KGAL_SURFACE_INFO_T));
	memset(&srect,0, sizeof(KGAL_RECT_T));
	memset(&drect,0, sizeof(KGAL_RECT_T));
	memset(&sblend,0, sizeof(KGAL_BLIT_SETTINGS_T));
	multi_en =  GDMA_AFBC_multi_seg_ctrl_get_afbc1_en(rtd_inl(GDMA_AFBC_multi_seg_ctrl_reg));
	compress = GDMA_DBG_OSD_WI_C_get_img_compress(rtd_inl(GDMA_DBG_OSD_WI_C_reg));
	top_addr = GDMA_DBG_OSD_WI_18_get_top_addr(rtd_inl(GDMA_DBG_OSD_WI_18_reg));
	sblend.srcBlend = KGAL_BLEND_ONE;
	sblend.dstBlend = KGAL_BLEND_ZERO;
	top_addr = src->src_phyaddr;
	ssurf.physicalAddress = src->src_phyaddr;
	ssurf.width = src->w;
	ssurf.height = src->h;
	ssurf.bpp = (src->pitch/src->w)*8;
	ssurf.pitch = src->pitch;
	ssurf.pixelFormat = src->fmt;
	memcpy(&dsurf,&ssurf,sizeof(KGAL_SURFACE_INFO_T));
	
	if(multi_en && compress)
	{
		for(buffer_index =0; buffer_index<3; buffer_index++)
		{
			buffer_offset = buffer_index*24;
			if(top_addr == rtd_inl(GDMA_AFBC1_O_start_0_reg + buffer_offset))
			{
				multi_setting.ori_start_addr1 = rtd_inl(GDMA_AFBC1_O_start_0_reg + buffer_offset);
				multi_setting.ori_end_addr1 = rtd_inl(GDMA_AFBC1_O_end_0_reg + buffer_offset);
				multi_setting.trans_start_addr1 = rtd_inl(GDMA_AFBC1_T_start_0_reg + buffer_offset);
				multi_setting.trans_end_addr1 = multi_setting.trans_start_addr1 + (multi_setting.ori_end_addr1 - multi_setting.ori_start_addr1);

				multi_setting.ori_start_addr2 = rtd_inl(GDMA_AFBC1_O_start_1_reg + buffer_offset);
				multi_setting.ori_end_addr2 = rtd_inl(GDMA_AFBC1_O_end_1_reg + buffer_offset);
				multi_setting.trans_start_addr2 = rtd_inl(GDMA_AFBC1_T_start_1_reg + buffer_offset);
				multi_setting.trans_end_addr2 = multi_setting.trans_start_addr2 + (multi_setting.ori_end_addr2 - multi_setting.ori_start_addr2);
				break;
			}
		}
	}
	dsurf.physicalAddress = src->dst_phyaddr;
	dsurf.pixelFormat = KGAL_PIXEL_FORMAT_ARGB;
	srect.x = src->x;
	srect.y = src->y;
	srect.w = ssurf.width;
	srect.h = ssurf.height;

	drect.x = dst->x;
	drect.y = dst->y;
	drect.w = ssurf.width/5;
	drect.h = ssurf.height/5;
	
	if(multi_en && compress)
	{
		KGAL_StretchBlit_Multi(&ssurf, &srect, &dsurf, &drect, &sflag, &sblend,&multi_setting);
	}
	else
	{
		KGAL_StretchBlit(&ssurf, &srect, &dsurf, &drect, &sflag, &sblend);
	}
	
	
#else
	printk(KERN_ERR"[%s] need enable CONFIG_RTK_KDRV_SE",__func__);
#endif

	return 0;
}
