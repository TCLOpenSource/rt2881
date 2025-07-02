#include <linux/compat.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/poll.h>
#include <asm/cacheflush.h>
#include <linux/proc_fs.h>
#include <linux/version.h>
#include <mach/rtk_platform.h>
#include "comm.h"
#include "demod_ext.h"
#include "demod_ext_all.h"
#include "demod_ext_common.h"

#include "ExtDemodMgr.h"
#include "demod_ext_debug.h"
#include <rtk_kdriver/pcbMgr.h>

#define EXTDEMOD_MAJOR  0


#define DRV_NAME "extdemod"

PS_EXT_DEMOD_DATA pExtDemod_globe;
EXT_DEMOD_DATA* pExtDemodData_str;

struct semaphore extdemod_Semaphore;/* This Semaphore is for rtkdemo control*/

unsigned char isExtDemodConstructors = 0;

unsigned char isUseExtDemod = 0;
unsigned char extdemod_suspend_done = 0;   // TRUE: suspend complete / FALSE: resume complete
//unsigned char atsc_demod_resume_done = 0;

/* parameters for debug */
struct proc_dir_entry * demod_ext_proc_dir = NULL;
struct proc_dir_entry * demod_ext_proc_entry = NULL;
struct device *gExtDemodDev = NULL;

#ifdef CONFIG_PM
static int ext_demod_suspend(struct device *dev)
{
	EXTDEMOD_MGR_CALLER("ext_demod_suspend begin\n");
	if (isExtDemodConstructors == 1 && isUseExtDemod == 1)
	{
		//pExtDemod_globe->m_pDemod->Suspend(pExtDemod_globe->m_pDemod);
		REALTEK_EXT_Destructors(pExtDemod_globe->m_pDemod);
		kfree(pExtDemod_globe->m_pDemod);
		pExtDemod_globe->m_pTuner->Destory(pExtDemod_globe->m_pTuner);
		kfree(pExtDemod_globe->m_pTuner);
		pExtDemod_globe->m_comm->Destory(pExtDemod_globe->m_comm);
		kfree(pExtDemod_globe->m_comm);
		kfree(pExtDemodData_str);
		isExtDemodConstructors = 0;
		isUseExtDemod = 0;
		EXTDEMOD_MGR_INFO("ext_demod_suspend done\n");
	}
	EXTDEMOD_MGR_CALLER("ext_demod_suspend end\n");
	extdemod_suspend_done = 1;

	return 0;
}

extern struct device* get_i2c_device_ptr_demod_use(void);

static int ext_demod_resume(struct device *dev)
{
	unsigned long long param;
	EXTDEMOD_MGR_CALLER("ext_demod_resume begin\n");
	

	if (isExtDemodConstructors == 1 && isUseExtDemod == 1)
	{
		EXTDEMOD_MGR_INFO("ext_demod_resume do nothing\n");
		//pExtDemod_globe->m_pDemod->SetTvMode(pExtDemod_globe->m_pDemod, (TV_SYSTEM_TYPE)pExtDemod_globe->set_tv_mode_data.mode);
	} 
	else
	{
		pExtDemod_globe->m_comm = kmalloc(sizeof(COMM), GFP_KERNEL);
		if (pcb_mgr_get_enum_info_byname("EXTDEMOD_I2C", &param) != 0) {
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: Get EXTDEMOD_I2C port fail use default !!!!!!!!!!!!!!!\n", __LINE__);
			param = 2;
		}
		EXTDEMOD_MGR_INFO("EXTDEMOD[%d]: set EXTDEMOD_I2C port = %d\n", __LINE__, param);
		Comm_Constructors_Ext(pExtDemod_globe->m_comm, (unsigned char)param);
		//Comm_Constructors_Ext(pExtDemod_globe->m_comm, 2);	//Demo Board
		pExtDemod_globe->m_pTuner = kmalloc(sizeof(TUNER), GFP_KERNEL);
		Tuner_Constructors_Ext(pExtDemod_globe->m_pTuner);
		pExtDemod_globe->m_pDemod = kmalloc(sizeof(DEMOD), GFP_KERNEL);
		pExtDemodData_str = kmalloc(sizeof(EXT_DEMOD_DATA), GFP_KERNEL);
		REALTEK_EXT_Constructors(pExtDemod_globe->m_pDemod, pExtDemodData_str->demod_init_data.m_addr, pExtDemodData_str->demod_init_data.m_output_mode, pExtDemodData_str->demod_init_data.m_output_freq, pExtDemod_globe->m_comm);
		pExtDemod_globe->m_pDemod->AttachTuner(pExtDemod_globe->m_pDemod, pExtDemod_globe->m_pTuner);
		//pExtDemod_globe->fpInit = filp;
		isExtDemodConstructors = 1;
		EXTDEMOD_MGR_INFO("ext_demod_resume done\n");
	}

	EXTDEMOD_MGR_CALLER("rtk_demod_resume end\n");
	extdemod_suspend_done = 0;
	return 0;
}

static int ext_demod_suspend_std(struct device *dev)
{
	EXTDEMOD_MGR_CALLER(KERN_NOTICE "ext_demod_suspend_std\n");
	if (isExtDemodConstructors == 1 && isUseExtDemod == 1)
		pExtDemod_globe->m_pDemod->Suspend(pExtDemod_globe->m_pDemod);
	extdemod_suspend_done = 1;
	EXTDEMOD_MGR_INFO("ext_demod_suspend_std done\n");
	return 0;
}

