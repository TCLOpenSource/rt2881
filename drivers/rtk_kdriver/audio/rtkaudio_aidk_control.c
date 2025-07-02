#include <linux/compat.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/fs.h>

#include <rtk_kdriver/rtkaudio.h>
#include <rtk_kdriver/RPCDriver.h>
#include <rtk_kdriver/rmm/rmm.h>
#include <linux/pageremap.h>
#include <rtd_log/rtd_module_log.h>

#include "AudioRPCBaseDS_data.h"
#include "AudioInbandAPI.h"
#include "audio_rpc.h"
#include "audio_base.h"
#include "audio_inc.h"

rtkaudio_aidk_information g_aidk_info;

static int rtkaudio_isvalid_lang(char lang[4])
{
	if ((lang[0] < 0x61) || (lang[0] > 0x7a)) {
		rtd_pr_adsp_info("[rtkaudio] unknown AC4 lang[0] %c, ignore\n", lang[0]);
		return FALSE;
	}

	if ((lang[1] < 0x61) || (lang[1] > 0x7a)) {
		rtd_pr_adsp_info("[rtkaudio] unknown AC4 lang[1] %c, ignore\n", lang[1]);
		return FALSE;
	}

	if ((lang[2] != 0) && ((lang[2] < 0x61) || (lang[2] > 0x7a))) {
		rtd_pr_adsp_info("[rtkaudio] unknown AC4 lang[2] %c, ignore\n", lang[2]);
		return FALSE;
	}

	if (lang[3] != 0) {
		rtd_pr_adsp_info("[rtkaudio] unknown AC4 lang[3] %c, ignore\n", lang[3]);
		return FALSE;
	}

	return TRUE;
}

static int rtkaudio_get_audio_aidk_caps(void)
{
	int caps = 0;

	unsigned long ret;
	AUDIO_RPC_PRIVATEINFO_PARAMETERS *cmd, *cmd_audio;
	AUDIO_RPC_PRIVATEINFO_RETURNVAL *res, *res_audio;

	void *vir_addr = NULL;
	void *unvir_addr = NULL;
	unsigned long phy_addr = 0;

	//rtd_pr_adsp_err("[audio] rtkaudio_get_audio_aidk_caps %d\n", __LINE__);

	vir_addr = get_rpc_mem(&unvir_addr);
	if (!vir_addr) {
		rtd_pr_adsp_err("[audio] %d alloc memory fail\n", __LINE__);
		return -ENOMEM;
	}
	phy_addr = (unsigned long) dvr_to_phys(vir_addr);

	/* set parameter to 0, for snapshot */
	memset(unvir_addr, 0, RPC_DVR_MALLOC_UNCACHED_SIZE);

	cmd = unvir_addr;
	res = (void *)(AUDIO_RPC_PRIVATEINFO_RETURNVAL *)((((unsigned long)unvir_addr + sizeof(AUDIO_RPC_PRIVATEINFO_PARAMETERS) + 8) & ALIGN4));

	cmd_audio = (void *)(AUDIO_RPC_PRIVATEINFO_PARAMETERS *) ((ulong) phy_addr);
	res_audio = (void *)(AUDIO_RPC_PRIVATEINFO_RETURNVAL *) (((((ulong) phy_addr) + sizeof(AUDIO_RPC_PRIVATEINFO_PARAMETERS) + 8) & ALIGN4));

	memset(cmd, 0, sizeof(AUDIO_RPC_PRIVATEINFO_PARAMETERS));
	cmd->type = htonl(ENUM_PRIVATEINFO_AUDIO_FIRMWARE_CAPABILITY);

	if (send_rpc_command(RPC_AUDIO,
		ENUM_KERNEL_RPC_PRIVATEINFO,
		(unsigned long) cmd_audio,
		(unsigned long) res_audio, &ret)) {
		rtd_pr_adsp_err("[audio] %s %d RPC fail\n", __func__, __LINE__);
		put_rpc_mem(vir_addr);
		return -1;
	}

	if (ret == S_OK) {
		caps = res->privateInfo[1];
		/* rtd_pr_adsp_info("[audio] get ADSP caps %x\n", caps); */
	}

	put_rpc_mem(vir_addr);

	return caps;
}

void rtkaudio_aidk_init(void)
{
	memset(&g_aidk_info, 0, sizeof(rtkaudio_aidk_information));

	g_aidk_info.ch_mode = AIDK_AUTO;
	g_aidk_info.atmos_lock = 0;

	g_aidk_info.stout_drc.boost = 100;
	g_aidk_info.stout_drc.cut = 100;
	g_aidk_info.stout_drc.drc_mode = AIDK_DRC_RF;
	g_aidk_info.stout_drc.dmx_mode = AIDK_DOWNMIX_LORO;

	g_aidk_info.multiout_drc.boost = 100;
	g_aidk_info.multiout_drc.cut = 100;
	g_aidk_info.multiout_drc.drc_mode = AIDK_DRC_RF;

	g_aidk_info.aac_prl = 127;
	g_aidk_info.ac4_de  = 0;
	g_aidk_info.ac4_mix_level.level = 0;
	g_aidk_info.dap_de  = 0;
	g_aidk_info.ac4_presentation_id = -1;
	g_aidk_info.ac4_short_program_id = -1;

	g_aidk_info.spk_virt_mode = VIRTUAL_DISABLE;
	g_aidk_info.hp_virt_mode = VIRTUAL_DISABLE;

	g_aidk_info.vl_mode = VL_DISABLE;
	g_aidk_info.vl_amount = 4;

	g_aidk_info.conti_out = true;
	g_aidk_info.dump_on = false;
	g_aidk_info.dump_mode = AIDK_DUMP_WRITE;

	g_aidk_info.ac4_pref.pres_mode = AIDK_AC4_PRES_LANGUAGE;
	g_aidk_info.ac4_pref.pres_assoc_type = AIDK_PRES_ASSOC_NONE;
	memcpy(g_aidk_info.ac4_pref.first_lang, "eng", 3);
	memcpy(g_aidk_info.ac4_pref.second_lang, "eng", 3);

	g_aidk_info.spk_virt_angle = 0;
	g_aidk_info.spk_virt_freq = 0;
	g_aidk_info.dap_virt_boost = 96;

	g_aidk_info.ad_on = 0;
	g_aidk_info.atmos_encoder_on = 1;
	g_aidk_info.bs_passthrough = 0;

	g_aidk_info.highest_caps = -1;
	g_aidk_info.is_atmos = -1;

       g_aidk_info.flex_mode = 0;
       g_aidk_info.b_enforce_single_oa_element = 0;
       g_aidk_info.center_extract_mode = 0;

	rtd_pr_adsp_info("aidk init value done\n");
}

