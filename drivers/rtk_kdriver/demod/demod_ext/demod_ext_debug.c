#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/poll.h>
#include <asm/cacheflush.h>
#include <linux/proc_fs.h>

#include "comm.h"
#include "tuner.h"
#include "demod_ext.h"
#include "demod_ext_all.h"
#include "demod_ext_common.h"

#include "ExtDemodMgr.h"
#include "demod_ext_debug.h"

extern PS_EXT_DEMOD_DATA pExtDemod_globe;


SEXT_DEMOD_DEBUG_CMD ExtDemod_debug_cmd[EXTDEMOD_PROC_COUNT + 1]={{"log_onoff=",    EXTDEMOD_PROC_SETLOGONOFF},
														{"check_lock=",   EXTDEMOD_PROC_GETLOCK},
														{"check_offset=", EXTDEMOD_PROC_GETCROFFSET},
														{"ats_onoff=", EXTDEMOD_PROC_ATSONOFF},
	{"NULL",          EXTDEMOD_PROC_COUNT}
};


unsigned char extdemod_log_onoff = 0;
//unsigned char ats_onoff = 0;
EXT_DEMOD_GET_SIGNAL_INFO get_ext_signal_info_data;
TV_SIG_QUAL quality_ext;


ssize_t extdemod_proc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	unsigned char str[128];
	U32BITS i;
	unsigned char *cmd = str;
	unsigned int input_data;

	/* check parameter */
	if (buf == NULL) {
		EXTDEMOD_MGR_WARNING("buf=%p\n", buf);
		return -EFAULT;
	}

	if (count == 0) {
		EXTDEMOD_MGR_WARNING("command should > 0\n");
		return -EINVAL;
	}
	
	if (count > 128) { /* procfs write and read has PAGE_SIZE limit */
		EXTDEMOD_MGR_WARNING("command too large, set length to 128 Byte\n");
		count = 128;
	}

	if (copy_from_user(str, buf, count)) {
		EXTDEMOD_MGR_WARNING("copy_from_user failed! (buf=%p, count="PT_UDEC_DUADDRESS")\n", buf, count);
		return -EFAULT;
	}

	str[count-1] = '\0';
#if 1
	EXTDEMOD_MGR_DBG("proc data: %s\n", str);
#endif

	/* get command string */
	for (i = 0; i < EXTDEMOD_PROC_COUNT; i++) {
		//EXTDEMOD_MGR_DBG("[%d]%s\n", i, ExtDemod_debug_cmd[i].extdemod_cmd_str);
		if (strncmp(str, ExtDemod_debug_cmd[i].extdemod_cmd_str, strlen(ExtDemod_debug_cmd[i].extdemod_cmd_str)) == 0) {
			EXTDEMOD_MGR_DBG("TP debug command: %s\n", ExtDemod_debug_cmd[i].extdemod_cmd_str);
			break;
		}
	}

	/* command out of range check */
	if (i >= EXTDEMOD_PROC_COUNT) {
	    EXTDEMOD_MGR_WARNING("Cannot find your command: \"%s\"\n", str);
	    //return -EINVAL; for coverity

	} else {
	    /* get parameter */
	    cmd += strlen(ExtDemod_debug_cmd[i].extdemod_cmd_str);
		if (sscanf(cmd, "%9u", &input_data) < 1) {
	        EXTDEMOD_MGR_DBG("sscanf get more data failed (%s) - there is no more parameter.\n", cmd);
			//return -EFAULT;
		} else {
			EXTDEMOD_MGR_DBG("input data=%u\n", input_data);
	    }
	}

	switch (i) {
	case EXTDEMOD_PROC_SETLOGONOFF: {
	        extdemod_log_onoff = (unsigned char)input_data;

		if (input_data == 1) {
	            EXTDEMOD_MGR_WARNING("Enable ExtDemod log\n");
		} else {
	            EXTDEMOD_MGR_WARNING("Disable ExtDemod log\n");
	        }
	        break;
	    }
	case EXTDEMOD_PROC_ATSONOFF: {
	        //ats_onoff = (unsigned char)input_data;

		if (input_data == 1) {
	            EXTDEMOD_MGR_WARNING("Enable ExtDemod ATS\n");
		} else {
	            EXTDEMOD_MGR_WARNING("Disable ExtDemod ATS\n");
	        }
	        break;
	    }
	case EXTDEMOD_PROC_GETLOCK: {
			unsigned char lock_status = 0;
			if(pExtDemod_globe->m_pDemod!=NULL)
				pExtDemod_globe->m_pDemod->GetLockStatus(pExtDemod_globe->m_pDemod, &lock_status);
			else
				lock_status=0;
	        EXTDEMOD_MGR_WARNING("Lock=%d\n", lock_status);
			break;
	    }
	case EXTDEMOD_PROC_GETCROFFSET: {
			S32BITS offset_value;
			if(pExtDemod_globe->m_pDemod!=NULL)
				pExtDemod_globe->m_pDemod->GetCarrierOffset(pExtDemod_globe->m_pDemod, &offset_value);
			else
				offset_value = 0;
	        EXTDEMOD_MGR_WARNING("Offset="PT_S32BITS"\n", offset_value);
	        break;
	    }
	default: {
	        EXTDEMOD_MGR_WARNING("unsupport proc command=%s\n", str);
	        return -EINVAL;
	    }
    }

    return count;

}