static int ext_demod_resume_std(struct device *dev)
{
	EXTDEMOD_MGR_CALLER(KERN_NOTICE "ext_demod_resume\n");
	extdemod_suspend_done = 0;
	EXTDEMOD_MGR_INFO("ext_demod_resume_std done\n");
	return 0;
}
#endif

static int extdemod_major = EXTDEMOD_MAJOR;
static int extdemod_minor;

module_param(extdemod_major, int, S_IRUGO);
module_param(extdemod_minor, int, S_IRUGO);

static struct dev_pm_ops const ext_demod_plat_pm_ops = {
	.suspend     = ext_demod_suspend,
	.resume      = ext_demod_resume,
	.freeze      = ext_demod_suspend_std,
	.thaw        = ext_demod_resume_std,
	.poweroff    = ext_demod_suspend_std,
	.restore     = ext_demod_resume_std,
};

static const char drv_name[] = DRV_NAME;

static struct class *ext_demod_class;
static struct platform_device *ext_demod_platform_devs;
static struct platform_driver  ext_demod_device_driver
	= {
	.driver = {
		.name       = (char *)drv_name,
		.bus        = &platform_bus_type,
#ifdef CONFIG_PM
		.pm 	  = &ext_demod_plat_pm_ops,
#endif
	},
};

struct device* get_extdemod_device_ptr(void)
{
	return &(ext_demod_platform_devs->dev);
}

static int ext_demod_open(struct inode *inode, struct file *filp)
{
	EXTDEMOD_MGR_CALLER("func %s, line %d, NOT implemented!!\n", __func__, __LINE__);
	return 0;
}

static int ext_demod_release(struct inode *inode, struct file *filp)
{
	EXTDEMOD_MGR_CALLER("func %s, line %d\n", __func__, __LINE__);

	if (filp == pExtDemod_globe->fpInit) {
		if (down_interruptible(&pExtDemod_globe->sem))
			return -ERESTARTSYS;


		up(&pExtDemod_globe->sem);
	}

	return 0;
}