long aidk_ioctl_set(aidk_cmd_info aidk_info)
{
	AUDIO_CONFIG_COMMAND_RTKAUDIO cmd_info;
	long ret = 0;

	AUDIO_RPC_PRIVATEINFO_PARAMETERS parameter;
	AUDIO_RPC_PRIVATEINFO_RETURNVAL res;
	void *vir_addr = NULL;
	unsigned int rpc_res;

	switch(aidk_info.type)
	{
		case AIDK_CHANNEL_MODE:
		{
			AIDK_OUTPUT_CHANNEL_MODE ch_mode = AIDK_AUTO;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set output channel mode\n");

			if (copy_from_user(&ch_mode, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (g_aidk_info.ch_mode != ch_mode) {
				rtd_pr_adsp_info("[rtkaudio] output channel mode changed (%d > %d)\n", g_aidk_info.ch_mode, ch_mode);
				g_aidk_info.ch_mode = ch_mode;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_CHANNEL_MODE;
				cmd_info.value[0] = g_aidk_info.ch_mode;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}

			ret = 0;
			break;
		}
		case AIDK_ATMOS_LOCK:
		{
			int atmos_lock = 0;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set atmos lock %d\n", atmos_lock);

			if (copy_from_user(&atmos_lock, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (g_aidk_info.atmos_lock != atmos_lock) {
				rtd_pr_adsp_info("[rtkaudio] atmos lock changed (%d > %d)\n", g_aidk_info.atmos_lock, atmos_lock);
				g_aidk_info.atmos_lock = atmos_lock;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_SET_DOLBY_OTT_MODE;
				cmd_info.value[0] = atmos_lock;
				cmd_info.value[1] = atmos_lock;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}

			ret = 0;
			break;
		}
		case AIDK_STEREO_OUT_DRC:
		{
			aidk_stereo_drc stout_drc;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set stereo drc parameter\n");

			if (copy_from_user(&stout_drc, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] stereo drc boost changed (%d > %d)\n", g_aidk_info.stout_drc.boost, stout_drc.boost);
			g_aidk_info.stout_drc.boost = stout_drc.boost;

			rtd_pr_adsp_info("[rtkaudio] stereo drc cut changed (%d > %d)\n", g_aidk_info.stout_drc.cut, stout_drc.cut);
			g_aidk_info.stout_drc.cut = stout_drc.cut;

			/* update boost and cut here */
			cmd_info.msg_id = AUDIO_CONFIG_CMD_DD_SCALE_2CH;

			cmd_info.value[0] = 0x7FFFFFFF / 100 * g_aidk_info.stout_drc.cut;
			cmd_info.value[1] = 0x7FFFFFFF / 100 * g_aidk_info.stout_drc.boost;

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] stereo drc_mode changed (%d > %d)\n", g_aidk_info.stout_drc.drc_mode, stout_drc.drc_mode);
			g_aidk_info.stout_drc.drc_mode = stout_drc.drc_mode;

			cmd_info.msg_id = AUDIO_CONFIG_CMD_DD_COMP_2CH;

			if (stout_drc.drc_mode == AIDK_DRC_LINE) {
				cmd_info.value[0] = COMP_LINEOUT;
			} else {
				cmd_info.value[0] = COMP_RF;
			}

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] stereo drc dmx_mode changed (%d > %d)\n", g_aidk_info.stout_drc.dmx_mode, stout_drc.dmx_mode);
			g_aidk_info.stout_drc.dmx_mode = stout_drc.dmx_mode;

			cmd_info.msg_id = AUDIO_CONFIG_CMD_DD_DOWNMIXMODE;
			cmd_info.value[1] = 0;
			cmd_info.value[2] = 0x00002379;

			if (stout_drc.dmx_mode == AIDK_DOWNMIX_LORO) {
				cmd_info.value[0] = MODE_STEREO;
			} else if (stout_drc.dmx_mode == AIDK_DOWNMIX_ARIB) {
				cmd_info.value[0] = MODE_ARIB;
			} else {
				cmd_info.value[0] = MODE_DOLBY_SURROUND;
			}

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			ret = 0;
			break;
		}
		case AIDK_MULTI_OUT_DRC:
		{
			aidk_multiout_drc multiout_drc;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set stereo drc parameter\n");

			if (copy_from_user(&multiout_drc, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] multich drc boost changed (%d > %d)\n", g_aidk_info.multiout_drc.boost, multiout_drc.boost);
			g_aidk_info.multiout_drc.boost = multiout_drc.boost;


			rtd_pr_adsp_info("[rtkaudio] multich drc cut changed (%d > %d)\n", g_aidk_info.multiout_drc.cut, multiout_drc.cut);
			g_aidk_info.multiout_drc.cut = multiout_drc.cut;


			/* update boost and cut here */
			cmd_info.msg_id = AUDIO_CONFIG_CMD_DD_SCALE;

			cmd_info.value[0] = 0x7FFFFFFF / 100 * g_aidk_info.multiout_drc.cut;
			cmd_info.value[1] = 0x7FFFFFFF / 100 * g_aidk_info.multiout_drc.boost;

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] multich drc_mode changed (%d > %d)\n", g_aidk_info.multiout_drc.drc_mode, multiout_drc.drc_mode);
			g_aidk_info.multiout_drc.drc_mode = multiout_drc.drc_mode;

			cmd_info.msg_id = AUDIO_CONFIG_CMD_DD_COMP;

			if (multiout_drc.drc_mode == AIDK_DRC_LINE) {
				cmd_info.value[0] = COMP_LINEOUT;
			} else {
				cmd_info.value[0] = COMP_RF;
			}

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			ret = 0;
			break;
		}
	 	case AIDK_AAC_PRL:
		{
			unsigned int aac_prl;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set aac prl\n");

			if (copy_from_user(&aac_prl, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (aac_prl != g_aidk_info.aac_prl) {
				rtd_pr_adsp_info("[rtkaudio] aac prl changed (%d > %d)\n", g_aidk_info.aac_prl, aac_prl);
				g_aidk_info.aac_prl = aac_prl;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_AAC_PRL;
				cmd_info.value[0] = g_aidk_info.aac_prl;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}

			ret = 0;
			break;
		}
		case AIDK_AC4_DE:
		{
			unsigned int ac4_de;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set ac4 dialog enhancemeant\n");

			if (copy_from_user(&ac4_de, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] ac4 de changed (%d > %d)\n", g_aidk_info.ac4_de, ac4_de);
			g_aidk_info.ac4_de = ac4_de;

			cmd_info.msg_id = AUDIO_CONFIG_CMD_AC4_DIALOGUE_ENHANCEMENT_GAIN;
			cmd_info.value[0] = g_aidk_info.ac4_de;

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			ret = 0;
			break;
		}
		case AIDK_AC4_PRESENTATION_ID:
		{
			unsigned int ac4_presentation_id;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set ac4 presentation ID\n");

			if (copy_from_user(&ac4_presentation_id, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] ac4 presentation ID (%d > %d)\n", g_aidk_info.ac4_presentation_id, ac4_presentation_id);
			g_aidk_info.ac4_presentation_id = ac4_presentation_id;

			cmd_info.msg_id = AUDIO_CONFIG_CMD_AC4_PRESENTATION_GROUP_INDEX;
			cmd_info.value[0] = g_aidk_info.ac4_presentation_id;

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			ret = 0;
			break;
		}
		case AIDK_AC4_SHORT_PROGRAM_ID:
		{
			unsigned int ac4_short_program_id;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set ac4 short program ID\n");

			if (copy_from_user(&ac4_short_program_id, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] ac4 short program ID (%d > %d)\n", g_aidk_info.ac4_short_program_id, ac4_short_program_id);
			g_aidk_info.ac4_short_program_id = ac4_short_program_id;

			cmd_info.msg_id = AUDIO_CONFIG_CMD_AC4_SHORT_PROGRAM_INDEX;
			cmd_info.value[0] = g_aidk_info.ac4_short_program_id;

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			ret = 0;
			break;
		}
		case AIDK_DAP_DE:
		{
			unsigned int dap_de;
			void *unvir_addr = NULL;
			DAP_PARAM_DIALOGUE_ENHANCER *v_dap = NULL;
			rtd_pr_adsp_info("[rtkaudio] set dap dialog enhancemeant\n");

			if (copy_from_user(&dap_de, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] dap de changed (%d > %d)\n", g_aidk_info.dap_de, dap_de);
			g_aidk_info.dap_de = dap_de;

			vir_addr = get_rpc_mem(&unvir_addr);

			if (vir_addr == NULL) {
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO malloc failed\n", __FUNCTION__, __LINE__);
				return ret;
			}

			v_dap = (DAP_PARAM_DIALOGUE_ENHANCER *) unvir_addr;
			v_dap->de_amount = g_aidk_info.dap_de;

			parameter.type = ENUM_PRIVATEINFO_AUDIO_SET_DAP_DIALOGUE_ENHANCER;
			parameter.privateInfo[0] = dvr_to_phys(vir_addr);
			parameter.instanceID =0;//ENUM_PRIVATEINFO_AUDIO_SET_DAP_DIALOGUE_ENHANCER no care

			rpc_res = RTKAUDIO_RPC_TOAGENT_PRIVATEINFO_SVC(&parameter, &res);
			if (rpc_res != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				put_rpc_mem(vir_addr);
				return ret;
			}

			put_rpc_mem(vir_addr);

			ret = 0;
			break;
		}
		case AIDK_VIRTUALIZER_MODE:
		{
			aidk_virt_mode virt_mode;
			void *unvir_addr = NULL;
			DAP_PARAM_SURROUND_VIRTUALIZER *v_dap = NULL;
			rtd_pr_adsp_info("[rtkaudio] set virtualizer mode\n");

			if (copy_from_user(&virt_mode, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (virt_mode.dev == AIDK_SPEAKER) {
				rtd_pr_adsp_info("[rtkaudio] speaker virtual mode changed (%d > %d)\n", g_aidk_info.spk_virt_mode, virt_mode.mode);
				g_aidk_info.spk_virt_mode = virt_mode.mode;

				vir_addr = get_rpc_mem(&unvir_addr);

				if (vir_addr == NULL) {
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO malloc failed\n", __FUNCTION__, __LINE__);
					return ret;
				}

				v_dap = (DAP_PARAM_SURROUND_VIRTUALIZER *) unvir_addr;
				v_dap->virtualizer_mode = g_aidk_info.spk_virt_mode;
				v_dap->surround_boost = g_aidk_info.dap_virt_boost;

				parameter.type = ENUM_PRIVATEINFO_AUDIO_SET_DAP_SURROUND_VIRTUALIZER;
				parameter.privateInfo[0] = dvr_to_phys(vir_addr);
				parameter.instanceID =0;// no care

				rpc_res = RTKAUDIO_RPC_TOAGENT_PRIVATEINFO_SVC(&parameter, &res);
				if (rpc_res != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					put_rpc_mem(vir_addr);
					return ret;
				}

				put_rpc_mem(vir_addr);
			} else if (virt_mode.dev == AIDK_HEADPHONE) {
				rtd_pr_adsp_info("[rtkaudio] headphone virtual mode changed (%d > %d)\n", g_aidk_info.hp_virt_mode, virt_mode.mode);
				g_aidk_info.hp_virt_mode = virt_mode.mode;

				/* didn't support headphone virtualizer */
			} else {
				rtd_pr_adsp_info("[rtkaudio] unknown device for virtual mode (%d, %d)\n", virt_mode.dev, virt_mode.mode);
			}

			ret = 0;
			break;
		}
		case AIDK_VOLUME_LEVELER_MODE:
		{
			AIDK_VL_MODE vl_mode;
			void *unvir_addr = NULL;
			DAP_PARAM_VOLUME_LEVELER *v_dap = NULL;

			rtd_pr_adsp_info("[rtkaudio] set volume leveler mode\n");

			if (copy_from_user(&vl_mode, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] volume leveler mode (%d > %d)\n", g_aidk_info.vl_mode, vl_mode);
			g_aidk_info.vl_mode = vl_mode;

			vir_addr = get_rpc_mem(&unvir_addr);

			if (vir_addr == NULL) {
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO malloc failed\n", __FUNCTION__, __LINE__);
				return ret;
			}

			v_dap = (DAP_PARAM_VOLUME_LEVELER *) unvir_addr;
			v_dap->leveler_setting = g_aidk_info.vl_mode;
			v_dap->leveler_amount  = g_aidk_info.vl_amount;

			parameter.type = ENUM_PRIVATEINFO_AUDIO_SET_DAP_VOLUME_LEVELER;
			parameter.privateInfo[0] = dvr_to_phys(vir_addr);
			parameter.instanceID =0;//SET_DAP_VOLUME_LEVELER no care

			rpc_res = RTKAUDIO_RPC_TOAGENT_PRIVATEINFO_SVC(&parameter, &res);
			if (rpc_res != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				put_rpc_mem(vir_addr);
				return ret;
			}

			put_rpc_mem(vir_addr);

			ret = 0;
			break;
		}
		case AIDK_VOLUME_LEVELER_AMOUNT:
		{
			unsigned int vl_amount;
			void *unvir_addr = NULL;
			DAP_PARAM_VOLUME_LEVELER *v_dap = NULL;
			rtd_pr_adsp_info("[rtkaudio] set volume leveler amount\n");

			if (copy_from_user(&vl_amount, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] volume leveler amount (%d > %d)\n", g_aidk_info.vl_amount, vl_amount);
			g_aidk_info.vl_amount = vl_amount;

			vir_addr = get_rpc_mem(&unvir_addr);

			if (vir_addr == NULL) {
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO malloc failed\n", __FUNCTION__, __LINE__);
				return ret;
			}

			v_dap = (DAP_PARAM_VOLUME_LEVELER *) unvir_addr;
			v_dap->leveler_setting = g_aidk_info.vl_mode;
			v_dap->leveler_amount  = g_aidk_info.vl_amount;

			parameter.type = ENUM_PRIVATEINFO_AUDIO_SET_DAP_VOLUME_LEVELER;
			parameter.privateInfo[0] = dvr_to_phys(vir_addr);
			parameter.instanceID =0;// no care

			rpc_res = RTKAUDIO_RPC_TOAGENT_PRIVATEINFO_SVC(&parameter, &res);
			if (rpc_res != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				put_rpc_mem(vir_addr);
				return ret;
			}

			put_rpc_mem(vir_addr);

			ret = 0;
			break;
		}
		case AIDK_CONTINUOUS_OUT:
		{
			unsigned int conti_out;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set continue output\n");

			if (copy_from_user(&conti_out, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (conti_out != g_aidk_info.conti_out) {
				rtd_pr_adsp_info("[rtkaudio] continue output changed (%d > %d)\n", g_aidk_info.conti_out, conti_out);
				g_aidk_info.conti_out = conti_out;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_CONTINUOUS_OUT;
				cmd_info.value[0] = g_aidk_info.conti_out;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}

			ret = 0;
			break;
		}
		case AIDK_AUDIO_DUMP:
		{
			unsigned int dump_on;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set Audio Dump onoff\n");

			if (copy_from_user(&dump_on, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (dump_on != g_aidk_info.dump_on) {
				char cmd_buf[100] = {0};
				int size = 0;

				rtd_pr_adsp_info("[rtkaudio] Audio Dump onoff changed (%d > %d)\n", g_aidk_info.dump_on, dump_on);
				g_aidk_info.dump_on = dump_on;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_AIDK_DUMP;
				cmd_info.value[0] = g_aidk_info.dump_on;

				if (g_aidk_info.dump_on) {
					ret = rtkaudio_send_audio_config(&cmd_info);
					if (ret != S_OK) {
						ret = -EFAULT;
						rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
						return ret;
					}

					size = 19;
					snprintf(cmd_buf, size, "dump_enable 163840");
					rtkaudio_send_string(&cmd_buf[0], size);
				} else {
					size = 20;
					snprintf(cmd_buf, size, "dump_disable 163840");
					rtkaudio_send_string(&cmd_buf[0], size);

					ret = rtkaudio_send_audio_config(&cmd_info);
					if (ret != S_OK) {
						ret = -EFAULT;
						rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
						return ret;
					}
				}
			}

			ret = 0;
			break;
		}
		case AIDK_AUDIO_DUMP_MODE:
		{
			AIDK_DUMP_MODE dump_mode;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set Audio Dump mode\n");

			if (copy_from_user(&dump_mode, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (dump_mode != g_aidk_info.dump_mode) {
				rtd_pr_adsp_info("[rtkaudio] Audio Dump mode changed (%d > %d)\n", g_aidk_info.dump_mode, dump_mode);
				g_aidk_info.dump_mode = dump_mode;
			}

			ret = 0;
			break;
		}
		case AIDK_AC4_PRES_PREF:
		{
			aidk_ac4_pref ac4_pref;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set AC4 presentation preference\n");

			if (copy_from_user(&ac4_pref, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (ac4_pref.pres_mode != g_aidk_info.ac4_pref.pres_mode) {
				rtd_pr_adsp_info("[rtkaudio] AC4 presentation mode changed (%d > %d)\n", g_aidk_info.ac4_pref.pres_mode, ac4_pref.pres_mode);
				g_aidk_info.ac4_pref.pres_mode = ac4_pref.pres_mode;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_AC4_PRIORITIZE_ADTYPE;
				cmd_info.value[0] = g_aidk_info.ac4_pref.pres_mode;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}

			if (ac4_pref.pres_assoc_type != g_aidk_info.ac4_pref.pres_assoc_type) {
				rtd_pr_adsp_info("[rtkaudio] AC4 presentation associate type changed (%d > %d)\n", g_aidk_info.ac4_pref.pres_assoc_type, ac4_pref.pres_assoc_type);
				g_aidk_info.ac4_pref.pres_assoc_type = ac4_pref.pres_assoc_type;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_AC4_ADTYPE;
				cmd_info.value[0] = g_aidk_info.ac4_pref.pres_assoc_type;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}

			if (rtkaudio_isvalid_lang(ac4_pref.first_lang)) {
				unsigned int *lang;
				int idx = 0;

				rtd_pr_adsp_info("[rtkaudio] set AC4 fisrt lang %s\n", ac4_pref.first_lang);
				for (idx = 0; idx < 4; idx++) {
					g_aidk_info.ac4_pref.first_lang[idx] = ac4_pref.first_lang[3 - idx];
				}

				lang = (unsigned int *) &g_aidk_info.ac4_pref.first_lang[0];
				cmd_info.msg_id = AUDIO_CONFIG_CMD_AC4_FIRST_LANGUAGE;
				cmd_info.value[0] = *lang;
				if (ac4_pref.first_lang[2] != 0) {
					cmd_info.value[1] = 1;
				} else {
					cmd_info.value[1] = 0;
				}

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			} else {
				rtd_pr_adsp_info("[rtkaudio] unknown AC4 first lang %s, ignore\n", ac4_pref.first_lang);
			}

			if (rtkaudio_isvalid_lang(ac4_pref.second_lang)) {
				unsigned int *lang;
				int idx = 0;

				rtd_pr_adsp_info("[rtkaudio] set AC4 second lang %s\n", ac4_pref.second_lang);
				for (idx = 0; idx < 4; idx++) {
					g_aidk_info.ac4_pref.second_lang[idx] = ac4_pref.second_lang[3 - idx];
				}

				lang = (unsigned int *) &g_aidk_info.ac4_pref.second_lang[0];
				cmd_info.msg_id = AUDIO_CONFIG_CMD_AC4_SECOND_LANGUAGE;
				cmd_info.value[0] = *lang;
				if (ac4_pref.second_lang[2] != 0) {
					cmd_info.value[1] = 1;
				} else {
					cmd_info.value[1] = 0;
				}

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			} else {
				rtd_pr_adsp_info("[rtkaudio] unknown AC4 second lang %s, ignore\n", ac4_pref.second_lang);
			}

			ret = 0;
			break;
		}
		case AIDK_DAP_SPK_VIRT_ANGLE:
		{
			/* It is MS12 v2.3 API, ignore */
			ret = -1;
			break;
		}
		case AIDK_DAP_SPK_VIRT_FREQ:
		{
			/* It is MS12 v2.3 API, ignore */
			ret = -1;
			break;
		}
		case AIDK_DAP_VIRT_SURR_BOOST:
		{
			unsigned int dap_virt_boost;
			void *unvir_addr = NULL;
			DAP_PARAM_SURROUND_VIRTUALIZER *v_dap = NULL;
			rtd_pr_adsp_info("[rtkaudio] set DAP virtualizer boost\n");

			if (copy_from_user(&dap_virt_boost, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] DAP virtualizer boost changed (%d > %d)\n", g_aidk_info.dap_virt_boost, dap_virt_boost);
			g_aidk_info.dap_virt_boost = dap_virt_boost;

			vir_addr = get_rpc_mem(&unvir_addr);

			if (vir_addr == NULL) {
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO malloc failed\n", __FUNCTION__, __LINE__);
				return ret;
			}

			v_dap = (DAP_PARAM_SURROUND_VIRTUALIZER *) unvir_addr;
			v_dap->virtualizer_mode = g_aidk_info.spk_virt_mode;
			v_dap->surround_boost = g_aidk_info.dap_virt_boost;

			parameter.type = ENUM_PRIVATEINFO_AUDIO_SET_DAP_SURROUND_VIRTUALIZER;
			parameter.privateInfo[0] = dvr_to_phys(vir_addr);
			parameter.instanceID =0;// no care

			rpc_res = RTKAUDIO_RPC_TOAGENT_PRIVATEINFO_SVC(&parameter, &res);
			if (rpc_res != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				put_rpc_mem(vir_addr);
				return ret;
			}

			put_rpc_mem(vir_addr);

			ret = 0;
			break;
		}
		case AIDK_AD_ON:
		{
			unsigned int ad_on;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set Audio Description onoff\n");

			if (copy_from_user(&ad_on, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (ad_on != g_aidk_info.ad_on) {
				rtd_pr_adsp_info("[rtkaudio] Audio Description onoff changed (%d > %d)\n", g_aidk_info.ad_on, ad_on);
				g_aidk_info.ad_on = ad_on;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_AD_MIXING;
				cmd_info.value[0] = g_aidk_info.ad_on;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}

			ret = 0;
			break;
		}
		case AIDK_ATMOS_ON:
		{
			unsigned int atmos_encoder_on;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set ATMOS encoder onoff\n");

			if (copy_from_user(&atmos_encoder_on, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (atmos_encoder_on != g_aidk_info.atmos_encoder_on) {
				rtd_pr_adsp_info("[rtkaudio] ATMOS encoder onoff changed (%d > %d)\n", g_aidk_info.atmos_encoder_on, atmos_encoder_on);
				g_aidk_info.atmos_encoder_on = atmos_encoder_on;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_EAC3_ATMOS_ENCODE_ONOFF;
				cmd_info.value[0] = g_aidk_info.atmos_encoder_on;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}

			ret = 0;
			break;
		}
		case AIDK_DAP_SET_TUNING_FILE:
		{
			void *vir_addr, *tuning_file;
			int file_size;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set DAP tuning file\n");

			file_size = PAGE_ALIGN(aidk_info.size);
			vir_addr = dvr_malloc_uncached_specific(file_size, GFP_DCU1, &tuning_file);
			memset(tuning_file, 0, file_size);

			if (!vir_addr) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO cma malloc error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (copy_from_user(tuning_file, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				dvr_free(vir_addr);
				return ret;
			}

			cmd_info.msg_id = AUDIO_CONFIG_CMD_DAP_TUNINGDATA;
			cmd_info.value[0] = dvr_to_phys(vir_addr);
			cmd_info.value[1] = aidk_info.size;

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				dvr_free(vir_addr);
				return ret;
			}

			dvr_free(vir_addr);
			ret = 0;
			break;
		}
		case AIDK_FLAP_SET_SCONF_FILE:
		{
			void *vir_addr, *sconf_file;
			int file_size;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set FLAP sconf file\n");

			file_size = PAGE_ALIGN(aidk_info.size);
			vir_addr = dvr_malloc_uncached_specific(file_size, GFP_DCU1, &sconf_file);
			memset(sconf_file, 0, file_size);

			if (!vir_addr) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO cma malloc error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (copy_from_user(sconf_file, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				dvr_free(vir_addr);
				return ret;
			}

			cmd_info.msg_id = AUDIO_CONFIG_CMD_SET_FLEXR_CONFIG;
			cmd_info.value[0] = dvr_to_phys(vir_addr);
			cmd_info.value[1] = aidk_info.size;
                        cmd_info.value[2] = 0;

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				dvr_free(vir_addr);
				return ret;
			}

			dvr_free(vir_addr);
			ret = 0;
			break;
		}
		case AIDK_FLAP_SET_DCONF_FILE:
		{
			void *vir_addr, *dconf_file;
			int file_size;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set FLAP sconf file\n");

			file_size = PAGE_ALIGN(aidk_info.size);
			vir_addr = dvr_malloc_uncached_specific(file_size, GFP_DCU1, &dconf_file);
			memset(dconf_file, 0, file_size);

			if (!vir_addr) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO cma malloc error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (copy_from_user(dconf_file, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				dvr_free(vir_addr);
				return ret;
			}

			cmd_info.msg_id = AUDIO_CONFIG_CMD_SET_FLEXR_CONFIG;
			cmd_info.value[0] = dvr_to_phys(vir_addr);
			cmd_info.value[1] = aidk_info.size;
                        cmd_info.value[2] = 1;

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				dvr_free(vir_addr);
				return ret;
			}

			dvr_free(vir_addr);
			ret = 0;
			break;
		}
		case AIDK_AC4_MIX_LEVEL:
		{
			aidk_ac4_mixlevel ac4_mix_level;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set AC4 mix level\n");

			if (copy_from_user(&ac4_mix_level, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (ac4_mix_level.level != g_aidk_info.ac4_mix_level.level) {
				rtd_pr_adsp_info("[rtkaudio] AC4 mix level changed (%d > %d)\n", g_aidk_info.ac4_mix_level.level, ac4_mix_level.level);
				g_aidk_info.ac4_mix_level.level = ac4_mix_level.level;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_AC4_USER_BALANCE_ADJUSTMENT;
				cmd_info.value[0] = g_aidk_info.ac4_mix_level.level;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}

			ret = 0;
			break;
		}
		case AIDK_SET_BS_PASSTHROUGH:
		{
			unsigned int bs_passthrough;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set bitstream passthrough onoff\n");

			if (copy_from_user(&bs_passthrough, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (bs_passthrough != g_aidk_info.bs_passthrough) {
				rtd_pr_adsp_info("[rtkaudio] bitstream passthrough onoff changed (%d > %d)\n", g_aidk_info.bs_passthrough, bs_passthrough);
				g_aidk_info.bs_passthrough = bs_passthrough;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_BS_PASSHTROUGH_ON;
				cmd_info.value[0] = g_aidk_info.bs_passthrough;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}

			ret = 0;
			break;
		}
		case AIDK_DAP_SET_PARAMS:
		{
			DAP_PARAM dap_params;
			DAP_PARAM *v_dap = NULL;
			void *unvir_addr = NULL;
			rtd_pr_adsp_info("[rtkaudio] set dap params\n");

			if (copy_from_user(&dap_params, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			vir_addr = get_rpc_mem(&unvir_addr);

			if (vir_addr == NULL) {
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO malloc failed\n", __FUNCTION__, __LINE__);
				return ret;
			}

			memset(unvir_addr, 0, 4096);
			v_dap = (DAP_PARAM *) unvir_addr;
			memcpy(v_dap, &dap_params, sizeof(DAP_PARAM));

			parameter.type = ENUM_PRIVATEINFO_AUDIO_SET_DAP_PARAM;
			parameter.privateInfo[0] = dvr_to_phys(vir_addr);
			parameter.instanceID =0;// no care

			rpc_res = RTKAUDIO_RPC_TOAGENT_PRIVATEINFO_SVC(&parameter, &res);
			if (rpc_res != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				put_rpc_mem(vir_addr);
				return ret;
			}

			put_rpc_mem(vir_addr);

			ret = 0;
			break;
		}
		case AIDK_ENABLE_FLEX_MODE:
		{
			unsigned int flex_mode = 0;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set flex mode %d\n", flex_mode);

			if (copy_from_user(&flex_mode, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (g_aidk_info.flex_mode != flex_mode) {
				rtd_pr_adsp_info("[rtkaudio] flex mode changed (%d > %d)\n", g_aidk_info.flex_mode, flex_mode);
				g_aidk_info.flex_mode = flex_mode;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_ENABLE_FLEX_MODE;
				cmd_info.value[0] = flex_mode;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}
			ret = 0;
			break;
		}
		case AIDK_SET_FLEXR_CENTER_EXTRACT_MODE:
		{
			int center_extract_mode = 0;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set flexR center extract mode%d\n", center_extract_mode);

			if (copy_from_user(&center_extract_mode, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (g_aidk_info.center_extract_mode != center_extract_mode) {
				rtd_pr_adsp_info("[rtkaudio] flexR center extract mode changed (%d > %d)\n", g_aidk_info.center_extract_mode, center_extract_mode);
				g_aidk_info.center_extract_mode = center_extract_mode;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_SET_FLEXR_CENTER_EXTRACT_MODE;
				cmd_info.value[0] = center_extract_mode;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}
			ret = 0;
			break;
		}
		case AIDK_ENFORCE_SINGLE_OA_ELEMENT:
		{
			unsigned int b_enforce_single_oa_element = 0;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] enforce single oa element %d\n", b_enforce_single_oa_element);

			if (copy_from_user(&b_enforce_single_oa_element, (const void __user*) (long) aidk_info.addr, aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (g_aidk_info.b_enforce_single_oa_element != b_enforce_single_oa_element) {
				rtd_pr_adsp_info("[rtkaudio] enforece single oa element changed (%d > %d)\n", g_aidk_info.b_enforce_single_oa_element, b_enforce_single_oa_element);
				g_aidk_info.b_enforce_single_oa_element = b_enforce_single_oa_element;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_ENFORCE_SINGLE_OA_ELEMENT;
				cmd_info.value[0] = b_enforce_single_oa_element;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}
			ret = 0;
			break;
		}
		default:
			ret = -EFAULT;
			break;
	}


	return ret;
}

long aidk_ioctl_get(aidk_cmd_info aidk_info)
{
	long ret = 0;

	switch(aidk_info.type)
	{
		case AIDK_GET_HIGHEST_CAP:
		{
			int aidk_caps;

			aidk_caps = rtkaudio_get_audio_aidk_caps();
			aidk_caps = aidk_caps & 0x3;
			if (g_aidk_info.highest_caps != aidk_caps) {
				rtd_pr_adsp_info("[rtkaudio] [%s:%d] AIDK_GET_HIGHEST_CAP aidk_caps %d\n", __FUNCTION__, __LINE__, aidk_caps);
				g_aidk_info.highest_caps = aidk_caps;
			}
			ret = 0;

			if (copy_to_user((void __user *)  (long) aidk_info.addr, (void *) &aidk_caps, sizeof(unsigned int))) {
				ret = -EFAULT;
			}

			break;
		}
		case AIDK_GET_ATMOS_INDICATOR:
		{
			int is_atmos = 0;
			void *vir_addr, *unvir_addr;
			AUDIO_CONFIG_COMMAND_RTKAUDIO cmd_info;
			AUDIO_RPC_DEC_FORMAT_INFO *dec_info;
			memset(&cmd_info, 0, sizeof(cmd_info));

			//rtd_pr_adsp_err("[rtkaudio] AIDK_GET_ATMOS_INDICATOR %d\n", __LINE__);

			vir_addr = get_rpc_mem(&unvir_addr);

			if (!vir_addr) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO cma malloc error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			memset(unvir_addr, 0, 4096);

			cmd_info.msg_id = AUDIO_CONFIG_CMD_MEDIA_FORMAT;
			cmd_info.value[0] = dvr_to_phys(vir_addr);
			cmd_info.value[1] = sizeof(AUDIO_RPC_DEC_FORMAT_INFO);

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				put_rpc_mem(vir_addr);
				return ret;
			}
			ret = 0;

			dec_info = (AUDIO_RPC_DEC_FORMAT_INFO *) unvir_addr;

			is_atmos = (dec_info->reserved[1] & 0x1) && (g_aidk_info.spk_virt_mode || g_aidk_info.hp_virt_mode);

			if (g_aidk_info.is_atmos != is_atmos) {
				rtd_pr_adsp_info("[rtkaudio] [%s:%d] AIDK_GET_ATMOS_INDICATOR %d, decoding %x, rendering %d, %d\n",
					__FUNCTION__, __LINE__, is_atmos, dec_info->reserved[1], g_aidk_info.spk_virt_mode, g_aidk_info.hp_virt_mode);
				g_aidk_info.is_atmos = is_atmos;
			}

			if (copy_to_user((void __user *)  (long) aidk_info.addr, (void *) &is_atmos, sizeof(unsigned int))) {
				ret = -EFAULT;
			}

			put_rpc_mem(vir_addr);
			break;
		}
		case AIDK_DAP_GET_PARAMS:
		{
			AUDIO_RPC_PRIVATEINFO_PARAMETERS parameter;
			AUDIO_RPC_PRIVATEINFO_RETURNVAL res;
			void *vir_addr = NULL, *unvir_addr = NULL;
			unsigned int rpc_res;
			DAP_PARAM *v_dap = NULL;

			//rtd_pr_adsp_info("[rtkaudio] AIDK_DAP_GET_PARAMS\n");

			vir_addr = get_rpc_mem(&unvir_addr);

			if (vir_addr == NULL) {
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO malloc failed\n", __FUNCTION__, __LINE__);
				return ret;
			}

			v_dap = (DAP_PARAM *) unvir_addr;
			memset(v_dap, 0, sizeof(DAP_PARAM));

			parameter.type = ENUM_PRIVATEINFO_AUDIO_GET_DAP_PARAM;
			parameter.privateInfo[0] = dvr_to_phys(vir_addr);
			parameter.instanceID = 0;// GET_DAP_PARAM no care

			rpc_res = RTKAUDIO_RPC_TOAGENT_PRIVATEINFO_SVC(&parameter, &res);
			if (rpc_res != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				put_rpc_mem(vir_addr);
				return ret;
			}

			if (copy_to_user((void __user*) (long) aidk_info.addr, (void *) v_dap, sizeof(DAP_PARAM))) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				put_rpc_mem(vir_addr);
				return ret;
			}

			put_rpc_mem(vir_addr);

			ret = 0;
			break;
		}
		case AIDK_ENABLE_FLEX_MODE:
		{
			if (copy_to_user((void __user *)  (long) aidk_info.addr, (void *) &g_aidk_info.flex_mode, sizeof(unsigned int))) {
				return -EFAULT;
			}

			ret = 0;
			break;
		}
		default:
			ret = -EFAULT;
			break;
	}

	return ret;
}

#ifdef CONFIG_ARM64
#ifdef CONFIG_COMPAT
long aidk_ioctl_set_compat(aidk_cmd_info aidk_info)
{
	AUDIO_CONFIG_COMMAND_RTKAUDIO cmd_info;
	long ret = 0;

	AUDIO_RPC_PRIVATEINFO_PARAMETERS parameter;
	AUDIO_RPC_PRIVATEINFO_RETURNVAL res;
	void *vir_addr = NULL;
	unsigned int rpc_res;

	switch(aidk_info.type)
	{
		case AIDK_CHANNEL_MODE:
		{
			AIDK_OUTPUT_CHANNEL_MODE ch_mode = AIDK_AUTO;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set output channel mode\n");

			if (copy_from_user(&ch_mode, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (g_aidk_info.ch_mode != ch_mode) {
				rtd_pr_adsp_info("[rtkaudio] output channel mode changed (%d > %d)\n", g_aidk_info.ch_mode, ch_mode);
				g_aidk_info.ch_mode = ch_mode;

				rtd_pr_adsp_info("[rtkaudio] output channel mode changed (%d > %d)\n", g_aidk_info.ch_mode, ch_mode);
				g_aidk_info.ch_mode = ch_mode;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_CHANNEL_MODE;
				cmd_info.value[0] = g_aidk_info.ch_mode;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}

			ret = 0;
			break;
		}
		case AIDK_ATMOS_LOCK:
		{
			unsigned int atmos_lock = 0;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set atmos lock %d\n", atmos_lock);

			if (copy_from_user(&atmos_lock, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (g_aidk_info.atmos_lock != atmos_lock) {
				rtd_pr_adsp_info("[rtkaudio] atmos lock changed (%d > %d)\n", g_aidk_info.atmos_lock, atmos_lock);
				g_aidk_info.atmos_lock = atmos_lock;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_SET_DOLBY_OTT_MODE;
				cmd_info.value[0] = atmos_lock;
				cmd_info.value[1] = atmos_lock;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}

			ret = 0;
			break;
		}
		case AIDK_STEREO_OUT_DRC:
		{
			aidk_stereo_drc stout_drc;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set stereo drc parameter\n");

			if (copy_from_user(&stout_drc, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] stereo drc boost changed (%d > %d)\n", g_aidk_info.stout_drc.boost, stout_drc.boost);
			g_aidk_info.stout_drc.boost = stout_drc.boost;


			rtd_pr_adsp_info("[rtkaudio] stereo drc cut changed (%d > %d)\n", g_aidk_info.stout_drc.cut, stout_drc.cut);
			g_aidk_info.stout_drc.cut = stout_drc.cut;

			/* update boost and cut here */
			cmd_info.msg_id = AUDIO_CONFIG_CMD_DD_SCALE_2CH;

			cmd_info.value[0] = 0x7FFFFFFF / 100 * g_aidk_info.stout_drc.cut;
			cmd_info.value[1] = 0x7FFFFFFF / 100 * g_aidk_info.stout_drc.boost;

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] stereo drc drc_mode changed (%d > %d)\n", g_aidk_info.stout_drc.drc_mode, stout_drc.drc_mode);
			g_aidk_info.stout_drc.drc_mode = stout_drc.drc_mode;

			cmd_info.msg_id = AUDIO_CONFIG_CMD_DD_COMP;

			if (stout_drc.drc_mode == AIDK_DRC_LINE) {
				cmd_info.value[0] = COMP_LINEOUT;
			} else {
				cmd_info.value[0] = COMP_RF;
			}

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] stereo dmx_mode changed (%d > %d)\n", g_aidk_info.stout_drc.dmx_mode, stout_drc.dmx_mode);
			g_aidk_info.stout_drc.dmx_mode = stout_drc.dmx_mode;

			cmd_info.msg_id = AUDIO_CONFIG_CMD_DD_DOWNMIXMODE;
			cmd_info.value[1] = 0;
			cmd_info.value[2] = 0x00002379;

			if (stout_drc.dmx_mode == AIDK_DOWNMIX_LORO) {
				cmd_info.value[0] = MODE_STEREO;
			} else if (stout_drc.dmx_mode == AIDK_DOWNMIX_ARIB) {
				cmd_info.value[0] = MODE_ARIB;
			} else {
				cmd_info.value[0] = MODE_DOLBY_SURROUND;
			}

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			ret = 0;
			break;
		}
		case AIDK_MULTI_OUT_DRC:
		{
			aidk_multiout_drc multiout_drc;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set stereo drc parameter\n");

			if (copy_from_user(&multiout_drc, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] multich drc boost changed (%d > %d)\n", g_aidk_info.multiout_drc.boost, multiout_drc.boost);
			g_aidk_info.multiout_drc.boost = multiout_drc.boost;

			rtd_pr_adsp_info("[rtkaudio] multich drc cut changed (%d > %d)\n", g_aidk_info.multiout_drc.cut, multiout_drc.cut);
			g_aidk_info.multiout_drc.cut = multiout_drc.cut;

			/* update boost and cut here */
			cmd_info.msg_id = AUDIO_CONFIG_CMD_DD_SCALE;

			cmd_info.value[0] = 0x7FFFFFFF / 100 * g_aidk_info.multiout_drc.cut;
			cmd_info.value[1] = 0x7FFFFFFF / 100 * g_aidk_info.multiout_drc.boost;

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] multich drc_mode changed (%d > %d)\n", g_aidk_info.multiout_drc.drc_mode, multiout_drc.drc_mode);
			g_aidk_info.multiout_drc.drc_mode = multiout_drc.drc_mode;

			cmd_info.msg_id = AUDIO_CONFIG_CMD_DD_COMP;

			if (multiout_drc.drc_mode == AIDK_DRC_LINE) {
				cmd_info.value[0] = COMP_LINEOUT;
			} else {
				cmd_info.value[0] = COMP_RF;
			}

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			ret = 0;
			break;
		}
	 	case AIDK_AAC_PRL:
		{
			unsigned int aac_prl;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set aac prl\n");

			if (copy_from_user(&aac_prl, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (aac_prl != g_aidk_info.aac_prl) {
				rtd_pr_adsp_info("[rtkaudio] aac prl changed (%d > %d)\n", g_aidk_info.aac_prl, aac_prl);
				g_aidk_info.aac_prl = aac_prl;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_AAC_PRL;
				cmd_info.value[0] = g_aidk_info.aac_prl;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}

			ret = 0;
			break;
		}
		case AIDK_AC4_DE:
		{
			unsigned int ac4_de;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set ac4 dialog enhancemeant\n");

			if (copy_from_user(&ac4_de, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] ac4 de changed (%d > %d)\n", g_aidk_info.ac4_de, ac4_de);
			g_aidk_info.ac4_de = ac4_de;

			cmd_info.msg_id = AUDIO_CONFIG_CMD_AC4_DIALOGUE_ENHANCEMENT_GAIN;
			cmd_info.value[0] = g_aidk_info.ac4_de;

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			ret = 0;
			break;
		}
		case AIDK_AC4_PRESENTATION_ID:
		{
			unsigned int ac4_presentation_id;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set ac4 presentation ID\n");

			if (copy_from_user(&ac4_presentation_id, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] ac4 presentation ID (%d > %d)\n", g_aidk_info.ac4_presentation_id, ac4_presentation_id);
			g_aidk_info.ac4_presentation_id = ac4_presentation_id;

			cmd_info.msg_id = AUDIO_CONFIG_CMD_AC4_PRESENTATION_GROUP_INDEX;
			cmd_info.value[0] = g_aidk_info.ac4_presentation_id;

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			ret = 0;
			break;
		}
		case AIDK_AC4_SHORT_PROGRAM_ID:
		{
			unsigned int ac4_short_program_id;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set ac4 short program ID\n");

			if (copy_from_user(&ac4_short_program_id, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] ac4 short program ID (%d > %d)\n", g_aidk_info.ac4_short_program_id, ac4_short_program_id);
			g_aidk_info.ac4_short_program_id = ac4_short_program_id;

			cmd_info.msg_id = AUDIO_CONFIG_CMD_AC4_SHORT_PROGRAM_INDEX;
			cmd_info.value[0] = g_aidk_info.ac4_short_program_id;

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			ret = 0;
			break;
		}
		case AIDK_DAP_DE:
		{
			unsigned int dap_de;
			void *unvir_addr = NULL;
			DAP_PARAM_DIALOGUE_ENHANCER *v_dap = NULL;
			rtd_pr_adsp_info("[rtkaudio] set dap dialog enhancemeant\n");

			if (copy_from_user(&dap_de, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] dap de changed (%d > %d)\n", g_aidk_info.dap_de, dap_de);
			g_aidk_info.dap_de = dap_de;

			vir_addr = get_rpc_mem(&unvir_addr);

			if (vir_addr == NULL) {
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO malloc failed\n", __FUNCTION__, __LINE__);
				return ret;
			}

			v_dap = (DAP_PARAM_DIALOGUE_ENHANCER *) unvir_addr;
			v_dap->de_amount = g_aidk_info.dap_de;

			parameter.type = ENUM_PRIVATEINFO_AUDIO_SET_DAP_DIALOGUE_ENHANCER;
			parameter.privateInfo[0] = dvr_to_phys(vir_addr);
			parameter.instanceID =0;// no care

			rpc_res = RTKAUDIO_RPC_TOAGENT_PRIVATEINFO_SVC(&parameter, &res);
			if (rpc_res != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				put_rpc_mem(vir_addr);
				return ret;
			}

			put_rpc_mem(vir_addr);

			ret = 0;
			break;
		}
		case AIDK_VIRTUALIZER_MODE:
		{
			aidk_virt_mode virt_mode;
			void *unvir_addr = NULL;
			DAP_PARAM_SURROUND_VIRTUALIZER *v_dap = NULL;
			rtd_pr_adsp_info("[rtkaudio] set virtualizer mode\n");

			if (copy_from_user(&virt_mode, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (virt_mode.dev == AIDK_SPEAKER) {
				rtd_pr_adsp_info("[rtkaudio] speaker virtual mode changed (%d > %d)\n", g_aidk_info.spk_virt_mode, virt_mode.mode);
				g_aidk_info.spk_virt_mode = virt_mode.mode;

				vir_addr = get_rpc_mem(&unvir_addr);

				if (vir_addr == NULL) {
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO malloc failed\n", __FUNCTION__, __LINE__);
				return ret;
				}

				v_dap = (DAP_PARAM_SURROUND_VIRTUALIZER *) unvir_addr;
				v_dap->virtualizer_mode = g_aidk_info.spk_virt_mode;
				v_dap->surround_boost = g_aidk_info.dap_virt_boost;

				parameter.type = ENUM_PRIVATEINFO_AUDIO_SET_DAP_SURROUND_VIRTUALIZER;
				parameter.privateInfo[0] = dvr_to_phys(vir_addr);
				parameter.instanceID =0;// no care

				rpc_res = RTKAUDIO_RPC_TOAGENT_PRIVATEINFO_SVC(&parameter, &res);
				if (rpc_res != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				put_rpc_mem(vir_addr);
				return ret;
				}

				put_rpc_mem(vir_addr);
			} else if (virt_mode.dev == AIDK_HEADPHONE) {
				rtd_pr_adsp_info("[rtkaudio] headphone virtual mode changed (%d > %d)\n", g_aidk_info.hp_virt_mode, virt_mode.mode);
				g_aidk_info.hp_virt_mode = virt_mode.mode;

				/* didn't support headphone virtualizer */
                        } else {
				rtd_pr_adsp_info("[rtkaudio] unknown device for virtual mode (%d, %d)\n", virt_mode.dev, virt_mode.mode);
			}

			ret = 0;
			break;
		}
		case AIDK_VOLUME_LEVELER_MODE:
		{
			AIDK_VL_MODE vl_mode;
			DAP_PARAM_VOLUME_LEVELER *v_dap = NULL;
			void *unvir_addr = NULL;
			rtd_pr_adsp_info("[rtkaudio] set volume leveler mode\n");

			if (copy_from_user(&vl_mode, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] volume leveler mode (%d > %d)\n", g_aidk_info.vl_mode, vl_mode);
			g_aidk_info.vl_mode = vl_mode;

			vir_addr = get_rpc_mem(&unvir_addr);

			if (vir_addr == NULL) {
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO malloc failed\n", __FUNCTION__, __LINE__);
				return ret;
			}

			v_dap = (DAP_PARAM_VOLUME_LEVELER *) unvir_addr;
			v_dap->leveler_setting = g_aidk_info.vl_mode;
			v_dap->leveler_amount  = g_aidk_info.vl_amount;

			parameter.type = ENUM_PRIVATEINFO_AUDIO_SET_DAP_VOLUME_LEVELER;
			parameter.privateInfo[0] = dvr_to_phys(vir_addr);
			parameter.instanceID =0;// no care

			rpc_res = RTKAUDIO_RPC_TOAGENT_PRIVATEINFO_SVC(&parameter, &res);
			if (rpc_res != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				put_rpc_mem(vir_addr);
				return ret;
			}

			put_rpc_mem(vir_addr);

			ret = 0;
			break;
		}
		case AIDK_VOLUME_LEVELER_AMOUNT:
		{
			unsigned int vl_amount;
			void *unvir_addr = NULL;
			DAP_PARAM_VOLUME_LEVELER *v_dap = NULL;
			rtd_pr_adsp_info("[rtkaudio] set volume leveler amount\n");

			if (copy_from_user(&vl_amount, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] volume leveler amount (%d > %d)\n", g_aidk_info.vl_amount, vl_amount);
			g_aidk_info.vl_amount = vl_amount;

			vir_addr = get_rpc_mem(&unvir_addr);

			if (vir_addr == NULL) {
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO malloc failed\n", __FUNCTION__, __LINE__);
				return ret;
			}

			v_dap = (DAP_PARAM_VOLUME_LEVELER *) unvir_addr;
			v_dap->leveler_setting = g_aidk_info.vl_mode;
			v_dap->leveler_amount  = g_aidk_info.vl_amount;

			parameter.type = ENUM_PRIVATEINFO_AUDIO_SET_DAP_VOLUME_LEVELER;
			parameter.privateInfo[0] = dvr_to_phys(vir_addr);
			parameter.instanceID =0;// no care

			rpc_res = RTKAUDIO_RPC_TOAGENT_PRIVATEINFO_SVC(&parameter, &res);
			if (rpc_res != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				put_rpc_mem(vir_addr);
				return ret;
			}

			put_rpc_mem(vir_addr);

			ret = 0;
			break;
		}
		case AIDK_CONTINUOUS_OUT:
		{
			unsigned int conti_out;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set continue output\n");

			if (copy_from_user(&conti_out, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (conti_out != g_aidk_info.conti_out) {
				rtd_pr_adsp_info("[rtkaudio] continue output changed (%d > %d)\n", g_aidk_info.conti_out, conti_out);
				g_aidk_info.conti_out = conti_out;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_CONTINUOUS_OUT;
				cmd_info.value[0] = g_aidk_info.conti_out;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}

			ret = 0;
			break;
		}
		case AIDK_AUDIO_DUMP:
		{
			unsigned int dump_on;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set Audio Dump onoff\n");

			if (copy_from_user(&dump_on, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (dump_on != g_aidk_info.dump_on) {
				char cmd_buf[100] = {0};
				int size = 0;

				rtd_pr_adsp_info("[rtkaudio] Audio Dump onoff changed (%d > %d)\n", g_aidk_info.dump_on, dump_on);
				g_aidk_info.dump_on = dump_on;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_AIDK_DUMP;
				cmd_info.value[0] = g_aidk_info.dump_on;

				if (g_aidk_info.dump_on) {
					ret = rtkaudio_send_audio_config(&cmd_info);
					if (ret != S_OK) {
						ret = -EFAULT;
						rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
						return ret;
					}

					size = 19;
					snprintf(cmd_buf, size, "dump_enable 163840");
					rtkaudio_send_string(&cmd_buf[0], size);
				} else {
					size = 20;
					snprintf(cmd_buf, size, "dump_disable 163840");
					rtkaudio_send_string(&cmd_buf[0], size);

					ret = rtkaudio_send_audio_config(&cmd_info);
					if (ret != S_OK) {
						ret = -EFAULT;
						rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
						return ret;
					}
				}
			}

			ret = 0;
			break;
		}
		case AIDK_AUDIO_DUMP_MODE:
		{
			AIDK_DUMP_MODE dump_mode;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set Audio Dump mode\n");

			if (copy_from_user(&dump_mode, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (dump_mode != g_aidk_info.dump_mode) {
				rtd_pr_adsp_info("[rtkaudio] Audio Dump mode changed (%d > %d)\n", g_aidk_info.dump_mode, dump_mode);
				g_aidk_info.dump_mode = dump_mode;
			}

			ret = 0;
			break;
		}
		case AIDK_AC4_PRES_PREF:
		{
			aidk_ac4_pref ac4_pref;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set AC4 presentation preference\n");

			if (copy_from_user(&ac4_pref, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (ac4_pref.pres_mode != g_aidk_info.ac4_pref.pres_mode) {
				rtd_pr_adsp_info("[rtkaudio] AC4 presentation mode changed (%d > %d)\n", g_aidk_info.ac4_pref.pres_mode, ac4_pref.pres_mode);
				g_aidk_info.ac4_pref.pres_mode = ac4_pref.pres_mode;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_AC4_PRIORITIZE_ADTYPE;
				cmd_info.value[0] = g_aidk_info.ac4_pref.pres_mode;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}

			if (ac4_pref.pres_assoc_type != g_aidk_info.ac4_pref.pres_assoc_type) {
				rtd_pr_adsp_info("[rtkaudio] AC4 presentation associate type changed (%d > %d)\n", g_aidk_info.ac4_pref.pres_assoc_type, ac4_pref.pres_assoc_type);
				g_aidk_info.ac4_pref.pres_assoc_type = ac4_pref.pres_assoc_type;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_AC4_ADTYPE;
				cmd_info.value[0] = g_aidk_info.ac4_pref.pres_assoc_type;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}

			if (rtkaudio_isvalid_lang(ac4_pref.first_lang)) {
				unsigned int *lang;
				int idx = 0;

				rtd_pr_adsp_info("[rtkaudio] set AC4 fisrt lang %s\n", ac4_pref.first_lang);
				for (idx = 0; idx < 4; idx++) {
					g_aidk_info.ac4_pref.first_lang[idx] = ac4_pref.first_lang[3 - idx];
				}

				lang = (unsigned int *) &g_aidk_info.ac4_pref.first_lang[0];
				cmd_info.msg_id = AUDIO_CONFIG_CMD_AC4_FIRST_LANGUAGE;
				cmd_info.value[0] = *lang;
				if (ac4_pref.first_lang[2] != 0) {
					cmd_info.value[1] = 1;
				} else {
					cmd_info.value[1] = 0;
				}

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			} else {
				rtd_pr_adsp_info("[rtkaudio] unknown AC4 first lang %s, ignore\n", ac4_pref.first_lang);
			}

			if (rtkaudio_isvalid_lang(ac4_pref.second_lang)) {
				unsigned int *lang;
				int idx = 0;

				rtd_pr_adsp_info("[rtkaudio] set AC4 second lang %s\n", ac4_pref.second_lang);
				for (idx = 0; idx < 4; idx++) {
					g_aidk_info.ac4_pref.second_lang[idx] = ac4_pref.second_lang[3 - idx];
				}

				lang = (unsigned int *) &g_aidk_info.ac4_pref.second_lang[0];
				cmd_info.msg_id = AUDIO_CONFIG_CMD_AC4_SECOND_LANGUAGE;
				cmd_info.value[0] = *lang;
				if (ac4_pref.second_lang[2] != 0) {
					cmd_info.value[1] = 1;
				} else {
					cmd_info.value[1] = 0;
				}

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			} else {
				rtd_pr_adsp_info("[rtkaudio] unknown AC4 second lang %s, ignore\n", ac4_pref.second_lang);
			}

			ret = 0;
			break;
		}
		case AIDK_DAP_SPK_VIRT_ANGLE:
		{
			/* It is MS12 v2.3 API, ignore */
			ret = -1;
			break;
		}
		case AIDK_DAP_SPK_VIRT_FREQ:
		{
			/* It is MS12 v2.3 API, ignore */
			ret = -1;
			break;
		}
		case AIDK_DAP_VIRT_SURR_BOOST:
		{
			unsigned int dap_virt_boost;
			void *unvir_addr = NULL;
			DAP_PARAM_SURROUND_VIRTUALIZER *v_dap = NULL;
			rtd_pr_adsp_info("[rtkaudio] set DAP virtualizer boost\n");

			if (copy_from_user(&dap_virt_boost, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			rtd_pr_adsp_info("[rtkaudio] DAP virtualizer boost changed (%d > %d)\n", g_aidk_info.dap_virt_boost, dap_virt_boost);
			g_aidk_info.dap_virt_boost = dap_virt_boost;

			vir_addr = get_rpc_mem(&unvir_addr);

			if (vir_addr == NULL) {
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO malloc failed\n", __FUNCTION__, __LINE__);
				return ret;
			}

			v_dap = (DAP_PARAM_SURROUND_VIRTUALIZER *) unvir_addr;
			v_dap->virtualizer_mode = g_aidk_info.spk_virt_mode;
			v_dap->surround_boost = g_aidk_info.dap_virt_boost;

			parameter.type = ENUM_PRIVATEINFO_AUDIO_SET_DAP_SURROUND_VIRTUALIZER;
			parameter.privateInfo[0] = dvr_to_phys(vir_addr);
			parameter.instanceID = 0;// SET_DAP_SURROUND_VIRTUALIZER no care

			rpc_res = RTKAUDIO_RPC_TOAGENT_PRIVATEINFO_SVC(&parameter, &res);
			if (rpc_res != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				put_rpc_mem(vir_addr);
				return ret;
			}

			put_rpc_mem(vir_addr);

			ret = 0;
			break;
		}
		case AIDK_AD_ON:
		{
			unsigned int ad_on;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set Audio Description onoff\n");

			if (copy_from_user(&ad_on, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (ad_on != g_aidk_info.ad_on) {
				rtd_pr_adsp_info("[rtkaudio] Audio Description onoff changed (%d > %d)\n", g_aidk_info.ad_on, ad_on);
				g_aidk_info.ad_on = ad_on;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_AD_MIXING;
				cmd_info.value[0] = g_aidk_info.ad_on;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}

			ret = 0;
			break;
		}
		case AIDK_ATMOS_ON:
		{
			unsigned int atmos_encoder_on;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set ATMOS encoder onoff\n");

			if (copy_from_user(&atmos_encoder_on, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (atmos_encoder_on != g_aidk_info.atmos_encoder_on) {
				rtd_pr_adsp_info("[rtkaudio] ATMOS encoder onoff changed (%d > %d)\n", g_aidk_info.atmos_encoder_on, atmos_encoder_on);
				g_aidk_info.atmos_encoder_on = atmos_encoder_on;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_EAC3_ATMOS_ENCODE_ONOFF;
				cmd_info.value[0] = g_aidk_info.atmos_encoder_on;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}

			ret = 0;
			break;
		}
		case AIDK_DAP_SET_TUNING_FILE:
		{
			void *vir_addr, *tuning_file;
			int file_size;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set DAP tuning file\n");

			file_size = PAGE_ALIGN(aidk_info.size);
			vir_addr = dvr_malloc_uncached_specific(file_size, GFP_DCU1, &tuning_file);
			memset(tuning_file, 0, file_size);

			if (!vir_addr) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO cma malloc error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (copy_from_user(tuning_file, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				dvr_free(vir_addr);
				return ret;
			}

			cmd_info.msg_id = AUDIO_CONFIG_CMD_DAP_TUNINGDATA;
			cmd_info.value[0] = dvr_to_phys(vir_addr);
			cmd_info.value[1] = aidk_info.size;

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				dvr_free(vir_addr);
				return ret;
			}

			dvr_free(vir_addr);
			ret = 0;
			break;
		}
		case AIDK_FLAP_SET_SCONF_FILE:
		{
			void *vir_addr, *sconf_file;
			int file_size;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set FLAP sconf file\n");

			file_size = PAGE_ALIGN(aidk_info.size);
			vir_addr = dvr_malloc_uncached_specific(file_size, GFP_DCU1, &sconf_file);
			memset(sconf_file, 0, file_size);

			if (!vir_addr) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO cma malloc error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (copy_from_user(sconf_file, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				dvr_free(vir_addr);
				return ret;
			}

			cmd_info.msg_id = AUDIO_CONFIG_CMD_SET_FLEXR_CONFIG;
			cmd_info.value[0] = dvr_to_phys(vir_addr);
			cmd_info.value[1] = aidk_info.size;
                        cmd_info.value[2] = 0;

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				dvr_free(vir_addr);
				return ret;
			}

			dvr_free(vir_addr);
			ret = 0;
			break;
		}
		case AIDK_FLAP_SET_DCONF_FILE:
		{
			void *vir_addr, *dconf_file;
			int file_size;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set FLAP sconf file\n");

			file_size = PAGE_ALIGN(aidk_info.size);
			vir_addr = dvr_malloc_uncached_specific(file_size, GFP_DCU1, &dconf_file);
			memset(dconf_file, 0, file_size);

			if (!vir_addr) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO cma malloc error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (copy_from_user(dconf_file, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				dvr_free(vir_addr);
				return ret;
			}

			cmd_info.msg_id = AUDIO_CONFIG_CMD_SET_FLEXR_CONFIG;
			cmd_info.value[0] = dvr_to_phys(vir_addr);
			cmd_info.value[1] = aidk_info.size;
                        cmd_info.value[2] = 1;

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				dvr_free(vir_addr);
				return ret;
			}

			dvr_free(vir_addr);
			ret = 0;
			break;
		}
		case AIDK_AC4_MIX_LEVEL:
		{
			aidk_ac4_mixlevel ac4_mix_level;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set AC4 mix level\n");

			if (copy_from_user(&ac4_mix_level, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (ac4_mix_level.level != g_aidk_info.ac4_mix_level.level) {
				rtd_pr_adsp_info("[rtkaudio] AC4 mix level changed (%d > %d)\n", g_aidk_info.ac4_mix_level.level, ac4_mix_level.level);
				g_aidk_info.ac4_mix_level.level = ac4_mix_level.level;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_AC4_USER_BALANCE_ADJUSTMENT;
				cmd_info.value[0] = g_aidk_info.ac4_mix_level.level;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}

			ret = 0;
			break;
		}
		case AIDK_SET_BS_PASSTHROUGH:
		{
			unsigned int bs_passthrough;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set bitstream passthrough onoff\n");

			if (copy_from_user(&bs_passthrough, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (bs_passthrough != g_aidk_info.bs_passthrough) {
				rtd_pr_adsp_info("[rtkaudio] bitstream passthrough onoff changed (%d > %d)\n", g_aidk_info.bs_passthrough, bs_passthrough);
				g_aidk_info.bs_passthrough = bs_passthrough;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_BS_PASSHTROUGH_ON;
				cmd_info.value[0] = g_aidk_info.bs_passthrough;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}

			ret = 0;
			break;
		}
		case AIDK_DAP_SET_PARAMS:
		{
			DAP_PARAM dap_params;
			DAP_PARAM *v_dap = NULL;
			void *unvir_addr = NULL;
			rtd_pr_adsp_info("[rtkaudio] set dap params\n");

			if (copy_from_user(&dap_params, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			vir_addr = get_rpc_mem(&unvir_addr);

			if (vir_addr == NULL) {
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO malloc failed\n", __FUNCTION__, __LINE__);
				return ret;
			}

			memset(unvir_addr, 0, 4096);
			v_dap = (DAP_PARAM *) unvir_addr;
			memcpy(v_dap, &dap_params, sizeof(DAP_PARAM));

			parameter.type = ENUM_PRIVATEINFO_AUDIO_SET_DAP_PARAM;
			parameter.privateInfo[0] = dvr_to_phys(vir_addr);
			parameter.instanceID =0;// no care

			rpc_res = RTKAUDIO_RPC_TOAGENT_PRIVATEINFO_SVC(&parameter, &res);
			if (rpc_res != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				put_rpc_mem(vir_addr);
				return ret;
			}

			put_rpc_mem(vir_addr);

			ret = 0;
			break;
		}
		case AIDK_ENABLE_FLEX_MODE:
		{
			unsigned int flex_mode = 0;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set flex mode %d\n", flex_mode);

			if (copy_from_user(&flex_mode, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (g_aidk_info.flex_mode != flex_mode) {
				rtd_pr_adsp_info("[rtkaudio] flex mode changed (%d > %d)\n", g_aidk_info.flex_mode, flex_mode);
				g_aidk_info.flex_mode = flex_mode;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_ENABLE_FLEX_MODE;
				cmd_info.value[0] = flex_mode;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}
			ret = 0;
			break;
		}
		case AIDK_SET_FLEXR_CENTER_EXTRACT_MODE:
		{
			int center_extract_mode = 0;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] set flexR center extract mode%d\n", center_extract_mode);

			if (copy_from_user(&center_extract_mode, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (g_aidk_info.center_extract_mode != center_extract_mode) {
				rtd_pr_adsp_info("[rtkaudio] flexR center extract mode changed (%d > %d)\n", g_aidk_info.center_extract_mode, center_extract_mode);
				g_aidk_info.center_extract_mode = center_extract_mode;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_SET_FLEXR_CENTER_EXTRACT_MODE;
				cmd_info.value[0] = center_extract_mode;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}
			ret = 0;
			break;
		}
		case AIDK_ENFORCE_SINGLE_OA_ELEMENT:
		{
			unsigned int b_enforce_single_oa_element = 0;
			memset(&cmd_info, 0, sizeof(cmd_info));
			rtd_pr_adsp_info("[rtkaudio] enforce single oa element %d\n", b_enforce_single_oa_element);

			if (copy_from_user(&b_enforce_single_oa_element, (const void __user*) compat_ptr(aidk_info.addr), aidk_info.size)) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			if (g_aidk_info.b_enforce_single_oa_element != b_enforce_single_oa_element) {
				rtd_pr_adsp_info("[rtkaudio] enforece single oa element changed (%d > %d)\n", g_aidk_info.b_enforce_single_oa_element, b_enforce_single_oa_element);
				g_aidk_info.b_enforce_single_oa_element = b_enforce_single_oa_element;

				cmd_info.msg_id = AUDIO_CONFIG_CMD_ENFORCE_SINGLE_OA_ELEMENT;
				cmd_info.value[0] = b_enforce_single_oa_element;

				ret = rtkaudio_send_audio_config(&cmd_info);

				if (ret != S_OK) {
					ret = -EFAULT;
					rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
					return ret;
				}
			}
			ret = 0;
			break;
		}
		default:
			ret = -EFAULT;
			rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
			break;
	}

	return ret;
}

long aidk_ioctl_get_compat(aidk_cmd_info aidk_info)
{
	long ret = 0;

	switch(aidk_info.type)
	{
		case AIDK_GET_HIGHEST_CAP:
		{
			int aidk_caps;
			aidk_caps = 0;

			aidk_caps = rtkaudio_get_audio_aidk_caps();
			aidk_caps = aidk_caps & 0x3;
			if (g_aidk_info.highest_caps != aidk_caps) {
				rtd_pr_adsp_info("[rtkaudio] [%s:%d] AIDK_GET_HIGHEST_CAP aidk_caps %d\n", __FUNCTION__, __LINE__, aidk_caps);
				g_aidk_info.highest_caps = aidk_caps;
			}
			ret = 0;

			if (copy_to_user((void __user *) compat_ptr(aidk_info.addr), (void *) &aidk_caps, sizeof(unsigned int))) {
				ret = -EFAULT;
			}

			break;
		}
		case AIDK_GET_ATMOS_INDICATOR:
		{
			int is_atmos = 0;
			void *vir_addr, *unvir_addr;
			AUDIO_CONFIG_COMMAND_RTKAUDIO cmd_info;
			AUDIO_RPC_DEC_FORMAT_INFO *dec_info;
			memset(&cmd_info, 0, sizeof(cmd_info));

			//rtd_pr_adsp_info("[rtkaudio] AIDK_GET_ATMOS_INDICATOR\n");

			vir_addr = get_rpc_mem(&unvir_addr);

			if (!vir_addr) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO cma malloc error\n", __FUNCTION__, __LINE__);
				return ret;
			}

			memset(unvir_addr, 0, 4096);

			cmd_info.msg_id = AUDIO_CONFIG_CMD_MEDIA_FORMAT;
			cmd_info.value[0] = dvr_to_phys(vir_addr);
			cmd_info.value[1] = sizeof(AUDIO_RPC_DEC_FORMAT_INFO);

			ret = rtkaudio_send_audio_config(&cmd_info);

			if (ret != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				put_rpc_mem(vir_addr);
				return ret;
			}
			ret = 0;

			dec_info = (AUDIO_RPC_DEC_FORMAT_INFO *) unvir_addr;

			is_atmos = (dec_info->reserved[1] & 0x1) && (g_aidk_info.spk_virt_mode || g_aidk_info.hp_virt_mode);

			if (g_aidk_info.is_atmos != is_atmos) {
				rtd_pr_adsp_info("[rtkaudio] [%s:%d] AIDK_GET_ATMOS_INDICATOR %d, decoding %x, rendering %d, %d\n",
					__FUNCTION__, __LINE__, is_atmos, dec_info->reserved[1], g_aidk_info.spk_virt_mode, g_aidk_info.hp_virt_mode);
				g_aidk_info.is_atmos = is_atmos;
			}

			if (copy_to_user((void __user *) compat_ptr(aidk_info.addr), (void *) &is_atmos, sizeof(unsigned int))) {
				ret = -EFAULT;
			}

			put_rpc_mem(vir_addr);
			break;
		}
		case AIDK_DAP_GET_PARAMS:
		{
			AUDIO_RPC_PRIVATEINFO_PARAMETERS parameter;
			AUDIO_RPC_PRIVATEINFO_RETURNVAL res;
			void *vir_addr = NULL, *unvir_addr = NULL;
			unsigned int rpc_res;
			DAP_PARAM *v_dap = NULL;

			//rtd_pr_adsp_info("[rtkaudio] AIDK_DAP_GET_PARAMS\n");

			vir_addr = get_rpc_mem(&unvir_addr);

			if (vir_addr == NULL) {
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO malloc failed\n", __FUNCTION__, __LINE__);
				return ret;
			}

			v_dap = (DAP_PARAM *) unvir_addr;
			memset(v_dap, 0, sizeof(DAP_PARAM));

			parameter.type = ENUM_PRIVATEINFO_AUDIO_GET_DAP_PARAM;
            parameter.privateInfo[0] = dvr_to_phys(vir_addr);
			parameter.instanceID = 0; // GET_DAP_PARAM no care

			rpc_res = RTKAUDIO_RPC_TOAGENT_PRIVATEINFO_SVC(&parameter, &res);
			if (rpc_res != S_OK) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				put_rpc_mem(vir_addr);
				return ret;
			}

			if (copy_to_user((void __user*) compat_ptr(aidk_info.addr), (void *) v_dap, sizeof(DAP_PARAM))) {
				ret = -EFAULT;
				rtd_pr_adsp_err("[rtkaudio] [%s:%d] AIDK_CMD_INFO error\n", __FUNCTION__, __LINE__);
				put_rpc_mem(vir_addr);
				return ret;
			}

			put_rpc_mem(vir_addr);

			ret = 0;
			break;
		}
		case AIDK_ENABLE_FLEX_MODE:
		{
			if (copy_to_user((void __user *) compat_ptr(aidk_info.addr), (void *) &g_aidk_info.flex_mode, sizeof(unsigned int))) {
				return -EFAULT;
			}

			ret = 0;
			break;
		}
		default:
			ret = -EFAULT;
			break;
	}

	return ret;
}
#endif
#endif