ssize_t extdemod_proc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	char *str;
	int len;
	
	char *demod_mode_name[] = {
			"TV_MODULATION_UNKNOWN",
			"TV_MODULATION_NONE",
			"TV_MODULATION_VSB",
			"TV_MODULATION_OFDM",
			"TV_MODULATION_PSK",
			"TV_MODULATION_QAM",
			"TV_MODULATION_DVBC_QAM",
			"TV_MODULATION_DTMB_OFDM",
			"TV_MODULATION_DVBT2_OFDM", 	 // T2OFDM
			"TV_MODULATION_DVBS",
			"TV_MODULATION_DVBS2",
			"TV_MODULATION_ISDBT",			// ISDB-T
			"TV_MODULATION_ISDBS",				  // ISDB-S
			"TV_MODULATION_ABSS",			// ABS-S
			"TV_MODULATION_RTSP",
			"TV_MODULATION_ATV",
		};

	if (*ppos != 0) {
		EXTDEMOD_MGR_WARNING("*ppos!=0 just exit\n");
		return 0;
	}
	EXTDEMOD_MGR_WARNING("demod_proc_read Enter *ppos=0x%llx count=0x"PT_HEX_DUADDRESS"\n", *ppos, count);

	len = 0;
	str = kmalloc(EXTDBG_MSG_BUFFER_LENGTH, GFP_KERNEL);
	if(str == NULL){
		EXTDEMOD_MGR_WARNING("demod_proc_read kmalloc failed\n");
		return 0;
	}

	memset(str, 0, EXTDBG_MSG_BUFFER_LENGTH);

	if (pExtDemod_globe->m_pDemod != NULL) {
	{
		unsigned char lock_status;
		pExtDemod_globe->m_pDemod->GetLockStatus(pExtDemod_globe->m_pDemod, &lock_status);
			len += snprintf(str, EXTDBG_MSG_BUFFER_LENGTH-len, "LOCK     = %u\n", lock_status);
	}
	{
		pExtDemod_globe->m_pDemod->GetSignalQuality(pExtDemod_globe->m_pDemod, TV_QUAL_SNR, (TV_SIG_QUAL*)&quality_ext);
			len += snprintf(str+len, EXTDBG_MSG_BUFFER_LENGTH-len, "SNR      = "PT_U32BITS"."PT_U32BITS"\n", quality_ext.snr / 10000, quality_ext.snr % 10000);
	}
	{
		pExtDemod_globe->m_pDemod->GetSignalQuality(pExtDemod_globe->m_pDemod, TV_QUAL_AGC, (TV_SIG_QUAL*)&quality_ext);
			len += snprintf(str+len, EXTDBG_MSG_BUFFER_LENGTH-len, "AGC      = "PT_U32BITS"\n", quality_ext.agc);
	}
	{
		pExtDemod_globe->m_pDemod->GetSignalQuality(pExtDemod_globe->m_pDemod, TV_QUAL_SIGNAL_STRENGTH, (TV_SIG_QUAL*)&quality_ext);
			len += snprintf(str+len, EXTDBG_MSG_BUFFER_LENGTH-len, "STRENGTH = %u\n", quality_ext.strength);
	}
	{
		pExtDemod_globe->m_pDemod->GetSignalQuality(pExtDemod_globe->m_pDemod, TV_QUAL_SIGNAL_QUALITY, (TV_SIG_QUAL*)&quality_ext);
			len += snprintf(str+len, EXTDBG_MSG_BUFFER_LENGTH-len, "QUALITY  = %u\n", quality_ext.quality);
	}
	{
		pExtDemod_globe->m_pDemod->GetSignalQuality(pExtDemod_globe->m_pDemod, TV_QUAL_PEC, (TV_SIG_QUAL*)&quality_ext);
			len += snprintf(str+len, EXTDBG_MSG_BUFFER_LENGTH-len, "PER_num  = "PT_U32BITS"  PER_den = "PT_U32BITS"", quality_ext.layer_per.per_num, quality_ext.layer_per.per_den);
		pExtDemod_globe->m_pDemod->GetSignalQuality(pExtDemod_globe->m_pDemod, TV_QUAL_PER, (TV_SIG_QUAL*)&quality_ext);
			len += snprintf(str+len, EXTDBG_MSG_BUFFER_LENGTH-len, " PER = "PT_U32BITS"\n", quality_ext.per);
	}
	{
			REALTEK_ALL* pRealtekAll;
			pRealtekAll = (REALTEK_ALL*)(pExtDemod_globe->m_pDemod->private_data);
			if(pRealtekAll->m_output_mode == RTK_DEMOD_OUT_IF_PARALLEL)
				len += snprintf(str+len, EXTDBG_MSG_BUFFER_LENGTH-len, "m_output_mode = PARALLEL\n");
		else
				len += snprintf(str+len, EXTDBG_MSG_BUFFER_LENGTH-len, "m_output_mode = SERIAL\n");
	}
	{
		pExtDemod_globe->m_pDemod->GetSignalInfo(pExtDemod_globe->m_pDemod, (TV_SIG_INFO*)&get_ext_signal_info_data);
			//len = sprintf(str, "%smod = %s if_freq=%u if_inversion=%u agc_pola=%u\n", str, demod_mode_name[get_ext_signal_info_data.mod], get_ext_signal_info_data.demod_param.if_freq, get_ext_signal_info_data.demod_param.if_inversion, get_ext_signal_info_data.demod_param.agc_pola);
			len += snprintf(str+len, EXTDBG_MSG_BUFFER_LENGTH-len, "mod = %s\n", demod_mode_name[get_ext_signal_info_data.mod]);
		}
	} else {
		len += snprintf(str+len, EXTDBG_MSG_BUFFER_LENGTH-len, "External demod is not running!!\n");
	}

	//len = strlen(str);

	if (count > len) {
		if (copy_to_user(buf, str, len)) {
			EXTDEMOD_MGR_WARNING("Copy proc data to user space failed\n");
			kfree(str);
			return 0;
		}

		if (*ppos == 0)
			*ppos += len;
		else
			len = 0;
	} else {
		len=0;
	}

	kfree(str);
	EXTDEMOD_MGR_WARNING("demod_proc_read Exit *ppos=0x%llx count=0x"PT_HEX_DUADDRESS" len=0x%x\n", *ppos, count, len);

	return len;
}