static long ext_demod_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = -ENOTTY;
	unsigned long long param;
	TV_SIG_QUAL quality_temp;
	EXT_DEMOD_DATA* pExtDemodData;

	if (down_interruptible(&pExtDemod_globe->sem))
		return -ERESTARTSYS;

	down(&extdemod_Semaphore);

	pExtDemodData = kmalloc(sizeof(EXT_DEMOD_DATA), GFP_KERNEL);

	if (pExtDemodData == NULL) {
		ret = -EFAULT;
		EXTDEMOD_MGR_WARNING("pExtDemodData kmalloc Failed (ext_demod_ioctl) !!!!!!!!!!!!!!!\n");
		kfree(pExtDemodData);

		up(&extdemod_Semaphore);
		up(&pExtDemod_globe->sem);
		return ret;
	}


	switch (cmd) {
	case IOCTL_EXT_DEMOD_CONSTRUCTORS:
		//EXTDEMOD_MGR_CALLER("[ext_demod] IOCTL_EXT_DEMOD_CONSTRUCTORS\n");
		EXTDEMOD_MGR_INFO("[ext_demod] IOCTL_EXT_DEMOD_CONSTRUCTORS\n");
		if (copy_from_user((void *)&pExtDemodData->demod_init_data, (const void __user *)arg, sizeof(EXT_DEMOD_CONSTRUCTORS))) {
			ret = -EFAULT;
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: IOCTL_EXT_DEMOD_CONSTRUCTORS failed !!!!!!!!!!!!!!!\n", __LINE__);
		} else {
			//EXTDEMOD_MGR_DBG("ext_demod demod_init_data.m_addr = %d(0x%x)\n", pExtDemod_globe->demod_init_data.m_addr, pExtDemod_globe->demod_init_data.m_addr);
			//EXTDEMOD_MGR_DBG("ext_demod demod_init_data.m_output_mode = %d(0x%x)\n", pExtDemod_globe->demod_init_data.m_output_mode, pExtDemod_globe->demod_init_data.m_output_mode);
			//EXTDEMOD_MGR_DBG("ext_demod demod_init_data.m_output_freq = %d(0x%x)\n", pExtDemod_globe->demod_init_data.m_output_freq, pExtDemod_globe->demod_init_data.m_output_freq);

			if(isExtDemodConstructors == 1){
				EXTDEMOD_MGR_WARNING("EXTDEMOD already constructored !!!! \n");
				break;
			}
			pExtDemod_globe->m_comm = kmalloc(sizeof(COMM), GFP_KERNEL);

			if (pcb_mgr_get_enum_info_byname("EXTDEMOD_I2C", &param) != 0) {
				EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: Get EXTDEMOD_I2C port fail use default !!!!!!!!!!!!!!!\n", __LINE__);
				param = 2;
			}

			EXTDEMOD_MGR_INFO("EXTDEMOD[%d]: set EXTDEMOD_I2C port = %d\n", __LINE__, param);
			Comm_Constructors_Ext(pExtDemod_globe->m_comm, (unsigned char)param);
/* change to dynamic I2C port by PCB
#ifdef CONFIG_CUSTOMER_TV006
			Comm_Constructors_Ext(pExtDemod_globe->m_comm, 2);
#else
			Comm_Constructors_Ext(pExtDemod_globe->m_comm, 2);	//Demo Board
#endif
*/

			pExtDemod_globe->m_pTuner = kmalloc(sizeof(TUNER), GFP_KERNEL);
			Tuner_Constructors_Ext(pExtDemod_globe->m_pTuner);
			pExtDemod_globe->m_pDemod = kmalloc(sizeof(DEMOD), GFP_KERNEL);
			REALTEK_EXT_Constructors(pExtDemod_globe->m_pDemod, pExtDemodData->demod_init_data.m_addr, pExtDemodData->demod_init_data.m_output_mode, pExtDemodData->demod_init_data.m_output_freq, pExtDemod_globe->m_comm);
			pExtDemod_globe->m_pDemod->AttachTuner(pExtDemod_globe->m_pDemod, pExtDemod_globe->m_pTuner);
			ret = 0;
			pExtDemod_globe->fpInit = filp;
			isExtDemodConstructors = 1;
		}
		break;

	case IOCTL_EXT_DEMOD_SETTVMODE:

		if (isExtDemodConstructors != 1) {
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: need do IOCTL_EXT_DEMOD_CONSTRUCTORS first !!!!!!!!!!!!!!!\n", __LINE__);
			break;
		}

		//EXTDEMOD_MGR_CALLER("[ext_demod] IOCTL_EXT_DEMOD_SETTVMODE\n");
		isUseExtDemod = 1;

		if (copy_from_user((void *)&pExtDemod_globe->set_tv_mode_data, (const void __user *)arg, sizeof(EXT_DEMOD_SET_TVMODE))) {
			ret = -EFAULT;
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: ioctl IOCTL_EXT_DEMOD_SETTVMODE failed !!!!!!!!!!!!!!!\n", __LINE__);
		} else {
			//EXTDEMOD_MGR_DBG("ext_demod set_tv_mode_data.mode = %d(0x%x)\n", pExtDemod_globe->set_tv_mode_data.mode, pExtDemod_globe->set_tv_mode_data.mode);

			pExtDemod_globe->m_pDemod->SetTvMode(pExtDemod_globe->m_pDemod, (TV_SYSTEM_TYPE)pExtDemod_globe->set_tv_mode_data.mode);
			ret = 0;
		}
		break;

	case IOCTL_EXT_DEMOD_SETTVSYS:

		if (isExtDemodConstructors != 1) {
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: need do IOCTL_EXT_DEMOD_CONSTRUCTORS first !!!!!!!!!!!!!!!\n", __LINE__);
			break;
		}

		//EXTDEMOD_MGR_CALLER("[ext_demod] IOCTL_RTK_DEMOD_SETTVSYS\n");
		isUseExtDemod = 1;

		if (copy_from_user((void *)&pExtDemodData->set_tv_sys_ex_data, (const void __user *)arg, sizeof(EXT_DEMOD_SETTVSYSEX))) {
			ret = -EFAULT;
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: ioctl IOCTL_RTK_DEMOD_SETTVSYSEX failed !!!!!!!!!!!!!!!\n", __LINE__);
		} else {
			//EXTDEMOD_MGR_DBG("ext_demod set_tv_sys_ex_data.sys = %d\n", pExtDemod_globe->set_tv_sys_ex_data.sys);

			pExtDemod_globe->m_pDemod->SetTVSysEx(pExtDemod_globe->m_pDemod, pExtDemodData->set_tv_sys_ex_data.sys, NULL);
			ret = 0;
		}
		break;

	case IOCTL_EXT_DEMOD_SETTVSYSEX:

		if (isExtDemodConstructors != 1) {
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: need do IOCTL_EXT_DEMOD_CONSTRUCTORS first !!!!!!!!!!!!!!!\n", __LINE__);
			break;
		}

		//EXTDEMOD_MGR_CALLER("[ext_demod] IOCTL_RTK_DEMOD_SETTVSYSEX\n");
		isUseExtDemod = 1;
		if (copy_from_user((void *)&pExtDemodData->set_tv_sys_ex_data, (const void __user *)arg, sizeof(EXT_DEMOD_SETTVSYSEX))) {
			ret = -EFAULT;
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: ioctl IOCTL_RTK_DEMOD_SETTVSYSEX failed !!!!!!!!!!!!!!!\n", __LINE__);
		} else {
			//EXTDEMOD_MGR_DBG("ext_demod set_tv_sys_ex_data.sys = %d\n", pExtDemod_globe->set_tv_sys_ex_data.sys);

			pExtDemod_globe->m_pDemod->SetTVSysEx(pExtDemod_globe->m_pDemod, pExtDemodData->set_tv_sys_ex_data.sys, &pExtDemodData->set_tv_sys_ex_data.pSigInfo);
			ret = 0;
		}
		break;

	case IOCTL_EXT_DEMOD_ACQUIRESIGNAL:

		if (isExtDemodConstructors != 1) {
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: need do IOCTL_EXT_DEMOD_CONSTRUCTORS first !!!!!!!!!!!!!!!\n", __LINE__);
			break;
		}

		//EXTDEMOD_MGR_CALLER("[ext_demod] IOCTL_RTK_DEMOD_ACQUIRESIGNAL\n");
		if (copy_from_user((void *)&pExtDemodData->set_acq_sig_data, (const void __user *)arg, sizeof(EXT_DEMOD_SET_ACQ_SIG))) {
			//ret = -EFAULT;
			ret = 0;
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: ioctl IOCTL_RTK_DEMOD_ACQUIRESIGNAL failed !!!!!!!!!!!!!!!\n", __LINE__);
		} else {
			//EXTDEMOD_MGR_DBG("ext_demod set_acq_sig_data.WaitSignalLock = %d\n", pExtDemod_globe->set_acq_sig_data.WaitSignalLock);

			pExtDemodData->set_acq_sig_data.return_value = pExtDemod_globe->m_pDemod->AcquireSignal(pExtDemod_globe->m_pDemod, pExtDemodData->set_acq_sig_data.WaitSignalLock);
			if (copy_to_user((void __user *)arg, &pExtDemodData->set_acq_sig_data, sizeof(EXT_DEMOD_SET_ACQ_SIG)))
				ret = 0;
			else
				ret = -EFAULT;
		}
		break;

	case IOCTL_EXT_DEMOD_DESTRUCTORS:
		EXTDEMOD_MGR_CALLER("IOCTL_EXT_DEMOD_DESTRUCTORS\n");
		EXTDEMOD_MGR_INFO("[ext_demod] IOCTL_EXT_DEMOD_DESTRUCTORS\n");
		REALTEK_EXT_Destructors(pExtDemod_globe->m_pDemod);
		kfree(pExtDemod_globe->m_pDemod);
		pExtDemod_globe->m_pTuner->Destory(pExtDemod_globe->m_pTuner);
		kfree(pExtDemod_globe->m_pTuner);
		pExtDemod_globe->m_comm->Destory(pExtDemod_globe->m_comm);
		kfree(pExtDemod_globe->m_comm);
		isExtDemodConstructors = 0;
		ret = 0;
		break;

	case IOCTL_RTK_DEMOD_GETLOCK:

		if (isExtDemodConstructors != 1) {
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: need do IOCTL_EXT_DEMOD_CONSTRUCTORS first !!!!!!!!!!!!!!!\n", __LINE__);
			break;
		}

		pExtDemod_globe->m_pDemod->GetLockStatus(pExtDemod_globe->m_pDemod, &pExtDemodData->get_lock_data.lock);
		ret = 0;

		if (copy_to_user((void __user *)arg, &pExtDemodData->get_lock_data, sizeof(EXT_DEMOD_GET_LOCK)))
			EXTDEMOD_MGR_INFO("[ext_demod] IOCTL_RTK_DEMOD_GETLOCK\n");
		break;

	case IOCTL_RTK_DEMOD_GETCHANNELINFO:

		if (isExtDemodConstructors != 1) {
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: need do IOCTL_EXT_DEMOD_CONSTRUCTORS first !!!!!!!!!!!!!!!\n", __LINE__);
			break;
		}

		pExtDemod_globe->m_pDemod->GetChannelInfo(pExtDemod_globe->m_pDemod, &pExtDemodData->get_channelinfo_data.Standard, &pExtDemodData->get_channelinfo_data.Modulation, &pExtDemodData->get_channelinfo_data.CodeRate, &pExtDemodData->get_channelinfo_data.PilotOnOff, &pExtDemodData->get_channelinfo_data.Inverse);
		ret = 0;
		if (copy_to_user((void __user *)arg, &pExtDemodData->get_channelinfo_data, sizeof(EXT_DEMOD_GET_CHANNELINFO)))
			EXTDEMOD_MGR_INFO("[ext_demod] IOCTL_RTK_DEMOD_GETCHANNELINFO\n");
		break;

	case IOCTL_RTK_DEMOD_AUTO_TUNE:

		if (isExtDemodConstructors != 1) {
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: need do IOCTL_EXT_DEMOD_CONSTRUCTORS first !!!!!!!!!!!!!!!\n", __LINE__);
			break;
		}

		pExtDemod_globe->m_pDemod->AutoTune(pExtDemod_globe->m_pDemod);
		ret = 0;
		break;

	case IOCTL_RTK_DEMOD_SETMODE:

		if (isExtDemodConstructors != 1) {
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: need do IOCTL_EXT_DEMOD_CONSTRUCTORS first !!!!!!!!!!!!!!!\n", __LINE__);
			break;
		}

		//EXTDEMOD_MGR_CALLER("[ext_demod] IOCTL_RTK_DEMOD_SETMODE\n");

		if (copy_from_user((void *)&pExtDemodData->set_mode_data, (const void __user *)arg, sizeof(EXT_DEMOD_SETMODE))) {
			ret = -EFAULT;
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: ioctl IOCTL_RTK_DEMOD_SETMODE failed !!!!!!!!!!!!!!!\n", __LINE__);
		} else {
			//EXTDEMOD_MGR_DBG("ext_demod set_mode_data.mode = %d\n", pExtDemod_globe->set_mode_data.mode);

			pExtDemod_globe->m_pDemod->SetMode(pExtDemod_globe->m_pDemod, (TV_OP_MODE)pExtDemodData->set_mode_data.mode);
			ret = 0;
		}

		break;
	case IOCTL_EXT_DEMOD_SETTVPARA:


		if (isExtDemodConstructors != 1) {
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: need do IOCTL_EXT_DEMOD_CONSTRUCTORS first !!!!!!!!!!!!!!!\n", __LINE__);
			break;
		}

		//EXTDEMOD_MGR_CALLER("[ext_demod] IOCTL_RTK_DEMOD_SETTVPARA\n");

		if (copy_from_user((void *)&pExtDemodData->set_tv_para, (const void __user *)arg, sizeof(EXT_DEMOD_SET_TVPARA))) {
			ret = -EFAULT;
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: ioctl IOCTL_RTK_DEMOD_SETTVPARA failed !!!!!!!!!!!!!!!\n", __LINE__);
		} else {
			//EXTDEMOD_MGR_INFO("ext_demod set_tv_para.id = %d\n", pExtDemodData->set_tv_para.id);
			//EXTDEMOD_MGR_INFO("ext_demod set_tv_para.std = %d\n", pExtDemodData->set_tv_para.std);
			//EXTDEMOD_MGR_INFO("ext_demod set_tv_para.step_freq = %d\n", pExtDemodData->set_tv_para.step_freq);
			//EXTDEMOD_MGR_INFO("ext_demod set_tv_para.if_freq = %d\n", pExtDemodData->set_tv_para.if_freq);
			//EXTDEMOD_MGR_INFO("ext_demod set_tv_para.if_inversion = %d\n", pExtDemodData->set_tv_para.if_inversion);
			//EXTDEMOD_MGR_INFO("ext_demod set_tv_para.if_agc_pola = %d\n", pExtDemodData->set_tv_para.if_agc_pola);
			//EXTDEMOD_MGR_INFO("ext_demod set_tv_para.min_freq= %d\n", pExtDemodData->set_tv_para.min_freq);
			//EXTDEMOD_MGR_INFO("ext_demod set_tv_para.max_freq = %d\n", pExtDemodData->set_tv_para.max_freq);
			//EXTDEMOD_MGR_INFO("ext_demod set_tv_para.lowband_midband_limit_freq = %d\n", pExtDemodData->set_tv_para.lowband_midband_limit_freq);
			//EXTDEMOD_MGR_INFO("ext_demod set_tv_para.midband_highband_limit_freq = %d\n", pExtDemodData->set_tv_para.midband_highband_limit_freq);

			pExtDemod_globe->m_pTuner->TunerParam.id = pExtDemodData->set_tv_para.id;
			pExtDemod_globe->m_pTuner->TunerParam.std = pExtDemodData->set_tv_para.std;
			pExtDemod_globe->m_pTuner->TunerParam.step_freq = pExtDemodData->set_tv_para.step_freq;
			pExtDemod_globe->m_pTuner->TunerParam.if_freq = pExtDemodData->set_tv_para.if_freq;
			pExtDemod_globe->m_pTuner->TunerParam.if_inversion = pExtDemodData->set_tv_para.if_inversion;
			pExtDemod_globe->m_pTuner->TunerParam.if_agc_pola = pExtDemodData->set_tv_para.if_agc_pola;
			pExtDemod_globe->m_pTuner->TunerParam.min_freq = pExtDemodData->set_tv_para.min_freq;
			pExtDemod_globe->m_pTuner->TunerParam.max_freq = pExtDemodData->set_tv_para.max_freq;
			pExtDemod_globe->m_pTuner->TunerParam.lowband_midband_limit_freq = pExtDemodData->set_tv_para.lowband_midband_limit_freq;
			pExtDemod_globe->m_pTuner->TunerParam.midband_highband_limit_freq = pExtDemodData->set_tv_para.midband_highband_limit_freq;
			ret = 0;
		}

		break;
	case IOCTL_RTK_DEMOD_GET_DEMOD_INFO:

		if (isExtDemodConstructors != 1) {
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: need do IOCTL_EXT_DEMOD_CONSTRUCTORS first !!!!!!!!!!!!!!!\n", __LINE__);
			break;
		}

		//EXTDEMOD_MGR_CALLER("[ext_demod] IOCTL_RTK_DEMOD_GET_DEMOD_INFO\n");
		pExtDemod_globe->m_pDemod->GetDemodInfo(pExtDemod_globe->m_pDemod, (TV_DEMOD_INFO*)&pExtDemodData->get_demod_info_data);
		if (copy_to_user((void __user *)arg, &pExtDemodData->get_demod_info_data, sizeof(EXT_DEMOD_GET_DEMOD_INFO)))
			EXTDEMOD_MGR_INFO("[ext_demod] IOCTL_RTK_DEMOD_GET_DEMOD_INFO\n");
		ret = 0;
		break;

	case IOCTL_RTK_DEMOD_GET_DEBUG_INFO:

		if (isExtDemodConstructors != 1) {
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: need do IOCTL_EXT_DEMOD_CONSTRUCTORS first !!!!!!!!!!!!!!!\n", __LINE__);
			break;
		}

		//EXTDEMOD_MGR_CALLER("[ext_demod] IOCTL_RTK_DEMOD_GET_DEBUG_INFO\n");
		if (copy_from_user((void *)&pExtDemodData->get_debug_info_data, (const void __user *)arg, sizeof(EXT_DEMOD_GET_DEBUG_INFO))) {
			ret = -EFAULT;
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: ioctl IOCTL_RTK_DEMOD_GET_DEBUG_INFO failed !!!!!!!!!!!!!!!\n", __LINE__);
		} else {
			pExtDemod_globe->m_pDemod->DebugInfo(pExtDemod_globe->m_pDemod, pExtDemodData->get_debug_info_data.debug_mode);
			ret = 0;
		}
		break;

	case IOCTL_RTK_DEMOD_GET_SIGNAL_INFO:

		if (isExtDemodConstructors != 1) {
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: need do IOCTL_EXT_DEMOD_CONSTRUCTORS first !!!!!!!!!!!!!!!\n", __LINE__);
			break;
		}

		pExtDemod_globe->m_pDemod->GetSignalInfo(pExtDemod_globe->m_pDemod, (TV_SIG_INFO*)&pExtDemodData->get_signal_info_data);
		if (copy_to_user((void __user *)arg, &pExtDemodData->get_signal_info_data, sizeof(EXT_DEMOD_GET_SIGNAL_INFO)))
			EXTDEMOD_MGR_CALLER("[ext_demod] IOCTL_RTK_DEMOD_GET_SIGNAL_INFO\n");
		ret = 0;
		break;

	case IOCTL_RTK_DEMOD_INSPECT_SIGNAL:

		if (isExtDemodConstructors != 1) {
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: need do IOCTL_EXT_DEMOD_CONSTRUCTORS first !!!!!!!!!!!!!!!\n", __LINE__);
			break;
		}

		pExtDemod_globe->m_pDemod->InspectSignal(pExtDemod_globe->m_pDemod);
		ret = 0;
		break;

	case IOCTL_EXT_DEMOD_SET_DEMODINFO:
		if (copy_from_user((void *)&pExtDemod_globe->set_extdemod_info, (const void __user *)arg, sizeof(EXT_DEMOD_SET_DEMOD_INFO))) {
			ret = -EFAULT;
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: ioctl EXT_DEMOD_SET_EXTDEMOD_INFO failed !!!!!!!!!!!!!!!\n", __LINE__);
		} else {
			pExtDemod_globe->m_pDemod->SetExtDemodInfo(pExtDemod_globe->m_pDemod, &pExtDemod_globe->set_extdemod_info.pSigInfo);
			ret = 0;
		}
		break;
	case IOCTL_EXT_DEMOD_GETQUALITY:
		if (copy_from_user((void *)&pExtDemodData->get_quality_ext, (const void __user *)arg, sizeof(EXT_DEMOD_GET_QUALITY_EXT))) {
			ret = -EFAULT;
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: ioctl IOCTL_EXT_DEMOD_GETQUALITY failed !!!!!!!!!!!!!!!\n", __LINE__);
		} else {
			quality_temp.rflevel = pExtDemodData->get_quality_ext.pQuality.rflevel;
			quality_temp.demod_i2cAddr = pExtDemodData->get_quality_ext.pQuality.demod_i2cAddr;

			if(isExtDemodConstructors == 1){
				pExtDemod_globe->m_pDemod->GetSignalQuality_Ext(pExtDemod_globe->m_pDemod, (TV_SYSTEM_TYPE)pExtDemodData->get_quality_ext.mode, (ENUM_TV_QUAL)pExtDemodData->get_quality_ext.id, (TV_SIG_QUAL*)&quality_temp);
				ret = 0;
			}
			else {
				break;
			}

			switch (pExtDemodData->get_quality_ext.id) {
			case TV_QUAL_SNR :
				pExtDemodData->get_quality_ext.pQuality.snr = quality_temp.snr;
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.demod_ret = quality_temp.ExtIsdb_Status.demod_ret;
				break;
			case TV_QUAL_AGC:
				pExtDemodData->get_quality_ext.pQuality.agc = quality_temp.agc;
				break;
			case TV_QUAL_CARRIER_OFFSET:
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.freq_offset = quality_temp.ExtIsdb_Status.freq_offset;
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.demod_ret = quality_temp.ExtIsdb_Status.demod_ret;
				break;
			case TV_QUAL_ISDB_PER:
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.isdbtPER[0] = quality_temp.ExtIsdb_Status.isdbtPER[0];
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.isdbtPER[1] = quality_temp.ExtIsdb_Status.isdbtPER[1];
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.isdbtPER[2] = quality_temp.ExtIsdb_Status.isdbtPER[2];
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.isdbsPER[0] = quality_temp.ExtIsdb_Status.isdbsPER[0];
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.isdbsPER[1] = quality_temp.ExtIsdb_Status.isdbsPER[1];
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.demod_ret = quality_temp.ExtIsdb_Status.demod_ret;
				break;
			case TV_QUAL_PREBCHBER:
				pExtDemodData->get_quality_ext.pQuality.ExtIsdbs3_Status.preBCHBerH = quality_temp.ExtIsdbs3_Status.preBCHBerH;
				pExtDemodData->get_quality_ext.pQuality.ExtIsdbs3_Status.preBCHBerL = quality_temp.ExtIsdbs3_Status.preBCHBerL;
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.demod_ret = quality_temp.ExtIsdb_Status.demod_ret;
				break;
			case TV_QUAL_PRERSBER:
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.preRSBER[0] = quality_temp.ExtIsdb_Status.preRSBER[0];
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.preRSBER[1] = quality_temp.ExtIsdb_Status.preRSBER[1];
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.preRSBER[2] = quality_temp.ExtIsdb_Status.preRSBER[2];
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.demod_ret = quality_temp.ExtIsdb_Status.demod_ret;
				break;
			case TV_QUAL_LOCKSTATUS:
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.lock[0] = quality_temp.ExtIsdb_Status.lock[0];
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.lock[1] = quality_temp.ExtIsdb_Status.lock[1];
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.lock[2] = quality_temp.ExtIsdb_Status.lock[2];
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.demod_ret = quality_temp.ExtIsdb_Status.demod_ret;
				break;
			case TV_QUAL_TSIDERROR:
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.TSIDError = quality_temp.ExtIsdb_Status.TSIDError;
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.demod_ret = quality_temp.ExtIsdb_Status.demod_ret;
				break;
			case TV_QUAL_PRELDPCBER:
				pExtDemodData->get_quality_ext.pQuality.ExtIsdbs3_Status.preLDPCBer[0] = quality_temp.ExtIsdbs3_Status.preLDPCBer[0];
				pExtDemodData->get_quality_ext.pQuality.ExtIsdbs3_Status.preLDPCBer[1] = quality_temp.ExtIsdbs3_Status.preLDPCBer[1];
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.demod_ret = quality_temp.ExtIsdb_Status.demod_ret;
				break;
			case TV_QUAL_POSTBCHFER:
				pExtDemodData->get_quality_ext.pQuality.ExtIsdbs3_Status.postBCHFer[0] = quality_temp.ExtIsdbs3_Status.postBCHFer[0];
				pExtDemodData->get_quality_ext.pQuality.ExtIsdbs3_Status.postBCHFer[1] = quality_temp.ExtIsdbs3_Status.postBCHFer[1];
				pExtDemodData->get_quality_ext.pQuality.ExtIsdb_Status.demod_ret = quality_temp.ExtIsdb_Status.demod_ret;
				break;

			default:
				ret = -ENOTTY;
			}

			if (copy_to_user((void __user *)arg, &pExtDemodData->get_quality_ext, sizeof(EXT_DEMOD_GET_QUALITY_EXT)))
				EXTDEMOD_MGR_INFO("[ext_demod] IOCTL_RTK_DEMOD_GETQUALITY\n");
		}
		break;

	case IOCTL_EXT_DEMOD_SETTSMODE:
		//EXTDEMOD_MGR_CALLER("[ext_demod] IOCTL_EXT_DEMOD_SETTSMODE\n");

		if (copy_from_user((void *)&pExtDemodData->set_ts_mode_data, (const void __user *)arg, sizeof(EXT_DEMOD_SET_TS_MODE))) {
			ret = -EFAULT;
			EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: ioctl IOCTL_EXT_DEMOD_SETTSMODE failed !!!!!!!!!!!!!!!\n", __LINE__);
		} else {
			//EXTDEMOD_MGR_INFO("ext_demod set_ts_mode_data.tsOutEn = %u\n", pExtDemod_globe->set_ts_mode_data.tsOutEn);
			if(isExtDemodConstructors == 1){
				pExtDemod_globe->m_pDemod->SetTSMode_Ext(pExtDemod_globe->m_pDemod, (TS_PARAM*)&pExtDemodData->set_ts_mode_data);
			}
			ret = 0;
		}
		break;

	default:
		EXTDEMOD_MGR_WARNING("EXTDEMOD[%d]: unknown ioctl(0x%08x)\n", __LINE__, cmd);
		break;
	}


	kfree(pExtDemodData);

	up(&extdemod_Semaphore);
	up(&pExtDemod_globe->sem);
	return ret;
}

unsigned int ext_demod_poll(struct file *filp, poll_table *wait)
{
	if (pExtDemod_globe->filter_flag > 0) {
		pExtDemod_globe->filter_flag = 0;
		return POLLIN | POLLRDNORM;
	} else {
		return 0;
	}
}

#ifdef CONFIG_COMPAT
long ext_demod_compat_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	void __user *compat_arg = compat_ptr(arg);
	return ext_demod_ioctl(filp, cmd, (unsigned long)compat_arg);
}
#endif

struct file_operations extdemod_fops = {
	.owner              = THIS_MODULE,
	.unlocked_ioctl     = ext_demod_ioctl,
	.open               = ext_demod_open,
	.release            = ext_demod_release,
	.poll               = ext_demod_poll,
#ifdef CONFIG_COMPAT
	.compat_ioctl = ext_demod_compat_ioctl,
#endif

};

static char *ext_demod_devnode(struct device *dev, umode_t *mode)
{
	return NULL;
}

static void ext_demod_exit(void)
{

	dev_t dev = MKDEV(extdemod_major, extdemod_minor);

	if (ext_demod_platform_devs == NULL)
		BUG();

	device_destroy(ext_demod_class, dev);
	cdev_del(&pExtDemod_globe->cdev);
	platform_driver_unregister(&ext_demod_device_driver);
	platform_device_unregister(ext_demod_platform_devs);
	kfree(pExtDemod_globe);
	ext_demod_platform_devs = NULL;
	class_destroy(ext_demod_class);
	ext_demod_class = NULL;
	unregister_chrdev_region(dev, 1);
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
static const struct proc_ops extdemod_proc_fops = {
	.proc_write  = extdemod_proc_write,
	.proc_read  = extdemod_proc_read,
};
#else
static const struct file_operations extdemod_proc_fops = {
	.owner = THIS_MODULE,
	.write  = extdemod_proc_write,
	.read  = extdemod_proc_read,
};
#endif

static int __init ext_demod_init(void)
{
	int result;
	dev_t dev = 0;

	/*	EXTDEMOD_MGR_CALLER("\n\n\n\n *****************	  demod init module  %s*********************\n\n\n\n", __TIME__); // fix compile error */
	EXTDEMOD_MGR_CALLER("\n\n\n\n *****************	  demod init module  *********************\n\n\n\n");
	if (get_product_type() == PRODUCT_TYPE_DIAS) {
		EXTDEMOD_MGR_WARNING("ext demod not support on dias platform\n");
		return 0;
	}

	if (extdemod_major) {
		dev = MKDEV(EXTDEMOD_MAJOR, 0);
		result = register_chrdev_region(dev, 1, DRV_NAME);
	} else {
		result = alloc_chrdev_region(&dev, extdemod_minor, 1, DRV_NAME);
		extdemod_major = MAJOR(dev);
	}

	if (result < 0) {
		EXTDEMOD_MGR_WARNING("demod: can not get chrdev region...\n");
		return result;
	}

	ext_demod_class = class_create(THIS_MODULE, DRV_NAME);
	if (IS_ERR(ext_demod_class)) {
		EXTDEMOD_MGR_WARNING("demod: can not create class...\n");
		result = PTR_ERR(ext_demod_class);
		goto fail_class_create;
	}

	ext_demod_class->devnode = ext_demod_devnode;
	pExtDemod_globe = kmalloc(sizeof(SEXT_DEMOD_DATA), GFP_KERNEL);
	if (!pExtDemod_globe) {
		ext_demod_exit();
		return -ENOMEM;
	}

	ext_demod_platform_devs = platform_device_register_simple(DRV_NAME, -1, NULL, 0);
	if (platform_driver_register(&ext_demod_device_driver) != 0) {
		EXTDEMOD_MGR_WARNING("extdemod: can not register platform driver...\n");
		result = -EINVAL;
		goto fail_platform_driver_register;
	}

	memset(pExtDemod_globe, 0, sizeof(SEXT_DEMOD_DATA));
	cdev_init(&pExtDemod_globe->cdev, &extdemod_fops);
	pExtDemod_globe->cdev.owner = THIS_MODULE;
	pExtDemod_globe->cdev.ops   = &extdemod_fops;
	if (cdev_add(&pExtDemod_globe->cdev, dev, 1) < 0) {
		EXTDEMOD_MGR_WARNING("extdemod: can not add character device...\n");
		goto fail_cdev_init;
	}

	gExtDemodDev = device_create(ext_demod_class, NULL, dev, NULL, DRV_NAME);
	gExtDemodDev->coherent_dma_mask = DMA_BIT_MASK(32);
	gExtDemodDev->dma_mask = &gExtDemodDev->coherent_dma_mask;

	if(dma_set_mask(gExtDemodDev, DMA_BIT_MASK(32))) {
		EXTDEMOD_MGR_WARNING("DMA not supported\n");
	}
	sema_init(&extdemod_Semaphore, 1);
	sema_init(&pExtDemod_globe->sem, 1);

	/* [[KTASKWBS-1065] Make demod resume run at asynchronous for parallel resume time*/
	device_enable_async_suspend(get_extdemod_device_ptr());

#ifdef CONFIG_PROC_FS
	/* add proc entry */
	if ((demod_ext_proc_dir == NULL) && (demod_ext_proc_entry == NULL)) {
		demod_ext_proc_dir = proc_mkdir(EXTDEMOD_PROC_DIR , NULL);

		if (demod_ext_proc_dir == NULL) {
			EXTDEMOD_MGR_WARNING("create rhal_tp dir proc entry (%s) failed\n", demod_ext_proc_dir);
		} else {
			demod_ext_proc_entry = proc_create(EXTDEMOD_PROC_ENTRY, 0666, demod_ext_proc_dir, &extdemod_proc_fops);
			if (demod_ext_proc_entry == NULL) {
				EXTDEMOD_MGR_WARNING("failed to get proc entry for %s/%s \n", EXTDEMOD_PROC_DIR, demod_ext_proc_entry);
			}
		}
	}
#else
	EXTDEMOD_MGR_WARNING("This module requests the kernel to support procfs,need set CONFIG_PROC_FS configure Y\n");
#endif

	return 0;

fail_cdev_init:
	platform_driver_unregister(&ext_demod_device_driver);
fail_platform_driver_register:
	platform_device_unregister(ext_demod_platform_devs);
	ext_demod_platform_devs = NULL;
	class_destroy(ext_demod_class);
fail_class_create:
	ext_demod_class = NULL;
	unregister_chrdev_region(dev, 1);
	return result;
}

int ext_demod_get_debugbuf(unsigned int *buf_addr, unsigned int *buf_size)
{
	int ret = -ENOTTY;
	unsigned int buf_addr_get = 0, buf_size_get = 0, buf_type = 0;

	if (pExtDemod_globe->m_pDemod != NULL) {

#ifdef CONFIG_PROC_FS
		proc_remove(demod_ext_proc_entry);
		demod_ext_proc_entry = NULL;

		proc_remove(demod_ext_proc_dir);
		demod_ext_proc_dir = NULL;
#endif

		pExtDemod_globe->m_pDemod->GetDebugLogBuf(pExtDemod_globe->m_pDemod, &buf_addr_get, &buf_size_get);
		buf_type = (buf_addr_get & 0xC0000000) >> 30;
		buf_addr_get = (buf_addr_get & 0x3FFFFFFF) + DEMOD_MEM_START_PHYS;
		if (((buf_addr_get < DEMOD_MEM_START_PHYS) || (buf_addr_get > DEMOD_MEM_END_PHYS)) && (buf_type == 1)) {
			*buf_addr = 0;
			*buf_size = 0;//uncorrect, so clear
		} else {
			*buf_addr = (unsigned long)phys_to_virt(buf_addr_get);
			*buf_size = buf_size_get;
		}
		ret = 0;
		EXTDEMOD_MGR_INFO("demod debugbuf, type=%d, addr=0x%x, size=0x%x\n", buf_type, *buf_addr, *buf_size);
	}

	return ret;
}

module_init(ext_demod_init);
module_exit(ext_demod_exit);

MODULE_LICENSE("GPL");
