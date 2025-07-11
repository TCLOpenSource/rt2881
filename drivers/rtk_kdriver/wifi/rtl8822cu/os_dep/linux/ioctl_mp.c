/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#if defined(CONFIG_MP_INCLUDED)

#include <drv_types.h>
#include <rtw_mp.h>
#include "../../hal/phydm/phydm_precomp.h"
#include <linux/ctype.h>


#if defined(CONFIG_RTL8723B)
	#include <rtw_bt_mp.h>
#endif

#define RTW_IWD_MAX_LEN	128
/* IW_PRIV_SIZE_MASK = 2047*/
#define RTW_EXTRA_MAX_LEN	IW_PRIV_SIZE_MASK
inline u8 rtw_do_mp_iwdata_len_chk(const char *caller, u32 len)
{
	u8 is_illegal = _FALSE;
	if (len >= RTW_IWD_MAX_LEN) {
		RTW_ERR("%s : iw data len(%u) > RTW_IWD_MAX_LEN(%u)",
			caller, len, RTW_IWD_MAX_LEN);
		is_illegal = _TRUE;
	}
	return is_illegal;
}

/*
 * Input Format: %s,%d,%d
 *	%s is width, could be
 *		"b" for 1 byte
 *		"w" for WORD (2 bytes)
 *		"dw" for DWORD (4 bytes)
 *	1st %d is address(offset)
 *	2st %d is data to write
 */
int rtw_mp_write_reg(struct net_device *dev,
		     struct iw_request_info *info,
		     struct iw_point *wrqu, char *extra)
{
	char *pch, *pnext;
	char *width_str;
	char width;
	u32 addr, data;
	int ret;
	PADAPTER padapter = rtw_netdev_priv(dev);
	char input[RTW_IWD_MAX_LEN];

	if (rtw_do_mp_iwdata_len_chk(__func__, (wrqu->length + 1)))
		return -EFAULT;

	_rtw_memset(input, 0, sizeof(input));

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	input[wrqu->length] = '\0';

	_rtw_memset(extra, 0, wrqu->length);

	pch = input;

	pnext = strpbrk(pch, " ,.-");
	if (pnext == NULL)
		return -EINVAL;
	*pnext = 0;
	width_str = pch;

	pch = pnext + 1;
	pnext = strpbrk(pch, " ,.-");
	if (pnext == NULL)
		return -EINVAL;
	*pnext = 0;
	/*addr = simple_strtoul(pch, &ptmp, 16);
	_rtw_memset(buf, '\0', sizeof(buf));
	_rtw_memcpy(buf, pch, pnext-pch);
	ret = kstrtoul(buf, 16, &addr);*/
	ret = sscanf(pch, "%x", &addr);

	pch = pnext + 1;
	pnext = strpbrk(pch, " ,.-");
	if ((pch - input) >= wrqu->length)
		return -EINVAL;
	/*data = simple_strtoul(pch, &ptmp, 16);*/
	ret = sscanf(pch, "%x", &data);
	RTW_INFO("data=%x,addr=%x\n", (u32)data, (u32)addr);
	ret = 0;
	width = width_str[0];
	switch (width) {
	case 'b':
		/* 1 byte*/
		if (data > 0xFF) {
			ret = -EINVAL;
			break;
		}
		rtw_write8(padapter, addr, data);
		break;
	case 'w':
		/* 2 bytes*/
		if (data > 0xFFFF) {
			ret = -EINVAL;
			break;
		}
		rtw_write16(padapter, addr, data);
		break;
	case 'd':
		/* 4 bytes*/
		rtw_write32(padapter, addr, data);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}


/*
 * Input Format: %s,%d
 *	%s is width, could be
 *		"b" for 1 byte
 *		"w" for WORD (2 bytes)
 *		"dw" for DWORD (4 bytes)
 *	%d is address(offset)
 *
 * Return:
 *	%d for data readed
 */
int rtw_mp_read_reg(struct net_device *dev,
		    struct iw_request_info *info,
		    struct iw_point *wrqu, char *extra)
{
	char *input = NULL;
	char *pch, *pnext;
	char *width_str;
	char width;
	char data[20], tmp[20];
	u32 addr = 0, strtout = 0;
	u32 i = 0, j = 0, ret = 0, data32 = 0;
	PADAPTER padapter = rtw_netdev_priv(dev);
	char *pextra = extra;

	if (rtw_do_mp_iwdata_len_chk(__func__, (wrqu->length + 1)))
		return -EFAULT;

	if (wrqu->length > 128)
		return -EFAULT;

	input = (char *)rtw_zmalloc(RTW_IWD_MAX_LEN);
	if (!input)
		return -ENOMEM;

	if (copy_from_user(input, wrqu->pointer, wrqu->length)) {
		ret = -EFAULT;
		goto exit;
	}
	input[wrqu->length] = '\0';
	_rtw_memset(extra, 0, wrqu->length);
	_rtw_memset(data, '\0', sizeof(data));
	_rtw_memset(tmp, '\0', sizeof(tmp));
	pch = input;
	pnext = strpbrk(pch, " ,.-");
	if (pnext == NULL) {
		ret = -EINVAL;
		goto exit;
	}
	*pnext = 0;
	width_str = pch;

	pch = pnext + 1;

	ret = sscanf(pch, "%x", &addr);
	if (addr > MP_READ_REG_MAX_OFFSET) {
		ret = -EINVAL;
		goto exit;
	}
	ret = 0;
	width = width_str[0];

	switch (width) {
	case 'b':
		data32 = rtw_read8(padapter, addr);
		RTW_INFO("%x\n", data32);
		snprintf(extra, RTW_EXTRA_MAX_LEN, "%d", data32);
		wrqu->length = strlen(extra);
		break;
	case 'w':
		/* 2 bytes*/
		snprintf(data, sizeof(data), "%04x\n", rtw_read16(padapter, addr));

		for (i = 0 ; i <= strlen(data) ; i++) {
			if (i % 2 == 0) {
				tmp[j] = ' ';
				j++;
			}
			if (data[i] != '\0')
				tmp[j] = data[i];

			j++;
		}
		pch = tmp;
		RTW_INFO("pch=%s", pch);

		while (*pch != '\0') {
			pnext = strpbrk(pch, " ");
			if (!pnext || ((pnext - tmp) > 4))
				break;

			pnext++;
			if (*pnext != '\0') {
				/*strtout = simple_strtoul(pnext , &ptmp, 16);*/
				ret = sscanf(pnext, "%x", &strtout);
				pextra += sprintf(pextra, " %d", strtout);
			} else
				break;
			pch = pnext;
		}
		wrqu->length = strlen(extra);
		break;
	case 'd':
		/* 4 bytes */
		snprintf(data, sizeof(data), "%08x", rtw_read32(padapter, addr));
		/*add read data format blank*/
		for (i = 0 ; i <= strlen(data) ; i++) {
			if (i % 2 == 0) {
				tmp[j] = ' ';
				j++;
			}
			if (data[i] != '\0')
				tmp[j] = data[i];

			j++;
		}
		pch = tmp;
		RTW_INFO("pch=%s", pch);

		while (*pch != '\0') {
			pnext = strpbrk(pch, " ");
			if (!pnext)
				break;

			pnext++;
			if (*pnext != '\0') {
				ret = sscanf(pnext, "%x", &strtout);
				pextra += sprintf(pextra, " %d", strtout);
			} else
				break;
			pch = pnext;
		}
		wrqu->length = strlen(extra);
		break;

	default:
		wrqu->length = 0;
		ret = -EINVAL;
		break;
	}
exit:
	if (input)
		rtw_mfree(input, RTW_IWD_MAX_LEN);
	return ret;
}


/*
 * Input Format: %d,%x,%x
 *	%d is RF path, should be smaller than MAX_RF_PATH_NUMS
 *	1st %x is address(offset)
 *	2st %x is data to write
 */
int rtw_mp_write_rf(struct net_device *dev,
		    struct iw_request_info *info,
		    struct iw_point *wrqu, char *extra)
{
	u32 path, addr, data;
	int ret;
	PADAPTER padapter = rtw_netdev_priv(dev);
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(padapter);
	char *input = NULL;

	if (rtw_do_mp_iwdata_len_chk(__func__, wrqu->length))
		return -EFAULT;

	input = (char *)rtw_zmalloc(RTW_IWD_MAX_LEN);
	if (!input)
		return -ENOMEM;
	if (copy_from_user(input, wrqu->pointer, wrqu->length)) {
		rtw_mfree(input, RTW_IWD_MAX_LEN);
		return -EFAULT;
	}

	ret = sscanf(input, "%d,%x,%x", &path, &addr, &data);
	rtw_mfree(input, RTW_IWD_MAX_LEN);
	if (ret < 3)
		return -EINVAL;

	if (path >= hal_spec->rf_reg_path_num)
		return -EINVAL;
	if (addr > 0xFF)
		return -EINVAL;
	if (data > 0xFFFFF)
		return -EINVAL;

	_rtw_memset(extra, 0, wrqu->length);

	write_rfreg(padapter, path, addr, data);

	snprintf(extra, RTW_EXTRA_MAX_LEN, "write_rf completed\n");
	wrqu->length = strlen(extra);

	return 0;
}


/*
 * Input Format: %d,%x
 *	%d is RF path, should be smaller than MAX_RF_PATH_NUMS
 *	%x is address(offset)
 *
 * Return:
 *	%d for data readed
 */
int rtw_mp_read_rf(struct net_device *dev,
		   struct iw_request_info *info,
		   struct iw_point *wrqu, char *extra)
{
	char *input = NULL;
	char *pch, *pnext;
	char data[20], tmp[20];
	u32 path, addr, strtou;
	u32 ret, i = 0 , j = 0;
	PADAPTER padapter = rtw_netdev_priv(dev);
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(padapter);
	char *pextra = extra;

	if (rtw_do_mp_iwdata_len_chk(__func__, wrqu->length))
		return -EFAULT;

	if (wrqu->length > 128)
		return -EFAULT;

	input = (char *)rtw_zmalloc(RTW_IWD_MAX_LEN);
	if (!input)
		return -ENOMEM;
	if (copy_from_user(input, wrqu->pointer, wrqu->length)) {
		rtw_mfree(input, RTW_IWD_MAX_LEN);
		return -EFAULT;
	}
	ret = sscanf(input, "%d,%x", &path, &addr);
	rtw_mfree(input, RTW_IWD_MAX_LEN);
	if (ret < 2)
		return -EINVAL;

	if (path >= hal_spec->rf_reg_path_num)
		return -EINVAL;

	if (addr > MP_READ_REG_MAX_OFFSET)
		return -EINVAL;

	_rtw_memset(extra, 0, wrqu->length);

	snprintf(data, sizeof(data), "%08x", read_rfreg(padapter, path, addr));
	/*add read data format blank*/
	for (i = 0 ; i <= strlen(data) ; i++) {
		if (i % 2 == 0) {
			tmp[j] = ' ';
			j++;
		}
		tmp[j] = data[i];
		j++;
	}
	pch = tmp;
	RTW_INFO("pch=%s", pch);

	while (*pch != '\0') {
		pnext = strpbrk(pch, " ");
		if (!pnext)
			break;
		pnext++;
		if (*pnext != '\0') {
			/*strtou =simple_strtoul(pnext , &ptmp, 16);*/
			ret = sscanf(pnext, "%x", &strtou);
			pextra += sprintf(pextra, " %d", strtou);
		} else
			break;
		pch = pnext;
	}
	wrqu->length = strlen(extra);

	return 0;
}


int rtw_mp_start(struct net_device *dev,
		 struct iw_request_info *info,
		 struct iw_point *wrqu, char *extra)
{
	int ret = 0;
	PADAPTER padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmppriv = &padapter->mppriv;

	rtw_pm_set_ips(padapter, IPS_NONE);
	LeaveAllPowerSaveMode(padapter);

	pmppriv->bprocess_mp_mode = _TRUE;

	if (rtw_mi_check_fwstate(padapter, WIFI_UNDER_SURVEY)) {
		rtw_mi_buddy_set_scan_deny(padapter, 5000);
		rtw_mi_scan_abort(padapter, _TRUE);
	}

	rtw_hal_set_hwreg(padapter, HW_VAR_CHECK_TXBUF, 0);

	if (rtw_mp_cmd(padapter, MP_START, RTW_CMDF_WAIT_ACK) != _SUCCESS)
		ret = -EPERM;

	_rtw_memset(extra, 0, wrqu->length);
	sprintf(extra, "mp_start %s\n", ret == 0 ? "ok" : "fail");
	wrqu->length = strlen(extra);

	return ret;
}



int rtw_mp_stop(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra)
{
	int ret = 0;
	PADAPTER padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmppriv = &padapter->mppriv;

	if (pmppriv->mode != MP_ON)
		return -EPERM;

	if (rtw_mp_cmd(padapter, MP_STOP, RTW_CMDF_WAIT_ACK) != _SUCCESS)
		ret = -EPERM;

	pmppriv->bprocess_mp_mode = _FALSE;
	_rtw_memset(extra, 0, wrqu->length);
	snprintf(extra, RTW_EXTRA_MAX_LEN, "mp_stop %s\n", ret == 0 ? "ok" : "fail");
	wrqu->length = strlen(extra);

	return ret;
}


int rtw_mp_rate(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra)
{
	u32 rate = MPT_RATE_1M;
	u8 err = 0;
	u8 input[RTW_IWD_MAX_LEN];
	PADAPTER padapter = rtw_netdev_priv(dev);
	PMPT_CONTEXT		pMptCtx = &(padapter->mppriv.mpt_ctx);
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(padapter);
	struct	mp_priv	*pmppriv = &padapter->mppriv;

	if (rtw_do_mp_iwdata_len_chk(__func__, (wrqu->length + 1)))
		return -EFAULT;

	_rtw_memset(input, 0, sizeof(input));
	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	input[wrqu->length] = '\0';
	rate = rtw_mpRateParseFunc(padapter, input);
	pmppriv->rateidx = rate;

	if (rate == 0 && strcmp(input, "1M") != 0) {
		rate = rtw_atoi(input);
		if (rate <= MGN_VHT4SS_MCS9)
			pmppriv->rateidx = MRateToHwRate(rate);
		/*if (rate <= 0x7f)
			rate = wifirate2_ratetbl_inx((u8)rate);
		else if (rate < 0xC8)
			rate = (rate - 0x79 + MPT_RATE_MCS0);
		HT  rate 0x80(MCS0)  ~ 0x8F(MCS15) ~ 0x9F(MCS31) 128~159
		VHT1SS~2SS rate 0xA0 (VHT1SS_MCS0 44) ~ 0xB3 (VHT2SS_MCS9 #63) 160~179
		VHT rate 0xB4 (VHT3SS_MCS0 64) ~ 0xC7 (VHT2SS_MCS9 #83) 180~199
		else
		VHT rate 0x90(VHT1SS_MCS0) ~ 0x99(VHT1SS_MCS9) 144~153
		rate =(rate - MPT_RATE_VHT1SS_MCS0);
		*/
	}

	_rtw_memset(extra, 0, wrqu->length);

	if (pmppriv->rateidx > DESC_RATEVHTSS4MCS9) {
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Set %s Error" , input);
		wrqu->length = strlen(extra);
		return -EINVAL;
	}

	if (hal_spec->tx_nss_num < 2 && MPT_IS_2SS_RATE(HwRateToMPTRate(pmppriv->rateidx)))
		err = 1;
	if (hal_spec->tx_nss_num < 3 && MPT_IS_3SS_RATE(HwRateToMPTRate(pmppriv->rateidx)))
		err = 1;
	if (hal_spec->tx_nss_num < 4 && MPT_IS_4SS_RATE(HwRateToMPTRate(pmppriv->rateidx)))
		err = 1;
	if (!is_supported_vht(padapter->registrypriv.wireless_mode) && MPT_IS_VHT_RATE(HwRateToMPTRate(pmppriv->rateidx)))
		err = 1;
	if (!is_supported_ht(padapter->registrypriv.wireless_mode) && MPT_IS_HT_RATE(HwRateToMPTRate(pmppriv->rateidx)))
		err = 1;

	if (err == 1) {
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Set data rate to %s Error" , input);
		pmppriv->rateidx = 0;
	} else {
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Set data rate to %s index %d" , input, pmppriv->rateidx);
		RTW_INFO("%s: %s rate index=%d\n", __func__, input, pmppriv->rateidx);
		pMptCtx->mpt_rate_index = HwRateToMPTRate(pmppriv->rateidx);
		SetDataRate(padapter);
	}
	wrqu->length = strlen(extra);
	return err;
}

int rtw_mp_channel(struct net_device *dev,
		   struct iw_request_info *info,
		   struct iw_point *wrqu, char *extra)
{

	PADAPTER padapter = rtw_netdev_priv(dev);
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(padapter);
	u8 input[RTW_IWD_MAX_LEN];
	u8	channel = 1;
	struct	mp_priv	*pmppriv = &padapter->mppriv;

	if (rtw_do_mp_iwdata_len_chk(__func__, (wrqu->length + 1)))
		return -EFAULT;

	_rtw_memset(input, 0, sizeof(input));
	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	input[wrqu->length] = '\0';
	channel = rtw_atoi(input);
	_rtw_memset(extra, 0, wrqu->length);
	pmppriv->channel = channel;

	if (channel != pHalData->current_channel) {
		sprintf(extra, "Change channel %d to channel %d", pHalData->current_channel, channel);

		rtw_hal_set_hwreg(padapter, HW_VAR_CHECK_TXBUF, 0);
		rtw_adjust_chbw(padapter, channel, &pmppriv->bandwidth, &pmppriv->prime_channel_offset);
		RTW_INFO("%s: channel=%d,bandwidth=%d\n", __func__, channel, pmppriv->bandwidth);
		SetChannel(padapter);
		pHalData->current_channel = channel;
	} else
		snprintf(extra, RTW_EXTRA_MAX_LEN, "No change current channel:%d", pHalData->current_channel);

	wrqu->length = strlen(extra);
	return 0;
}


int rtw_mp_ch_offset(struct net_device *dev,
		   struct iw_request_info *info,
		   struct iw_point *wrqu, char *extra)
{

	PADAPTER padapter = rtw_netdev_priv(dev);
	u8 input[RTW_IWD_MAX_LEN];
	u32	ch_offset = 0;
	char *pch;

	if (rtw_do_mp_iwdata_len_chk(__func__, (wrqu->length + 1)))
		return -EFAULT;

	_rtw_memset(input, 0, sizeof(input));
	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	input[wrqu->length] = '\0';
	ch_offset = rtw_atoi(input);
	/*RTW_INFO("%s: channel=%d\n", __func__, channel);*/
	_rtw_memset(extra, 0, wrqu->length);
	pch = extra;
	pch += sprintf(pch, "Change prime channel offset %d to %d", padapter->mppriv.prime_channel_offset , ch_offset);
	padapter->mppriv.prime_channel_offset = ch_offset;
	SetChannel(padapter);

	wrqu->length = strlen(extra);
	return 0;
}


int rtw_mp_bandwidth(struct net_device *dev,
		     struct iw_request_info *info,
		     struct iw_point *wrqu, char *extra)
{
	u8 bandwidth = 0, sg = 0;
	PADAPTER padapter = rtw_netdev_priv(dev);
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(padapter);
	struct	mp_priv	*pmppriv = &padapter->mppriv;
	u8 input[RTW_IWD_MAX_LEN];

	if (rtw_do_mp_iwdata_len_chk(__func__, wrqu->length))
		return -EFAULT;

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	if (sscanf(input, "40M=%hhd,shortGI=%hhd", &bandwidth, &sg) > 0)
		RTW_INFO("%s: bw=%hhd sg=%hhd\n", __func__, bandwidth , sg);

	pmppriv->bandwidth = (u8)bandwidth;
	pmppriv->preamble = sg;

	if (bandwidth != pHalData->current_channel_bw) {
		rtw_adjust_chbw(padapter, pmppriv->channel, &bandwidth, &pmppriv->prime_channel_offset);
		RTW_INFO("%s: channel=%d,bandwidth=%d\n", __func__, pmppriv->channel, bandwidth);

		_rtw_memset(extra, 0, wrqu->length);
		sprintf(extra, "Change BW %d to BW %d\n", pHalData->current_channel_bw , bandwidth);
		rtw_hal_set_hwreg(padapter, HW_VAR_CHECK_TXBUF, 0);
		SetBandwidth(padapter);
		pHalData->current_channel_bw = bandwidth;
	} else
		snprintf(extra, RTW_EXTRA_MAX_LEN, "No change current BW %d\n", pHalData->current_channel_bw);

	wrqu->length = strlen(extra);

	return 0;
}


int rtw_mp_txpower_index(struct net_device *dev,
			 struct iw_request_info *info,
			 struct iw_point *wrqu, char *extra)
{
	PADAPTER padapter = rtw_netdev_priv(dev);
 	HAL_DATA_TYPE	*phal_data	= GET_HAL_DATA(padapter);
	char input[RTW_IWD_MAX_LEN];
	u32 rfpath = 0 ;
	u32 txpower_inx = 0, tarpowerdbm = 0;
	char *pextra = extra;

	if (rtw_do_mp_iwdata_len_chk(__func__, (wrqu->length + 1)))
		return -EFAULT;

	if (wrqu->length > 128)
		return -EFAULT;

	_rtw_memset(input, 0, sizeof(input));

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	input[wrqu->length] = '\0';
	_rtw_memset(extra, 0, strlen(extra));

	if (wrqu->length == 2) {
		if (input[0] != '\0' ) {
			rfpath = rtw_atoi(input);
			txpower_inx = mpt_ProQueryCalTxPower(padapter, rfpath);
		}
		pextra += sprintf(pextra, " %d\n", txpower_inx);
		tarpowerdbm = mpt_get_tx_power_finalabs_val(padapter, rfpath);
		if (tarpowerdbm > 0)
			pextra += sprintf(pextra, "\t\t dBm:%d", tarpowerdbm);
	} else {
		if (phal_data->ant_path == 1)
			rfpath = 1;
		else
			rfpath = 0;

		txpower_inx = mpt_ProQueryCalTxPower(padapter, rfpath);
		pextra += sprintf(pextra, "patha=%d", txpower_inx);
		if (phal_data->rf_type > RF_1T2R) {
			txpower_inx = mpt_ProQueryCalTxPower(padapter, 1);
			pextra += sprintf(pextra, ",pathb=%d", txpower_inx);
		}
		if (phal_data->rf_type > RF_2T4R) {
			txpower_inx = mpt_ProQueryCalTxPower(padapter, 2);
			pextra += sprintf(pextra, ",pathc=%d", txpower_inx);
		}
		if (phal_data->rf_type > RF_3T4R) {
			txpower_inx = mpt_ProQueryCalTxPower(padapter, 3);
			pextra += sprintf(pextra, ",pathd=%d", txpower_inx);
		}

		tarpowerdbm = mpt_get_tx_power_finalabs_val(padapter, rfpath);
		pextra += sprintf(pextra, "\n\t\t\tpatha dBm=%d", tarpowerdbm);
		if (phal_data->rf_type > RF_1T2R) {
			tarpowerdbm = mpt_get_tx_power_finalabs_val(padapter, 1);
			pextra += sprintf(pextra, ",pathb dBm=%d", tarpowerdbm);
		}
		if (phal_data->rf_type > RF_2T4R) {
			tarpowerdbm = mpt_get_tx_power_finalabs_val(padapter, 2);
			pextra += sprintf(pextra, ",pathc dBm=%d", tarpowerdbm);
		}
		if (phal_data->rf_type > RF_3T4R) {
			tarpowerdbm = mpt_get_tx_power_finalabs_val(padapter, 3);
			pextra += sprintf(pextra, ",pathd dBm=%d", tarpowerdbm);
		}
	}
	wrqu->length = strlen(extra);

	return 0;
}


int rtw_mp_txpower(struct net_device *dev,
		   struct iw_request_info *info,
		   struct iw_point *wrqu, char *extra)
{
	u32 idx_a = 0, idx_b = 0, idx_c = 0, idx_d = 0;
	int MsetPower = 1;
	u8 *input = NULL;
	char pout_str_buf[8];
	u8 res = 0;
	char *pextra;

	PADAPTER padapter = rtw_netdev_priv(dev);
	PMPT_CONTEXT		pMptCtx = &(padapter->mppriv.mpt_ctx);
	struct mp_priv *pmppriv = &padapter ->mppriv;
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(padapter);

	if (rtw_do_mp_iwdata_len_chk(__func__, wrqu->length))
		return -EFAULT;

	input = (char *)rtw_zmalloc(RTW_IWD_MAX_LEN);
	if (!input)
		return -ENOMEM;

	if (copy_from_user(input, wrqu->pointer, wrqu->length)){
		sprintf(extra, "copy_from_user() lead to an error, line(%d)\n", __LINE__);
		goto mp_txpower_end;
	}
	input[wrqu->length] = '\0';

	MsetPower = strncmp(input, "off", 3);
	if (MsetPower == 0) {
		pmppriv->bSetTxPower = 0;
		snprintf(extra, RTW_EXTRA_MAX_LEN, "MP Set power off");
	} else {
			res = sscanf(input, "patha=%d,pathb=%d,pathc=%d,pathd=%d", &idx_a, &idx_b, &idx_c, &idx_d);

			if (res < 1) {
				if(isdigit(input[0])){
					idx_a = rtw_atoi(input);
					RTW_INFO("direct set RF Path A Power =%d\n", idx_a);
				} else {
					sprintf(extra, "Invalid format on string :%s ", input);
					RTW_INFO("Invalid format on %s !, \
						Get patha=%d,pathb=%d,pathc=%d,pathd=%d\n", input , idx_a , idx_b , idx_c , idx_d);
				}
			}
			if (res > 0 || idx_a != 0) {

				pmppriv->txpoweridx = (u8)idx_a;
				pMptCtx->TxPwrLevel[RF_PATH_A] = (u8)idx_a;
				pMptCtx->TxPwrLevel[RF_PATH_B] = (u8)idx_b;
				pMptCtx->TxPwrLevel[RF_PATH_C] = (u8)idx_c;
				pMptCtx->TxPwrLevel[RF_PATH_D]	= (u8)idx_d;
				pmppriv->bSetTxPower = 1;
				SetTxPower(padapter);
				
				sprintf(extra, "Set power level path_A:%d path_B:%d path_C:%d path_D:%d",
										idx_a , idx_b , idx_c , idx_d);
				goto mp_txpower_end;
			}

			if (sscanf(input, "a_dbm=%d", &idx_a) >= 1) {
				s32 db_temp = 0;
				s16 pout  = 0;
				u32 poutdbm  = 0;
				u32 poutdbm_int;
				u32 poutdbm_dec;
				if (idx_a < 9 || idx_a >= 22) {
					sprintf(extra, "Error input:%d,Tune dBm range is 9-22\n", idx_a);
					goto mp_txpower_end;
				}

				db_temp = (s16)hal_mpt_tssi_turn_target_power(padapter, pout, RF_PATH_A);
				RTW_INFO("%s: mpt_tssi_turn_target_power db_temp=%d\n", __func__, db_temp);

				pout = idx_a*100 - db_temp;
				poutdbm = hal_mpt_tssi_turn_target_power(padapter, pout, RF_PATH_A);
				sprintf(extra, "Path A Set power dbm :%d\n", idx_a);

				pmppriv->bSetTxPower = 1;
			} else if (sscanf(input, "b_dbm=%d", &idx_b) >= 1) {
				s32 db_temp = 0;
				s16 pout  = 0;
				u32 poutdbm  = 0;

				if (idx_b < 9 || idx_b >= 22) {
					sprintf(extra, "Error input:%d,Tune dBm range is 9-22\n", idx_b);
					goto mp_txpower_end;
				}
				db_temp = (s16)hal_mpt_tssi_turn_target_power(padapter, pout, RF_PATH_B);
				RTW_INFO("%s: mpt_tssi_turn_target_power db_temp=%d\n", __func__, db_temp);

				pout = idx_b*100 - db_temp;
				poutdbm = hal_mpt_tssi_turn_target_power(padapter, pout, RF_PATH_B);
				sprintf(extra, "Path B Set power dbm :%d\n", idx_b);

				pmppriv->bSetTxPower = 1;
			} else if (strncmp(input, "pwroffset", 9) == 0) {/*pwroffset patha/pathb=-9 ~ 9*/
				u8 signed_flag = 0;
				u8 rfpath = 0xff;
				int int_num = 0;
				u32 dec_num = 0;
				s16 pout = 0;
				int i;
				u32 poutdbm;
				u8 poutdbm_int;
				u8 poutdbm_dec;

				if (sscanf(input+10, "patha=%7s", pout_str_buf) == 1) {
					rfpath = 0;
				} else if (sscanf(input+10, "pathb=%7s", pout_str_buf) == 1) {
					rfpath = 1;
				} else {
					sprintf(extra, "[pwroffset patha/pathb=-9.00 ~ 9.00]");
					goto mp_txpower_end;
				}

				if(pout_str_buf[0] == '-')
					signed_flag = 1;
				i = sscanf(pout_str_buf, "%d.%3u", &int_num, &dec_num);
				pout = int_num * 100;
				if(i == 2) {
					dec_num = (dec_num < 10) ? dec_num * 10 : dec_num;
					pout += ((pout < 0 || signed_flag == 1) ? -dec_num : dec_num);
				}

				if (pout < -900 || pout > 900 || rfpath == 0xff) {
					sprintf(extra, "CMD Format:[pwroffset patha/pathb=-9.00 ~ 9.00]\n"
							" each scale step value must 0.25 or -0.25\n"
							" scale limit range is -9.00 - 9.00");
					goto mp_txpower_end;
				}

				pmppriv->txpower_dbm_offset = pout;
				RTW_INFO("%s: pout = %d\n", __func__, pout);
				hal_mpt_tssi_set_power_offset(padapter, pout, rfpath);
				sprintf(extra, "Config power offset:%d OK\n", pout);
			} else if (strncmp(input, "dbm", 3) == 0) {
				u8 signed_flag = 0;
				u8 ret = 0xff;
				int int_num = 0;
				u32 dec_num = 0;
				s16 pout = 0;
				int i;
				u32 poutdbm = 0;
				s32 db_temp = 0;
				s16 pset = 0;
				u8 rfpath;

				if (sscanf(input, "dbm=%7s", pout_str_buf) == 1) {
					ret = 0;
				} else {
					sprintf(extra, "[dbm =0 ~ 20.00]");
					goto mp_txpower_end;
				}

				if(pout_str_buf[0] == '-')
					signed_flag = 1;
				i = sscanf(pout_str_buf, "%d.%3u", &int_num, &dec_num);

				pset = int_num * 100;
				if(i == 2) {
					dec_num = (dec_num < 10) ? dec_num * 10 : dec_num;
					pset += ((pset < 0 || signed_flag == 1) ? -dec_num : dec_num);
				}

				if (pset < 900 || pset > 2200 || ret == 0xff) {
					sprintf(extra, "CMD Format:[dbm= 9.00 ~ 22.00]\n"
						" each scale step value must 0.25 or -0.25\n");
					goto mp_txpower_end;
				}
				for (rfpath = 0; rfpath < hal_spec->rf_reg_path_num; rfpath++) {
					db_temp = (s16)hal_mpt_tssi_turn_target_power(padapter, pout, rfpath);
					pout = pset - db_temp;
					RTW_INFO("%s: path[%d] db_temp=%d pout = %d\n", __func__, rfpath, db_temp, pout);
					poutdbm = hal_mpt_tssi_turn_target_power(padapter, pout, rfpath);
				}
				poutdbm = db_temp + pout;

				sprintf(extra, "Path Set power dbm :%d.%d\n", poutdbm / 100, poutdbm % 100);
			}
	}

mp_txpower_end:
	wrqu->length = strlen(extra);
	if (input)
		rtw_mfree(input, RTW_IWD_MAX_LEN);
	return 0;
}


int rtw_mp_ant_tx(struct net_device *dev,
		  struct iw_request_info *info,
		  struct iw_point *wrqu, char *extra)
{
	u8 i;
	u8 input[RTW_IWD_MAX_LEN];
	u16 antenna = 0;
	PADAPTER padapter = rtw_netdev_priv(dev);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);

	if (rtw_do_mp_iwdata_len_chk(__func__, (wrqu->length + 1)))
		return -EFAULT;

	_rtw_memset(input, 0, sizeof(input));
	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	input[wrqu->length] = '\0';
	snprintf(extra, RTW_EXTRA_MAX_LEN, "switch Tx antenna to %s", input);

	for (i = 0; i < strlen(input); i++) {
		switch (input[i]) {
		case 'a':
			antenna |= ANTENNA_A;
			break;
		case 'b':
			antenna |= ANTENNA_B;
			break;
		case 'c':
			antenna |= ANTENNA_C;
			break;
		case 'd':
			antenna |= ANTENNA_D;
			break;
		}
	}
	/*antenna |= BIT(extra[i]-'a');*/
	RTW_INFO("%s: antenna=0x%x\n", __func__, antenna);
	padapter->mppriv.antenna_tx = antenna;

	/*RTW_INFO("%s:mppriv.antenna_rx=%d\n", __func__, padapter->mppriv.antenna_tx);*/
	pHalData->antenna_tx_path = antenna;
	if ((IS_HARDWARE_TYPE_8822C(padapter) || IS_HARDWARE_TYPE_8822E(padapter))
		&& padapter->mppriv.antenna_tx == ANTENNA_B) {
		if (padapter->mppriv.antenna_rx == ANTENNA_A || padapter->mppriv.antenna_rx == ANTENNA_B) {
			padapter->mppriv.antenna_rx = ANTENNA_AB;
			pHalData->AntennaRxPath = ANTENNA_AB;
			RTW_INFO("%s:8822C/8822E Tx-B Rx Ant to AB\n", __func__);
		}
	}
	SetAntenna(padapter);

	wrqu->length = strlen(extra);
	return 0;
}


int rtw_mp_ant_rx(struct net_device *dev,
		  struct iw_request_info *info,
		  struct iw_point *wrqu, char *extra)
{
	u8 i;
	u16 antenna = 0;
	u8 input[RTW_IWD_MAX_LEN];
	PADAPTER padapter = rtw_netdev_priv(dev);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);

	if (rtw_do_mp_iwdata_len_chk(__func__, (wrqu->length + 1)))
		return -EFAULT;

	_rtw_memset(input, 0, sizeof(input));
	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	input[wrqu->length] = '\0';
	/*RTW_INFO("%s: input=%s\n", __func__, input);*/
	_rtw_memset(extra, 0, wrqu->length);

	snprintf(extra, RTW_EXTRA_MAX_LEN, "switch Rx antenna to %s", input);

	for (i = 0; i < strlen(input); i++) {
		switch (input[i]) {
		case 'a':
			antenna |= ANTENNA_A;
			break;
		case 'b':
			antenna |= ANTENNA_B;
			break;
		case 'c':
			antenna |= ANTENNA_C;
			break;
		case 'd':
			antenna |= ANTENNA_D;
			break;
		}
	}

	RTW_INFO("%s: antenna=0x%x\n", __func__, antenna);

	padapter->mppriv.antenna_rx = antenna;
	pHalData->AntennaRxPath = antenna;
	/*RTW_INFO("%s:mppriv.antenna_rx=%d\n", __func__, padapter->mppriv.antenna_rx);*/
	SetAntenna(padapter);
	wrqu->length = strlen(extra);

	return 0;
}


int rtw_set_ctx_destAddr(struct net_device *dev,
			 struct iw_request_info *info,
			 struct iw_point *wrqu, char *extra)
{
	int jj, kk = 0;

	struct pkt_attrib *pattrib;
	struct mp_priv *pmp_priv;
	PADAPTER padapter = rtw_netdev_priv(dev);

	pmp_priv = &padapter->mppriv;
	pattrib = &pmp_priv->tx.attrib;

	if (strlen(extra) < 5)
		return _FAIL;

	RTW_INFO("%s: in=%s\n", __func__, extra);
	for (jj = 0, kk = 0; jj < ETH_ALEN; jj++, kk += 3)
		pattrib->dst[jj] = key_2char2num(extra[kk], extra[kk + 1]);

	RTW_INFO("pattrib->dst:%x %x %x %x %x %x\n", pattrib->dst[0], pattrib->dst[1], pattrib->dst[2], pattrib->dst[3], pattrib->dst[4], pattrib->dst[5]);
	return 0;
}



int rtw_mp_ctx(struct net_device *dev,
	       struct iw_request_info *info,
	       struct iw_point *wrqu, char *extra)
{
	u32 pkTx = 1;
	int countPkTx = 1, cotuTx = 1, CarrSprTx = 1, scTx = 1, sgleTx = 1, stop = 1, payload = 1;
	u32 bStartTest = 1;
	u32 count = 0, pktinterval = 0, pktlen = 0;
	u8 status;
	struct mp_priv *pmp_priv;
	struct pkt_attrib *pattrib;
	PADAPTER padapter = rtw_netdev_priv(dev);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);

	pmp_priv = &padapter->mppriv;
	pattrib = &pmp_priv->tx.attrib;

	if (padapter->registrypriv.mp_mode != 1 ) {
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Error: can't tx ,not in MP mode. \n");
		wrqu->length = strlen(extra);
		return 0;
	}

	if (copy_from_user(extra, wrqu->pointer, wrqu->length))
		return -EFAULT;

	*(extra + wrqu->length) = '\0';
	RTW_INFO("%s: in=%s\n", __func__, extra);
#ifdef CONFIG_CONCURRENT_MODE
	if (!is_primary_adapter(padapter)) {
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Error: MP mode can't support Virtual Adapter, Please to use main Adapter.\n");
		wrqu->length = strlen(extra);
		return 0;
	}
#endif
	countPkTx = strncmp(extra, "count=", 5); /* strncmp TRUE is 0*/
	cotuTx = strncmp(extra, "background", 20);
	CarrSprTx = strncmp(extra, "background,cs", 20);
	scTx = strncmp(extra, "background,sc", 20);
	sgleTx = strncmp(extra, "background,stone", 20);
	pkTx = strncmp(extra, "background,pkt", 20);
	stop = strncmp(extra, "stop", 4);
	payload = strncmp(extra, "payload=", 8);

	if (sscanf(extra, "count=%d,pkt", &count) > 0)
		RTW_INFO("count= %d\n", count);
	if (sscanf(extra, "pktinterval=%d", &pktinterval) > 0)
		RTW_INFO("pktinterval= %d\n", pktinterval);
	if (sscanf(extra, "pktlen=%d", &pktlen) > 0)
		RTW_INFO("pktlen= %d\n", pktlen);

	if (payload == 0) {
			payload = MP_TX_Payload_default_random;
			if (strncmp(extra, "payload=prbs9", 14) == 0) {
				payload = MP_TX_Payload_prbs9;
				snprintf(extra, RTW_EXTRA_MAX_LEN, "config payload PRBS9\n");
			} else {
				if (sscanf(extra, "payload=%x", &payload) > 0){
					RTW_INFO("payload= %x\n", payload);
					snprintf(extra, RTW_EXTRA_MAX_LEN, "config payload setting = %x\n"
									"1. input payload=[]:\n		"
									"[0]: 00, [1]: A5, [2]: 5A, [3]: FF, [4]: PRBS-9, [5]: Random\n"
									"2. specified a hex payload: payload=0xee\n", payload);
				 }
			}
			pmp_priv->tx.payload = payload;
			wrqu->length = strlen(extra);
			return 0;
	}

	if (_rtw_memcmp(extra, "destmac=", 8)) {
		rtw_set_ctx_destAddr(dev, info, wrqu, &extra[8]);
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Set dest mac OK !\n");
		wrqu->length = strlen(extra);
		return 0;
	}
	/*RTW_INFO("%s: count=%d countPkTx=%d cotuTx=%d CarrSprTx=%d scTx=%d sgleTx=%d pkTx=%d stop=%d\n", __func__, count, countPkTx, cotuTx, CarrSprTx, pkTx, sgleTx, scTx, stop);*/
	_rtw_memset(extra, '\0', strlen(extra));

	if (pktinterval != 0) {
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Pkt Interval = %d", pktinterval);
		padapter->mppriv.pktInterval = pktinterval;
		wrqu->length = strlen(extra);
		return 0;

	} else if (pktlen != 0) {
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Pkt len = %d", pktlen);
		pattrib->pktlen = pktlen;
		wrqu->length = strlen(extra);
		return 0;

	} else if (stop == 0) {
		struct xmit_priv	*pxmitpriv = &(padapter->xmitpriv);
		_queue *pfree_xmitbuf_queue = &pxmitpriv->free_xmitbuf_queue;
		_queue *pfree_xmit_queue = &pxmitpriv->free_xmit_queue;

		u32 i = 0;
		bStartTest = 0; /* To set Stop*/
		pmp_priv->tx.stop = 1;
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Stop continuous Tx");
		odm_write_dig(&pHalData->odmpriv, 0x20);
		do {
			if (pxmitpriv->free_xmitframe_cnt == NR_XMITFRAME && pxmitpriv->free_xmitbuf_cnt == NR_XMITBUFF)
				break;
			else {
				i++;
				RTW_INFO("%s:wait queue_empty %d!!\n", __func__, i);
				rtw_msleep_os(10);
			}
		} while (i < 1000);
	} else {
		bStartTest = 1;
		odm_write_dig(&pHalData->odmpriv, 0x3f);
		if ((IS_HARDWARE_TYPE_8822C(padapter) || IS_HARDWARE_TYPE_8822E(padapter))
			&& pmp_priv->antenna_tx == ANTENNA_B) {
			if (pmp_priv->antenna_rx == ANTENNA_A || pmp_priv->antenna_rx == ANTENNA_B) {
				pmp_priv->antenna_rx = ANTENNA_AB;
				pHalData->AntennaRxPath = ANTENNA_AB;
				RTW_INFO("%s:8822C/8822E Tx-B Rx Ant to AB\n", __func__);
				SetAntenna(padapter);
			}
		}
		if (pmp_priv->mode != MP_ON) {
			if (pmp_priv->tx.stop != 1) {
				RTW_INFO("%s:Error MP_MODE %d != ON\n", __func__, pmp_priv->mode);
				return	-EFAULT;
			}
		}
	}

	pmp_priv->tx.count = count;

	if (pkTx == 0 || countPkTx == 0)
		pmp_priv->mode = MP_PACKET_TX;
	if (sgleTx == 0)
		pmp_priv->mode = MP_SINGLE_TONE_TX;
	if (cotuTx == 0)
		pmp_priv->mode = MP_CONTINUOUS_TX;
	if (CarrSprTx == 0)
		pmp_priv->mode = MP_CARRIER_SUPPRISSION_TX;
	if (scTx == 0)
		pmp_priv->mode = MP_SINGLE_CARRIER_TX;

	status = rtw_mp_pretx_proc(padapter, bStartTest, extra);

	if (stop == 0)
		pmp_priv->mode = MP_ON;

	wrqu->length = strlen(extra);
	return status;
}



int rtw_mp_disable_bt_coexist(struct net_device *dev,
			      struct iw_request_info *info,
			      union iwreq_data *wrqu, char *extra)
{
#ifdef CONFIG_BT_COEXIST
	PADAPTER padapter = (PADAPTER)rtw_netdev_priv(dev);

#endif
	u8 input[RTW_IWD_MAX_LEN];
	u32 bt_coexist;

	if (rtw_do_mp_iwdata_len_chk(__func__, (wrqu->data.length + 1)))
		return -EFAULT;

	_rtw_memset(input, 0, sizeof(input));

	if (copy_from_user(input, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	input[wrqu->data.length] = '\0';

	bt_coexist = rtw_atoi(input);

	if (bt_coexist == 0) {
		RTW_INFO("Set OID_RT_SET_DISABLE_BT_COEXIST: disable BT_COEXIST\n");
#ifdef CONFIG_BT_COEXIST
		rtw_btcoex_HaltNotify(padapter);
		rtw_btcoex_SetManualControl(padapter, _TRUE);
		/* Force to switch Antenna to WiFi*/
		rtw_write16(padapter, 0x870, 0x300);
		rtw_write16(padapter, 0x860, 0x110);
#endif
		/* CONFIG_BT_COEXIST */
	} else {
#ifdef CONFIG_BT_COEXIST
		rtw_btcoex_SetManualControl(padapter, _FALSE);
#endif
	}

	return 0;
}


int rtw_mp_arx(struct net_device *dev,
	       struct iw_request_info *info,
	       struct iw_point *wrqu, char *extra)
{
	int bStartRx = 0, bStopRx = 0, bQueryPhy = 0, bQueryMac = 0, bSetBssid = 0, bSetRxframe = 0;
	int bmac_filter = 0, bmon = 0, bSmpCfg = 0;
	u8 *input = NULL;
	char *pch, *token, *tmp[2] = {0x00, 0x00};
	u32 i = 0, jj = 0, kk = 0, cnts = 0, ret = 0;
	PADAPTER padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmppriv = &padapter->mppriv;
	struct dbg_rx_counter rx_counter;

	if (rtw_do_mp_iwdata_len_chk(__func__, wrqu->length))
		return -EFAULT;

	input = (char *)rtw_zmalloc(RTW_IWD_MAX_LEN * sizeof(char));
	if (!input)
		return -ENOMEM;
	if (copy_from_user(input, wrqu->pointer, wrqu->length)){
		ret = -EFAULT;
		goto exit;
	}

	RTW_INFO("%s: %s\n", __func__, input);
#ifdef CONFIG_CONCURRENT_MODE
	if (!is_primary_adapter(padapter)) {
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Error: MP mode can't support Virtual Adapter, Please to use main Adapter.\n");
		wrqu->length = strlen(extra);
		ret = 0;
		goto exit;
	}
#endif
	bStartRx = (strncmp(input, "start", 5) == 0) ? 1 : 0; /* strncmp TRUE is 0*/
	bStopRx = (strncmp(input, "stop", 5) == 0) ? 1 : 0; /* strncmp TRUE is 0*/
	bQueryPhy = (strncmp(input, "phy", 3) == 0) ? 1 : 0; /* strncmp TRUE is 0*/
	bQueryMac = (strncmp(input, "mac", 3) == 0) ? 1 : 0; /* strncmp TRUE is 0*/
	bSetBssid = (strncmp(input, "setbssid=", 8) == 0) ? 1 : 0; /* strncmp TRUE is 0*/
	bSetRxframe = (strncmp(input, "frametype", 9) == 0) ? 1 : 0;
	/*bfilter_init = (strncmp(input, "filter_init",11)==0)?1:0;*/
	bmac_filter = (strncmp(input, "accept_mac", 10) == 0) ? 1 : 0;
	bmon = (strncmp(input, "mon=", 4) == 0) ? 1 : 0;
	bSmpCfg = (strncmp(input , "smpcfg=" , 7) == 0) ? 1 : 0;
	pmppriv->bloopback = (strncmp(input, "loopbk", 6) == 0) ? 1 : 0; /* strncmp TRUE is 0*/

	if (bSetBssid == 1) {
		pch = input;
		while ((token = strsep(&pch, "=")) != NULL) {
			if (i > 1)
				break;
			tmp[i] = token;
			i++;
		}
		if ((tmp[0] != NULL) && (tmp[1] != NULL)) {
			cnts = strlen(tmp[1]) / 2;
			if (cnts < 1) {
				ret = -EFAULT;
				goto exit;
			}
			RTW_INFO("%s: cnts=%d\n", __func__, cnts);
			RTW_INFO("%s: data=%s\n", __func__, tmp[1]);
			for (jj = 0, kk = 0; jj < cnts ; jj++, kk += 2) {
				pmppriv->network_macaddr[jj] = key_2char2num(tmp[1][kk], tmp[1][kk + 1]);
				RTW_INFO("network_macaddr[%d]=%x\n", jj, pmppriv->network_macaddr[jj]);
			}
		} else {
			ret = -EFAULT;
			goto exit;
		}

		pmppriv->bSetRxBssid = _TRUE;
	}
	if (bSetRxframe) {
		if (strncmp(input, "frametype beacon", 16) == 0)
			pmppriv->brx_filter_beacon = _TRUE;
		else
			pmppriv->brx_filter_beacon = _FALSE;
	}

	if (bmac_filter) {
		pmppriv->bmac_filter = bmac_filter;
		pch = input;
		while ((token = strsep(&pch, "=")) != NULL) {
			if (i > 1)
				break;
			tmp[i] = token;
			i++;
		}
		if ((tmp[0] != NULL) && (tmp[1] != NULL)) {
			cnts = strlen(tmp[1]) / 2;
			if (cnts < 1) {
				ret = -EFAULT;
				goto exit;
			}
			RTW_INFO("%s: cnts=%d\n", __func__, cnts);
			RTW_INFO("%s: data=%s\n", __func__, tmp[1]);
			for (jj = 0, kk = 0; jj < cnts ; jj++, kk += 2) {
				pmppriv->mac_filter[jj] = key_2char2num(tmp[1][kk], tmp[1][kk + 1]);
				RTW_INFO("%s mac_filter[%d]=%x\n", __func__, jj, pmppriv->mac_filter[jj]);
			}
		} else {
			ret = -EFAULT;
			goto exit;
		}

	}

	if (bStartRx) {
		snprintf(extra, RTW_EXTRA_MAX_LEN, "start");
		SetPacketRx(padapter, bStartRx, _FALSE);
	} else if (bStopRx) {
		SetPacketRx(padapter, bStartRx, _FALSE);
		pmppriv->bmac_filter = _FALSE;
		pmppriv->bSetRxBssid = _FALSE;
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Received packet OK:%d CRC error:%d ,Filter out:%d", padapter->mppriv.rx_pktcount, padapter->mppriv.rx_crcerrpktcount, padapter->mppriv.rx_pktcount_filter_out);
	} else if (bQueryPhy) {
		_rtw_memset(&rx_counter, 0, sizeof(struct dbg_rx_counter));
		rtw_dump_phy_rx_counters(padapter, &rx_counter);

		RTW_INFO("%s: OFDM_FA =%d\n", __func__, rx_counter.rx_ofdm_fa);
		RTW_INFO("%s: CCK_FA =%d\n", __func__, rx_counter.rx_cck_fa);
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Phy Received packet OK:%d CRC error:%d FA Counter: %d", rx_counter.rx_pkt_ok, rx_counter.rx_pkt_crc_error, rx_counter.rx_cck_fa + rx_counter.rx_ofdm_fa);


	} else if (bQueryMac) {
		_rtw_memset(&rx_counter, 0, sizeof(struct dbg_rx_counter));
		rtw_dump_mac_rx_counters(padapter, &rx_counter);
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Mac Received packet OK: %d , CRC error: %d , Drop Packets: %d\n",
			rx_counter.rx_pkt_ok, rx_counter.rx_pkt_crc_error, rx_counter.rx_pkt_drop);

	}

	if (bmon == 1) {
		ret = sscanf(input, "mon=%d", &bmon);

		if (bmon == 1) {
			pmppriv->rx_bindicatePkt = _TRUE;
			snprintf(extra, RTW_EXTRA_MAX_LEN, "Indicating Receive Packet to network start\n");
		} else {
			pmppriv->rx_bindicatePkt = _FALSE;
			snprintf(extra, RTW_EXTRA_MAX_LEN, "Indicating Receive Packet to network Stop\n");
		}
	}
	if (bSmpCfg == 1) {
		ret = sscanf(input, "smpcfg=%d", &bSmpCfg);

		if (bSmpCfg == 1) {
			pmppriv->bRTWSmbCfg = _TRUE;
			snprintf(extra, RTW_EXTRA_MAX_LEN, "Indicate By Simple Config Format\n");
			SetPacketRx(padapter, _TRUE, _TRUE);
		} else {
			pmppriv->bRTWSmbCfg = _FALSE;
			snprintf(extra, RTW_EXTRA_MAX_LEN, "Indicate By Normal Format\n");
			SetPacketRx(padapter, _TRUE, _FALSE);
		}
	}

	if (pmppriv->bloopback == _TRUE) {
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Enter MAC LoopBack mode\n");
#if defined(CONFIG_RTL8814B)
		/* 1. No adhoc, 2. Enable short cut */
		rtw_write32(padapter, 0x100, 0x0B000EFF);
#else
		rtw_write32(padapter, 0x100, 0x0B0106FF);
#endif
		RTW_INFO("0x100 :0x%x", rtw_read32(padapter, 0x100));
		rtw_write16(padapter, 0x608, 0x30c);
		RTW_INFO("0x608 :0x%x", rtw_read32(padapter, 0x608));
	}

	wrqu->length = strlen(extra) + 1;
	ret = 0;
exit:
	if (input)
		rtw_mfree(input, RTW_IWD_MAX_LEN);
	return ret;
}


int rtw_mp_trx_query(struct net_device *dev,
		     struct iw_request_info *info,
		     struct iw_point *wrqu, char *extra)
{
	u32 txok, txfail, rxok, rxfail, rxfilterout;
	PADAPTER padapter = rtw_netdev_priv(dev);
	PMPT_CONTEXT	pMptCtx		=	&(padapter->mppriv.mpt_ctx);
	RT_PMAC_TX_INFO	PMacTxInfo	=	pMptCtx->PMacTxInfo;

	if (PMacTxInfo.bEnPMacTx == TRUE)
		txok = hal_mpt_query_phytxok(padapter);
	else
		txok = padapter->mppriv.tx.sended;

	txfail = 0;
	rxok = padapter->mppriv.rx_pktcount;
	rxfail = padapter->mppriv.rx_crcerrpktcount;
	rxfilterout = padapter->mppriv.rx_pktcount_filter_out;

	_rtw_memset(extra, '\0', 128);

	snprintf(extra, RTW_EXTRA_MAX_LEN, "Tx OK:%d, Tx Fail:%d, Rx OK:%d, CRC error:%d ,Rx Filter out:%d\n", txok, txfail, rxok, rxfail, rxfilterout);

	wrqu->length = strlen(extra) + 1;

	return 0;
}


int rtw_mp_pwrtrk(struct net_device *dev,
		  struct iw_request_info *info,
		  struct iw_point *wrqu, char *extra)
{
	u8 enable;
	u32 thermal;
	s32 ret;
	PADAPTER padapter = rtw_netdev_priv(dev);
	u8 input[RTW_IWD_MAX_LEN];

	if (rtw_do_mp_iwdata_len_chk(__func__, wrqu->length))
		return -EFAULT;

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	_rtw_memset(extra, 0, wrqu->length);

	enable = 1;
	if (wrqu->length > 1) {
		/* not empty string*/
		if (strncmp(input, "stop", 4) == 0) {
			enable = 0;
			snprintf(extra, RTW_EXTRA_MAX_LEN, "mp tx power tracking stop");
		} else if (sscanf(input, "ther=%d", &thermal) == 1) {
			ret = SetThermalMeter(padapter, (u8)thermal);
			if (ret == _FAIL)
				return -EPERM;
			snprintf(extra, RTW_EXTRA_MAX_LEN, "mp tx power tracking start,target value=%d ok", thermal);
		} else
			return -EINVAL;
	}

	ret = SetPowerTracking(padapter, enable);
	if (ret == _FAIL)
		return -EPERM;

	wrqu->length = strlen(extra);

	return 0;
}



int rtw_mp_psd(struct net_device *dev,
	       struct iw_request_info *info,
	       struct iw_point *wrqu, char *extra)
{
	PADAPTER padapter = rtw_netdev_priv(dev);
	u8 input[RTW_IWD_MAX_LEN];

	if (rtw_do_mp_iwdata_len_chk(__func__, (wrqu->length + 1)))
		return -EFAULT;

	_rtw_memset(input, 0, sizeof(input));
	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	input[wrqu->length] = '\0';
	strcpy(extra, input);

	wrqu->length = mp_query_psd(padapter, extra);

	return 0;
}


int rtw_mp_thermal(struct net_device *dev,
		   struct iw_request_info *info,
		   struct iw_point *wrqu, char *extra)
{
	u8 val[4] = {0};
	u8 ret = 0;
	u16 ther_path_addr[4] = {0};
	u16 cnt = 1;
	PADAPTER padapter = rtw_netdev_priv(dev);
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(padapter);
	int rfpath = RF_PATH_A;

#ifdef CONFIG_RTL8188E
	ther_path_addr[0] = EEPROM_THERMAL_METER_88E;
#endif
#if defined(CONFIG_RTL8812A) || defined(CONFIG_RTL8821A) || defined(CONFIG_RTL8814A)
	ther_path_addr[0] = EEPROM_THERMAL_METER_8812;
#endif
#ifdef CONFIG_RTL8192E
	ther_path_addr[0] = EEPROM_THERMAL_METER_8192E;
#endif
#ifdef CONFIG_RTL8192F
	ther_path_addr[0] = EEPROM_THERMAL_METER_8192F;
#endif
#ifdef CONFIG_RTL8723B
	ther_path_addr[0] = EEPROM_THERMAL_METER_8723B;
#endif
#ifdef CONFIG_RTL8703B
	ther_path_addr[0] = EEPROM_THERMAL_METER_8703B;
#endif
#ifdef CONFIG_RTL8723D
	ther_path_addr[0] = EEPROM_THERMAL_METER_8723D;
#endif
#ifdef CONFIG_RTL8188F
	ther_path_addr[0] = EEPROM_THERMAL_METER_8188F;
#endif
#ifdef CONFIG_RTL8188GTV
	ther_path_addr[0] = EEPROM_THERMAL_METER_8188GTV;
#endif
#ifdef CONFIG_RTL8822B
	ther_path_addr[0] = EEPROM_THERMAL_METER_8822B;
#endif
#ifdef CONFIG_RTL8821C
	ther_path_addr[0] = EEPROM_THERMAL_METER_8821C;
#endif
#ifdef CONFIG_RTL8710B
	ther_path_addr[0] = EEPROM_THERMAL_METER_8710B;
#endif
#ifdef CONFIG_RTL8822C
	ther_path_addr[0]  = EEPROM_THERMAL_METER_A_8822C;
	ther_path_addr[1]  = EEPROM_THERMAL_METER_B_8822C;
#endif
#ifdef CONFIG_RTL8822E
	ther_path_addr[0]  = EEPROM_THERMAL_METER_A_8822E;
	ther_path_addr[1]  = EEPROM_THERMAL_METER_B_8822E;
#endif
#ifdef CONFIG_RTL8814B
	ther_path_addr[0] = EEPROM_THERMAL_METER_A_8814B;
	ther_path_addr[1] = EEPROM_THERMAL_METER_B_8814B;
	ther_path_addr[2] = EEPROM_THERMAL_METER_C_8814B;
	ther_path_addr[3] = EEPROM_THERMAL_METER_D_8814B;
#endif
#ifdef CONFIG_RTL8723F
	ther_path_addr[0] = EEPROM_THERMAL_METER_8723F;
#endif

	if (copy_from_user(extra, wrqu->pointer, wrqu->length))
		return -EFAULT;

	if ((strncmp(extra, "write", 6) == 0)) {
		int i;
		u16 raw_cursize = 0, raw_maxsize = 0;
#ifdef RTW_HALMAC
		raw_maxsize = efuse_GetavailableSize(padapter);
#else
		efuse_GetCurrentSize(padapter, &raw_cursize);
		raw_maxsize = efuse_GetMaxSize(padapter);
#endif
		RTW_INFO("[eFuse available raw size]= %d bytes\n", raw_maxsize - raw_cursize);
		if (2 > raw_maxsize - raw_cursize) {
			RTW_INFO("no available efuse!\n");
			return -EFAULT;
		}

		for (i = 0; i < hal_spec->rf_reg_path_num; i++) {
				GetThermalMeter(padapter, i , &val[i]);
				if (ther_path_addr[i] != 0 && val[i] != 0) {
					if (rtw_efuse_map_write(padapter, ther_path_addr[i], cnt, &val[i]) == _FAIL) {
						RTW_INFO("Error efuse write thermal addr 0x%x ,val = 0x%x\n", ther_path_addr[i], val[i]);
						return -EFAULT;
					}
				} else {
						RTW_INFO("Error efuse write thermal Null addr,val \n");
						return -EFAULT;
				}
		}
		_rtw_memset(extra, 0, wrqu->length);
		snprintf(extra, RTW_EXTRA_MAX_LEN, " efuse write ok :%d", val[0]);
	} else {
		ret = sscanf(extra, "%d", &rfpath);
		if (ret < 1) {
			rfpath = RF_PATH_A;
			RTW_INFO("default thermal of path(%d)\n", rfpath);
		}
		if (rfpath >= hal_spec->rf_reg_path_num)
			return -EINVAL;

		RTW_INFO("read thermal of path(%d)\n", rfpath);
		GetThermalMeter(padapter, rfpath, &val[0]);

		_rtw_memset(extra, 0, wrqu->length);
		snprintf(extra, RTW_EXTRA_MAX_LEN, "%d", val[0]);
	}
	wrqu->length = strlen(extra);

	return 0;
}



int rtw_mp_reset_stats(struct net_device *dev,
		       struct iw_request_info *info,
		       struct iw_point *wrqu, char *extra)
{
	struct mp_priv *pmp_priv;
	PADAPTER padapter = rtw_netdev_priv(dev);

	pmp_priv = &padapter->mppriv;

	pmp_priv->tx.sended = 0;
	pmp_priv->tx_pktcount = 0;
	pmp_priv->rx_pktcount = 0;
	pmp_priv->rx_pktcount_filter_out = 0;
	pmp_priv->rx_crcerrpktcount = 0;

	rtw_reset_phy_rx_counters(padapter);
	rtw_reset_mac_rx_counters(padapter);

	_rtw_memset(extra, 0, wrqu->length);
	snprintf(extra, RTW_EXTRA_MAX_LEN, "mp_reset_stats ok\n");
	wrqu->length = strlen(extra);

	return 0;
}


int rtw_mp_dump(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra)
{
	struct mp_priv *pmp_priv;
	u8 input[RTW_IWD_MAX_LEN];
	PADAPTER padapter = rtw_netdev_priv(dev);

	pmp_priv = &padapter->mppriv;

	if (rtw_do_mp_iwdata_len_chk(__func__, wrqu->length))
		return -EFAULT;

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	if (strncmp(input, "all", 4) == 0) {
		mac_reg_dump(RTW_DBGDUMP, padapter);
		bb_reg_dump(RTW_DBGDUMP, padapter);
		rf_reg_dump(RTW_DBGDUMP, padapter);
	}
	return 0;
}


int rtw_mp_phypara(struct net_device *dev,
		   struct iw_request_info *info,
		   struct iw_point *wrqu, char *extra)
{

	PADAPTER padapter = rtw_netdev_priv(dev);
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(padapter);
	char input[RTW_IWD_MAX_LEN];
	u32		invalxcap = 0, ret = 0, bwrite_xcap = 0, hwxtaladdr = 0;
	u16		pgval;

	if (rtw_do_mp_iwdata_len_chk(__func__, wrqu->length))
		return -EFAULT;

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	RTW_INFO("%s:priv in=%s\n", __func__, input);
	bwrite_xcap = (strncmp(input, "write_xcap=", 11) == 0) ? 1 : 0;

	if (bwrite_xcap == 1) {
		ret = sscanf(input, "write_xcap=%d", &invalxcap);
		invalxcap = invalxcap & 0x7f; /* xtal bit 0 ~6 */
		RTW_INFO("get crystal_cap %d\n", invalxcap);

		if ((IS_HARDWARE_TYPE_8822C(padapter) || IS_HARDWARE_TYPE_8822E(padapter))
			&& ret == 1) {
			hwxtaladdr = 0x110;
			pgval = invalxcap | 0x80; /* reserved default bit7 on */
			pgval = pgval | pgval << 8; /* xtal xi/xo efuse 0x110 0x111 */

			RTW_INFO("Get crystal_cap 0x%x\n", pgval);
			if (rtw_efuse_map_write(padapter, hwxtaladdr, 2, (u8*)&pgval) == _FAIL) {
					RTW_INFO("%s: rtw_efuse_map_write xcap error!!\n", __func__);
					snprintf(extra, RTW_EXTRA_MAX_LEN, "write xcap pgdata fail");
					ret = -EFAULT;
			} else
					snprintf(extra, RTW_EXTRA_MAX_LEN, "write xcap pgdata ok");

		}
	} else {
		ret = sscanf(input, "xcap=%d", &invalxcap);

		if (ret == 1) {
			pHalData->crystal_cap = (u8)invalxcap;
			RTW_INFO("%s:crystal_cap=%d\n", __func__, pHalData->crystal_cap);

			if (rtw_phydm_set_crystal_cap(padapter, pHalData->crystal_cap) == _FALSE) {
				RTW_ERR("set crystal_cap failed\n");
				rtw_warn_on(1);
			}
			snprintf(extra, RTW_EXTRA_MAX_LEN, "Set xcap=%d", invalxcap);
		}
	}

	wrqu->length = strlen(extra) + 1;
	return ret;
}


int rtw_mp_SetRFPath(struct net_device *dev,
		     struct iw_request_info *info,
		     struct iw_point *wrqu, char *extra)
{
	PADAPTER padapter = rtw_netdev_priv(dev);
	char input[RTW_IWD_MAX_LEN];
	int		bMain = 1, bTurnoff = 1;
#ifdef CONFIG_ANTENNA_DIVERSITY
	u8 ret = _TRUE;
#endif

	if (rtw_do_mp_iwdata_len_chk(__func__, wrqu->length))
		return -EFAULT;

	RTW_INFO("%s:iwpriv in=%s\n", __func__, input);

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	bMain = strncmp(input, "1", 2); /* strncmp TRUE is 0*/
	bTurnoff = strncmp(input, "0", 3); /* strncmp TRUE is 0*/

	_rtw_memset(extra, 0, wrqu->length);

	if (bMain == 0) {
		MP_PHY_SetRFPathSwitch(padapter, _TRUE);
		RTW_INFO("%s:PHY_SetRFPathSwitch=TRUE\n", __func__);
		snprintf(extra, RTW_EXTRA_MAX_LEN, "mp_setrfpath Main\n");

	} else if (bTurnoff == 0) {
		MP_PHY_SetRFPathSwitch(padapter, _FALSE);
		RTW_INFO("%s:PHY_SetRFPathSwitch=FALSE\n", __func__);
		snprintf(extra, RTW_EXTRA_MAX_LEN, "mp_setrfpath Aux\n");
	} else {
		bMain = MP_PHY_QueryRFPathSwitch(padapter);
		RTW_INFO("%s:Query RF Path = %s\n", __func__, (bMain ? "Main":"Aux"));
		if (IS_HARDWARE_TYPE_8821C(padapter))
			snprintf(extra, RTW_EXTRA_MAX_LEN, "RF Path %s\n" ,
				(bMain ? "ANT1/S0/PathB Mode: 0":"ANT2/S1/PathA Mode: 1"));
		else
			snprintf(extra, RTW_EXTRA_MAX_LEN, "RF Path %s\n" , (bMain ? "1":"0"));
	}

	wrqu->length = strlen(extra);

	return 0;
}


int rtw_mp_switch_rf_path(struct net_device *dev,
			struct iw_request_info *info,
			struct iw_point *wrqu, char *extra)
{
	PADAPTER padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv;
	char input[RTW_IWD_MAX_LEN];
	char *pch;
	int		bwlg = 1, bwla = 1, btg = 1, bbt=1;
	u8 ret = 0;

	if (rtw_do_mp_iwdata_len_chk(__func__, wrqu->length))
		return -EFAULT;

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	pmp_priv = &padapter->mppriv;

	RTW_INFO("%s: in=%s\n", __func__, input);

	_rtw_memset(extra, '\0', wrqu->length);
	pch = extra;
#ifdef CONFIG_RTL8821C /* only support for 8821c wlg/wla/btg/bt RF switch path */
	if ((strncmp(input, "WLG", 3) == 0) || (strncmp(input, "1", 1) == 0)) {
		pmp_priv->rf_path_cfg = SWITCH_TO_WLG;
		pch += sprintf(pch, "switch rf path WLG\n");

	} else if ((strncmp(input, "WLA", 3) == 0) || (strncmp(input, "2", 1) == 0)) {
		pmp_priv->rf_path_cfg = SWITCH_TO_WLA;
		pch += sprintf(pch, "switch rf path WLA\n");

	} else if ((strncmp(input, "BTG", 3) == 0) || (strncmp(input, "0", 1) == 0)) {
		pmp_priv->rf_path_cfg = SWITCH_TO_BTG;
		pch += sprintf(pch, "switch rf path BTG\n");

	} else if ((strncmp(input, "BT", 3) == 0) || (strncmp(input, "3", 1) == 0)) {
		pmp_priv->rf_path_cfg = SWITCH_TO_BT;
		pch += sprintf(pch, "switch rf path BT\n");
	} else {
		pmp_priv->rf_path_cfg = SWITCH_TO_WLG;
		pch += sprintf(pch, "Error input, default set WLG\n");
		return -EFAULT;
	}

	mp_phy_switch_rf_path_set(padapter, &pmp_priv->rf_path_cfg);
#endif
	wrqu->length = strlen(extra);

	return ret;

}
int rtw_mp_QueryDrv(struct net_device *dev,
		    struct iw_request_info *info,
		    union iwreq_data *wrqu, char *extra)
{
	PADAPTER padapter = rtw_netdev_priv(dev);
	char input[RTW_IWD_MAX_LEN];
	int	qAutoLoad = 1;

	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(padapter);

	if (rtw_do_mp_iwdata_len_chk(__func__, wrqu->data.length))
		return -EFAULT;

	if (copy_from_user(input, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;
	RTW_INFO("%s:iwpriv in=%s\n", __func__, input);

	qAutoLoad = strncmp(input, "autoload", 8); /* strncmp TRUE is 0*/

	if (qAutoLoad == 0) {
		RTW_INFO("%s:qAutoLoad\n", __func__);

		if (pHalData->bautoload_fail_flag)
			snprintf(extra, RTW_EXTRA_MAX_LEN, "fail");
		else
			snprintf(extra, RTW_EXTRA_MAX_LEN, "ok");
	}
	wrqu->data.length = strlen(extra) + 1;
	return 0;
}


int rtw_mp_PwrCtlDM(struct net_device *dev,
		    struct iw_request_info *info,
		    struct iw_point *wrqu, char *extra)
{
	PADAPTER padapter = rtw_netdev_priv(dev);
	u8 *input = NULL;
	int ret = 0;
	u8		pwrtrk_state = 0;
	u8		pwtk_type[5][25] = {"Thermal tracking off","Thermal tracking on",
					"TSSI tracking off","TSSI tracking on","TSSI calibration"};

	if (rtw_do_mp_iwdata_len_chk(__func__, wrqu->length))
		return -EFAULT;

	input = (char *)rtw_zmalloc(RTW_IWD_MAX_LEN);
	if (!input)
		return -ENOMEM;
	if (copy_from_user(input, wrqu->pointer, wrqu->length)) {
		ret = -EFAULT;
		goto exit;
	}

	input[wrqu->length - 1] = '\0';
	RTW_INFO("%s: in=%s\n", __func__, input);

	if (wrqu->length == 2) {
		if(input[0] >= '0' && input[0] <= '4') {
			pwrtrk_state = rtw_atoi(input);
			MPT_PwrCtlDM(padapter, pwrtrk_state);
			snprintf(extra, RTW_EXTRA_MAX_LEN, "PwrCtlDM start %s\n" , pwtk_type[pwrtrk_state]);
		} else {
			snprintf(extra, RTW_EXTRA_MAX_LEN, "Error unknown number ! Please check your input number\n"
				" 0 : Thermal tracking off\n 1 : Thermal tracking on\n 2 : TSSI tracking off\n"
				" 3 : TSSI tracking on\n 4 : TSSI calibration\n");
		}
		wrqu->length = strlen(extra);
		goto exit;
	}
	if (strncmp(input, "start", 5) == 0 || strncmp(input, "thertrk on", 10) == 0) {/* strncmp TRUE is 0*/
		pwrtrk_state = 1;
		snprintf(extra, RTW_EXTRA_MAX_LEN, "PwrCtlDM start %s\n" , pwtk_type[pwrtrk_state]);
	} else if (strncmp(input, "thertrk off", 11) == 0 || strncmp(input, "stop", 5) == 0) {
		pwrtrk_state = 0;
		snprintf(extra, RTW_EXTRA_MAX_LEN, "PwrCtlDM stop %s\n" , pwtk_type[pwrtrk_state]);
	} else if (strncmp(input, "tssitrk off", 11) == 0){
		pwrtrk_state = 2;
		snprintf(extra, RTW_EXTRA_MAX_LEN, "PwrCtlDM stop %s\n" , pwtk_type[pwrtrk_state]);
	} else if (strncmp(input, "tssitrk on", 10) == 0){
		pwrtrk_state = 3;
		snprintf(extra, RTW_EXTRA_MAX_LEN, "PwrCtlDM start %s\n" , pwtk_type[pwrtrk_state]);
	} else if (strncmp(input, "tssik", 5) == 0){
		pwrtrk_state = 4;
		snprintf(extra, RTW_EXTRA_MAX_LEN, "PwrCtlDM start %s\n" , pwtk_type[pwrtrk_state]);
	} else {
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Error input !!!\n"
			" thertrk off : Thermal tracking off\n thertrk on : Thermal tracking on\n"
			" tssitrk off : TSSI tracking off\n tssitrk on : TSSI tracking on\n tssik : TSSI calibration\n\n"
			" 0 : Thermal tracking off\n 1 : Thermal tracking on\n 2 : TSSI tracking off\n"
			" 3 : TSSI tracking on\n 4 : TSSI calibration\n");
		wrqu->length = strlen(extra);
		goto exit;
	}

	MPT_PwrCtlDM(padapter, pwrtrk_state);
	wrqu->length = strlen(extra);

exit:
	if (input)
		rtw_mfree(input, RTW_IWD_MAX_LEN);
	return ret;
}

int rtw_mp_iqk(struct net_device *dev,
		 struct iw_request_info *info,
		 struct iw_point *wrqu, char *extra)
{
	PADAPTER padapter = rtw_netdev_priv(dev);

	rtw_mp_trigger_iqk(padapter);

	return 0;
}

int rtw_mp_lck(struct net_device *dev,
		 struct iw_request_info *info,
		 struct iw_point *wrqu, char *extra)
{
	PADAPTER padapter = rtw_netdev_priv(dev);

	rtw_mp_trigger_lck(padapter);

	return 0;
}

int rtw_mp_dpk(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra)
{
	PADAPTER padapter = rtw_netdev_priv(dev);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct dm_struct		*pDM_Odm = &pHalData->odmpriv;
	struct pwrctrl_priv *pwrctrlpriv = adapter_to_pwrctl(padapter);
	char *pch;

	u8 ips_mode = IPS_NUM; /* init invalid value */
	u8 lps_mode = PS_MODE_NUM; /* init invalid value */

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	*(extra + wrqu->data.length) = '\0';
	pch = extra;

	if (strncmp(extra, "off", 3) == 0 && strlen(extra) < 4) {
			pDM_Odm->dpk_info.is_dpk_enable = 0;
			halrf_dpk_enable_disable(pDM_Odm);
			pch += sprintf(pch, "set dpk off\n");

	} else if (strncmp(extra, "on", 2) == 0 && strlen(extra) < 3) {
			pDM_Odm->dpk_info.is_dpk_enable = 1;
			halrf_dpk_enable_disable(pDM_Odm);
			pch += sprintf(pch, "set dpk on\n");
	} else	{
#ifdef CONFIG_LPS
			lps_mode = pwrctrlpriv->power_mgnt;/* keep org value */
			rtw_pm_set_lps(padapter, PS_MODE_ACTIVE);
#endif
#ifdef CONFIG_IPS
			ips_mode = pwrctrlpriv->ips_mode;/* keep org value */
			rtw_pm_set_ips(padapter, IPS_NONE);
#endif
			rtw_mp_trigger_dpk(padapter);
	if (padapter->registrypriv.mp_mode == 0) {
#ifdef CONFIG_IPS
			rtw_pm_set_ips(padapter, ips_mode);
#endif /* CONFIG_IPS */

#ifdef CONFIG_LPS
			rtw_pm_set_lps(padapter, lps_mode);
#endif /* CONFIG_LPS */
	}
			pch += sprintf(pch, "set dpk trigger\n");
	}

	wrqu->data.length = strlen(extra);

	return 0;
}

int rtw_mp_get_tsside(struct net_device *dev,
			 struct iw_request_info *info,
			 struct iw_point *wrqu, char *extra)
{
	PADAPTER padapter = rtw_netdev_priv(dev);
 	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct dm_struct		*pDM_Odm = &pHalData->odmpriv;
	char input[RTW_IWD_MAX_LEN];
	u8 rfpath;
	u32 tssi_de;

	u8 legal_param_num = 1;
	int param_num;
	char pout_str_buf[8];
	u8 signed_flag = 0;
	int integer_num;
	u32 decimal_num;
	s32 pout;
	char *pextra;
	int i;

	#ifdef CONFIG_RTL8723F
	/*
	* rtwpriv wlan0 mp_get_tsside rf_path pout
	* rf_path : 0 ~ 1
	* pout : -15.000 ~ 25.000
	* ex : rtwpriv wlan0 mp_get_tsside 0 -12.123
	*/
	legal_param_num = 2;
	#endif
	if (wrqu->length > 128)
		return -EFAULT;

	_rtw_memset(input, 0, sizeof(input));

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	param_num = sscanf(input, "%hhu %7s", &rfpath, pout_str_buf);

	/* Check parameter format*/
	if(param_num != legal_param_num)
		goto invalid_param_format;

	if(rfpath <0 || 3 < rfpath)
		goto invalid_param_format;

#ifdef CONFIG_RTL8723F
	/* Convert pout from floating-point to integer
	 * For Floating-Point Precision, pout*1000
	 */
	if(pout_str_buf[0] == '-')
		signed_flag = 1;
	i = sscanf(pout_str_buf, "%d.%3u", &integer_num, &decimal_num);
	pout = integer_num * 1000;
	RTW_INFO("%s() pout1 = %d\n", __func__, pout);

	if (i == 2) {
	/* Convert decimal number
	* ex : 0.1 => 100, 0.01 => 10, 0.001 => 1.
	*/
		u8 idx = 0;
		u32 dec = 0;
		u8 str_len = 0;
		char *token, *tmp[3] = {};

		pextra = pout_str_buf;
		while ((token = strsep (&pextra,".")) != NULL) {
			tmp[idx] = token;
			RTW_INFO("%s() token %d = %s strlen =%ld\n", __func__,
				idx, tmp[idx], strlen(tmp[idx]));
			idx++;
		}

		str_len = strlen(tmp[1]);
		dec = rtw_atoi(tmp[1]);
		decimal_num = (str_len == 1) ? dec * 100: (str_len == 2) ? dec * 10: dec;
		RTW_INFO("%s() decimal_num  = %d\n", __func__, decimal_num);
		pout += ((pout < 0 || signed_flag == 1) ? -decimal_num : decimal_num);
	}
	RTW_INFO("%s() pout2 = %d\n", __func__, pout);
#endif

#ifdef CONFIG_RTL8723F
	/* For Floating-Point Precision, pout */
	tssi_de = halrf_get_online_tssi_de(pDM_Odm, rfpath, pout);
#else
	tssi_de = halrf_tssi_get_de(pDM_Odm, rfpath);
#endif

	if (rfpath == 0)
		sprintf(extra, "patha=%d hex=%02x", tssi_de, (u8)tssi_de);
	else if (rfpath == 1)
		sprintf(extra, "pathb=%d hex=%02x", tssi_de, (u8)tssi_de);
	else if (rfpath == 2)
		sprintf(extra, "pathc=%d hex=%02x", tssi_de, (u8)tssi_de);
	else if (rfpath == 3)
		sprintf(extra, "pathd=%d hex=%02x", tssi_de, (u8)tssi_de);

	wrqu->length = strlen(extra);
	return 0;

invalid_param_format:
	snprintf(extra, RTW_EXTRA_MAX_LEN, "Invalid command format, please indicate RF path 0/1/2/3");
#ifdef CONFIG_RTL8723F
	pextra = extra + strlen(extra);
	snprintf(pextra, RTW_EXTRA_MAX_LEN - strlen(extra), " and pout value : -15.000 ~ 25.000\n");
#endif
	wrqu->length = strlen(extra);

	return 0;
}

int rtw_mp_set_tsside(struct net_device *dev,
		   struct iw_request_info *info,
		   struct iw_point *wrqu, char *extra)
{
	u32 tsside_a = 0, tsside_b = 0, tsside_c = 0, tsside_d = 0;
	char input[RTW_IWD_MAX_LEN];

	PADAPTER padapter = rtw_netdev_priv(dev);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct dm_struct		*pDM_Odm = &pHalData->odmpriv;

	if (rtw_do_mp_iwdata_len_chk(__func__, wrqu->length))
		return -EFAULT;

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	if (sscanf(input, "patha=%d", &tsside_a) == 1) {
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Set TSSI DE path_A: %d", tsside_a);
		halrf_tssi_set_de_for_tx_verify(pDM_Odm, tsside_a, RF_PATH_A);
		mpt_trigger_tssi_tracking(padapter, RF_PATH_A);

	}
	else if (sscanf(input, "pathb=%d", &tsside_b) == 1) {
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Set TSSI DE path_B: %d", tsside_b);
		halrf_tssi_set_de_for_tx_verify(pDM_Odm, tsside_b, RF_PATH_B);
		mpt_trigger_tssi_tracking(padapter, RF_PATH_B);

	}
#if defined(PHYDM_COMPILE_ABOVE_3SS)
	else if (sscanf(input, "pathc=%d", &tsside_c) == 1) {
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Set TSSI DE path_C: %d", tsside_c);
		halrf_tssi_set_de_for_tx_verify(pDM_Odm, tsside_c, RF_PATH_C);
		mpt_trigger_tssi_tracking(padapter, RF_PATH_C);

	}
#endif
#if defined(PHYDM_COMPILE_ABOVE_3SS)
	else if (sscanf(input, "pathd=%d", &tsside_d) == 1) {
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Set TSSI DE path_D: %d", tsside_d);
		halrf_tssi_set_de_for_tx_verify(pDM_Odm, tsside_d, RF_PATH_D);
		mpt_trigger_tssi_tracking(padapter, RF_PATH_D);

	}
#endif
	else
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Invalid command format, please input TSSI DE value within patha/b/c/d=xyz");

	wrqu->length = strlen(extra);

	return 0;
}


int rtw_mp_ant_div(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra)
{
	PADAPTER padapter = rtw_netdev_priv(dev);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	char *pch;
	u8 ret = _FALSE;
	u8 path = 0;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	*(extra + wrqu->data.length) = '\0';
	pch = extra;

	if (strncmp(extra, "0", 1) == 0)
		path = 0;
	else if (strncmp(extra, "1", 1) == 0)
		path = 1;
	else {
		pch += sprintf(pch, "set Ant Div Path not support\n");
		RTW_INFO("%s:Ant Div Path not support\n", __func__);
		wrqu->data.length = strlen(extra);
		return ret;
	}
#ifdef CONFIG_ANTENNA_DIVERSITY
	ret = rtw_mp_set_antdiv(padapter, path);
#endif
	if (ret == _FALSE) {
		pch += sprintf(pch, "set Ant Div Path FAIL\n");
		RTW_INFO("%s:ANTENNA_DIVERSITY FAIL\n", __func__);
	} else {
		RTW_INFO("%s:ANTENNA_DIVERSITY OK\n", __func__);
		pch += sprintf(pch, "set Ant Div Path %s\n", (path == 0 ? "AUX":"MAIN"));
	}

	wrqu->data.length = strlen(extra);

	return ret;
}

int rtw_mp_getver(struct net_device *dev,
		  struct iw_request_info *info,
		  union iwreq_data *wrqu, char *extra)
{
	PADAPTER padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv;

	pmp_priv = &padapter->mppriv;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	snprintf(extra, RTW_EXTRA_MAX_LEN, "rtwpriv=%d\n", RTWPRIV_VER_INFO);
	wrqu->data.length = strlen(extra);
	return 0;
}


int rtw_mp_mon(struct net_device *dev,
	       struct iw_request_info *info,
	       union iwreq_data *wrqu, char *extra)
{
	PADAPTER padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv = &padapter->mppriv;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct hal_ops *pHalFunc = &padapter->hal_func;
	NDIS_802_11_NETWORK_INFRASTRUCTURE networkType;
	int bstart = 1, bstop = 1;

	networkType = Ndis802_11Infrastructure;
	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	*(extra + wrqu->data.length) = '\0';
	rtw_pm_set_ips(padapter, IPS_NONE);
	LeaveAllPowerSaveMode(padapter);

#ifdef CONFIG_MP_INCLUDED
	if (init_mp_priv(padapter) == _FAIL)
		RTW_INFO("%s: initialize MP private data Fail!\n", __func__);
	padapter->mppriv.channel = 6;

	bstart = strncmp(extra, "start", 5); /* strncmp TRUE is 0*/
	bstop = strncmp(extra, "stop", 4); /* strncmp TRUE is 0*/
	if (bstart == 0) {
		mp_join(padapter, WIFI_FW_ADHOC_STATE);
		SetPacketRx(padapter, _TRUE, _FALSE);
		SetChannel(padapter);
		pmp_priv->rx_bindicatePkt = _TRUE;
		pmp_priv->bRTWSmbCfg = _TRUE;
		snprintf(extra, RTW_EXTRA_MAX_LEN, "monitor mode start\n");
	} else if (bstop == 0) {
		SetPacketRx(padapter, _FALSE, _FALSE);
		pmp_priv->rx_bindicatePkt = _FALSE;
		pmp_priv->bRTWSmbCfg = _FALSE;
		padapter->registrypriv.mp_mode = 1;
		pHalFunc->hal_deinit(padapter);
		padapter->registrypriv.mp_mode = 0;
		pHalFunc->hal_init(padapter);
		/*rtw_disassoc_cmd(padapter, 0, 0);*/
		if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE) {
			rtw_disassoc_cmd(padapter, 500, 0);
			rtw_indicate_disconnect(padapter, 0, _FALSE);
			/*rtw_free_assoc_resources_cmd(padapter, _TRUE, 0);*/
		}
		rtw_pm_set_ips(padapter, IPS_NORMAL);
		snprintf(extra, RTW_EXTRA_MAX_LEN, "monitor mode Stop\n");
	}
#endif
	wrqu->data.length = strlen(extra);
	return 0;
}

int rtw_mp_pretx_proc(PADAPTER padapter, u8 bStartTest, char *extra)
{
	struct mp_priv *pmp_priv = &padapter->mppriv;
	char *pextra = extra;

	switch (pmp_priv->mode) {

	case MP_PACKET_TX:
		if (bStartTest == 0) {
			pmp_priv->tx.stop = 1;
			pmp_priv->mode = MP_ON;
			#ifdef CONFIG_RTL8822B
			rtw_write8(padapter, 0x838, 0x61);
			#endif
			snprintf(extra, RTW_EXTRA_MAX_LEN, "Stop continuous Tx");
		} else if (pmp_priv->tx.stop == 1) {
			pextra = extra + strlen(extra);
			pextra += sprintf(pextra, "\nStart continuous DA=ffffffffffff len=1500 count=%u\n", pmp_priv->tx.count);
			pmp_priv->tx.stop = 0;
			#ifdef CONFIG_RTL8822B
			rtw_write8(padapter, 0x838, 0x6d);
			#endif
			SetPacketTx(padapter);
		} else
			return -EFAULT;
		return 0;
	case MP_SINGLE_TONE_TX:
		if (bStartTest != 0)
			strcat(extra, "\nStart continuous DA=ffffffffffff len=1500\n infinite=yes.");
		SetSingleToneTx(padapter, (u8)bStartTest);
		break;
	case MP_CONTINUOUS_TX:
		if (bStartTest != 0)
			strcat(extra, "\nStart continuous DA=ffffffffffff len=1500\n infinite=yes.");
		SetContinuousTx(padapter, (u8)bStartTest);
		break;
	case MP_CARRIER_SUPPRISSION_TX:
		if (bStartTest != 0) {
			if (HwRateToMPTRate(pmp_priv->rateidx) <= MPT_RATE_11M)
				strcat(extra, "\nStart continuous DA=ffffffffffff len=1500\n infinite=yes.");
			else
				strcat(extra, "\nSpecify carrier suppression but not CCK rate");
		}
		SetCarrierSuppressionTx(padapter, (u8)bStartTest);
		break;
	case MP_SINGLE_CARRIER_TX:
		if (bStartTest != 0)
			strcat(extra, "\nStart continuous DA=ffffffffffff len=1500\n infinite=yes.");
		SetSingleCarrierTx(padapter, (u8)bStartTest);
		break;

	default:
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Error! Continuous-Tx is not on-going.");
		return -EFAULT;
	}

	if (bStartTest == 1 && pmp_priv->mode != MP_ON) {
		struct mp_priv *pmp_priv = &padapter->mppriv;

		if (pmp_priv->tx.stop == 0) {
			pmp_priv->tx.stop = 1;
			rtw_msleep_os(5);
		}
#ifdef CONFIG_80211N_HT
		if(padapter->registrypriv.ht_enable &&
			is_supported_ht(padapter->registrypriv.wireless_mode))
			pmp_priv->tx.attrib.ht_en = 1;
#endif
		if (!IS_HARDWARE_TYPE_JAGUAR3(padapter)) {
			pmp_priv->tx.stop = 0;
			pmp_priv->tx.count = 1;
			SetPacketTx(padapter);
		}
	} else
		pmp_priv->mode = MP_ON;

#if defined(CONFIG_RTL8812A)
	if (IS_HARDWARE_TYPE_8812AU(padapter)) {
		/* <20130425, Kordan> Turn off OFDM Rx to prevent from CCA causing Tx hang.*/
		if (pmp_priv->mode == MP_PACKET_TX)
			phy_set_bb_reg(padapter, rCCAonSec_Jaguar, BIT3, 1);
		else
			phy_set_bb_reg(padapter, rCCAonSec_Jaguar, BIT3, 0);
	}
#endif

	return 0;
}


int rtw_mp_tx(struct net_device *dev,
	      struct iw_request_info *info,
	      union iwreq_data *wrqu, char *extra)
{
	PADAPTER padapter = rtw_netdev_priv(dev);
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(padapter);
	struct mp_priv *pmp_priv = &padapter->mppriv;
	PMPT_CONTEXT		pMptCtx = &(padapter->mppriv.mpt_ctx);
	char *pextra = extra;
	u32 bandwidth = 0, sg = 0, channel = 6, txpower = 40, rate = 108, ant = 0, txmode = 1, count = 0;
	u8 bStartTest = 1, status = 0;
#ifdef CONFIG_MP_VHT_HW_TX_MODE
	u8 Idx = 0, tmpU1B;
#endif
	u16 antenna = 0;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;
	RTW_INFO("extra = %s\n", extra);
#ifdef CONFIG_CONCURRENT_MODE
	if (!is_primary_adapter(padapter)) {
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Error: MP mode can't support Virtual Adapter, Please to use main Adapter.\n");
		wrqu->data.length = strlen(extra);
		return 0;
	}
#endif

	if (strncmp(extra, "stop", 3) == 0) {
		bStartTest = 0; /* To set Stop*/
		pmp_priv->tx.stop = 1;
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Stop continuous Tx");
		status = rtw_mp_pretx_proc(padapter, bStartTest, extra);
		wrqu->data.length = strlen(extra);
		return status;
	} else if (strncmp(extra, "count", 5) == 0) {
		if (sscanf(extra, "count=%d", &count) < 1)
			RTW_INFO("Got Count=%d]\n", count);
		pmp_priv->tx.count = count;
		return 0;
	} else if (strncmp(extra, "setting", 7) == 0) {
		_rtw_memset(extra, 0, wrqu->data.length);
		pextra += sprintf(pextra, "Current Setting :\n Channel:%d", pmp_priv->channel);
		pextra += sprintf(pextra, "\n Bandwidth:%d", pmp_priv->bandwidth);
		pextra += sprintf(pextra, "\n Rate index:%d", pmp_priv->rateidx);
		pextra += sprintf(pextra, "\n TxPower index:%d", pmp_priv->txpoweridx);
		pextra += sprintf(pextra, "\n Antenna TxPath:%d", pmp_priv->antenna_tx);
		pextra += sprintf(pextra, "\n Antenna RxPath:%d", pmp_priv->antenna_rx);
		pextra += sprintf(pextra, "\n MP Mode:%d", pmp_priv->mode);
		wrqu->data.length = strlen(extra);
		return 0;
#ifdef CONFIG_MP_VHT_HW_TX_MODE
	} else if (strncmp(extra, "pmact", 5) == 0) {
		if (strncmp(extra, "pmact=", 6) == 0) {
			_rtw_memset(&pMptCtx->PMacTxInfo, 0, sizeof(pMptCtx->PMacTxInfo));
			if (strncmp(extra, "pmact=start", 11) == 0) {
				pMptCtx->PMacTxInfo.bEnPMacTx = _TRUE;
				snprintf(extra, RTW_EXTRA_MAX_LEN, "Set PMac Tx Mode start\n");
			} else {
				pMptCtx->PMacTxInfo.bEnPMacTx = _FALSE;
				snprintf(extra, RTW_EXTRA_MAX_LEN, "Set PMac Tx Mode Stop\n");
			}
			if (pMptCtx->bldpc == TRUE)
				pMptCtx->PMacTxInfo.bLDPC = _TRUE;

			if (pMptCtx->bstbc == TRUE)
				pMptCtx->PMacTxInfo.bSTBC = _TRUE;

			pMptCtx->PMacTxInfo.bSPreamble = pmp_priv->preamble;
			pMptCtx->PMacTxInfo.bSGI = pmp_priv->preamble;
			pMptCtx->PMacTxInfo.BandWidth = pmp_priv->bandwidth;
			pMptCtx->PMacTxInfo.TX_RATE = HwRateToMPTRate(pmp_priv->rateidx);

			pMptCtx->PMacTxInfo.Mode = pMptCtx->HWTxmode;

			pMptCtx->PMacTxInfo.NDP_sound = FALSE;/*(Adapter.PacketType == NDP_PKT)?TRUE:FALSE;*/

			if (padapter->mppriv.pktInterval == 0)
				pMptCtx->PMacTxInfo.PacketPeriod = 100;
			else
				pMptCtx->PMacTxInfo.PacketPeriod = padapter->mppriv.pktInterval;

			if (padapter->mppriv.pktLength < 1000)
				pMptCtx->PMacTxInfo.PacketLength = 1000;
			else
				pMptCtx->PMacTxInfo.PacketLength = padapter->mppriv.pktLength;

			pMptCtx->PMacTxInfo.PacketPattern  = rtw_random32() % 0xFF;

			if (padapter->mppriv.tx_pktcount != 0)
				pMptCtx->PMacTxInfo.PacketCount = padapter->mppriv.tx_pktcount;

			pMptCtx->PMacTxInfo.Ntx = 0;
			for (Idx = 16; Idx < 20; Idx++) {
				tmpU1B = (padapter->mppriv.antenna_tx >> Idx) & 1;
				if (tmpU1B)
					pMptCtx->PMacTxInfo.Ntx++;
			}

			_rtw_memset(pMptCtx->PMacTxInfo.MacAddress, 0xFF, ETH_ALEN);

			PMAC_Get_Pkt_Param(&pMptCtx->PMacTxInfo, &pMptCtx->PMacPktInfo);

			if (MPT_IS_CCK_RATE(pMptCtx->PMacTxInfo.TX_RATE))

				CCK_generator(&pMptCtx->PMacTxInfo, &pMptCtx->PMacPktInfo);
			else {
				PMAC_Nsym_generator(&pMptCtx->PMacTxInfo, &pMptCtx->PMacPktInfo);
				/* 24 BIT*/
				L_SIG_generator(pMptCtx->PMacPktInfo.N_sym, &pMptCtx->PMacTxInfo, &pMptCtx->PMacPktInfo);
			}
			/*	48BIT*/
			if (MPT_IS_HT_RATE(pMptCtx->PMacTxInfo.TX_RATE))
				HT_SIG_generator(&pMptCtx->PMacTxInfo, &pMptCtx->PMacPktInfo);
			else if (MPT_IS_VHT_RATE(pMptCtx->PMacTxInfo.TX_RATE)) {
				/*	48BIT*/
				VHT_SIG_A_generator(&pMptCtx->PMacTxInfo, &pMptCtx->PMacPktInfo);

				/*	26/27/29 BIT  & CRC 8 BIT*/
				VHT_SIG_B_generator(&pMptCtx->PMacTxInfo);

				/* 32 BIT*/
				VHT_Delimiter_generator(&pMptCtx->PMacTxInfo);
			}

			mpt_ProSetPMacTx(padapter);

		} else if (strncmp(extra, "pmact,mode=", 11) == 0) {
			int txmode = 0;

			if (sscanf(extra, "pmact,mode=%d", &txmode) > 0) {
				if (txmode == 1) {
					pMptCtx->HWTxmode = CONTINUOUS_TX;
					snprintf(extra, RTW_EXTRA_MAX_LEN, "\t Config HW Tx mode = CONTINUOUS_TX\n");
				} else if (txmode == 2) {
					pMptCtx->HWTxmode = OFDM_Single_Tone_TX;
					snprintf(extra, RTW_EXTRA_MAX_LEN, "\t Config HW Tx mode = OFDM_Single_Tone_TX\n");
				} else {
					pMptCtx->HWTxmode = PACKETS_TX;
					snprintf(extra, RTW_EXTRA_MAX_LEN, "\t Config HW Tx mode = PACKETS_TX\n");
				}
			} else {
				pMptCtx->HWTxmode = PACKETS_TX;
				snprintf(extra, RTW_EXTRA_MAX_LEN, "\t Config HW Tx mode=\n 0 = PACKETS_TX\n 1 = CONTINUOUS_TX\n 2 = OFDM_Single_Tone_TX");
			}
		} else if (strncmp(extra, "pmact,", 6) == 0) {
			int PacketPeriod = 0, PacketLength = 0, PacketCout = 0;
			int bldpc = 0, bstbc = 0;

			if (sscanf(extra, "pmact,period=%d", &PacketPeriod) > 0) {
				padapter->mppriv.pktInterval = PacketPeriod;
				RTW_INFO("PacketPeriod=%d\n", padapter->mppriv.pktInterval);
				snprintf(extra, RTW_EXTRA_MAX_LEN, "PacketPeriod [1~255]= %d\n", padapter->mppriv.pktInterval);

			} else if (sscanf(extra, "pmact,length=%d", &PacketLength) > 0) {
				padapter->mppriv.pktLength = PacketLength;
				RTW_INFO("PacketPeriod=%d\n", padapter->mppriv.pktLength);
				snprintf(extra, RTW_EXTRA_MAX_LEN, "PacketLength[~65535]=%d\n", padapter->mppriv.pktLength);

			} else if (sscanf(extra, "pmact,count=%d", &PacketCout) > 0) {
				padapter->mppriv.tx_pktcount = PacketCout;
				RTW_INFO("Packet Cout =%d\n", padapter->mppriv.tx_pktcount);
				snprintf(extra, RTW_EXTRA_MAX_LEN, "Packet Cout =%d\n", padapter->mppriv.tx_pktcount);

			} else if (sscanf(extra, "pmact,ldpc=%d", &bldpc) > 0) {
				pMptCtx->bldpc = bldpc;
				RTW_INFO("Set LDPC =%d\n", pMptCtx->bldpc);
				snprintf(extra, RTW_EXTRA_MAX_LEN, "Set LDPC =%d\n", pMptCtx->bldpc);

			} else if (sscanf(extra, "pmact,stbc=%d", &bstbc) > 0) {
				pMptCtx->bstbc = bstbc;
				RTW_INFO("Set STBC =%d\n", pMptCtx->bstbc);
				snprintf(extra, RTW_EXTRA_MAX_LEN, "Set STBC =%d\n", pMptCtx->bstbc);
			} else
				snprintf(extra, RTW_EXTRA_MAX_LEN, "\n period={1~255}\n length={1000~65535}\n count={0~}\n ldpc={0/1}\n stbc={0/1}");

		}

		wrqu->data.length = strlen(extra);
		return 0;
#endif
	} else {

		if (sscanf(extra, "ch=%d,bw=%d,rate=%d,pwr=%d,ant=%d,tx=%d", &channel, &bandwidth, &rate, &txpower, &ant, &txmode) < 6) {
			RTW_INFO("Invalid format [ch=%d,bw=%d,rate=%d,pwr=%d,ant=%d,tx=%d]\n", channel, bandwidth, rate, txpower, ant, txmode);
			_rtw_memset(extra, 0, wrqu->data.length);
			pextra += sprintf(pextra, "\n Please input correct format as bleow:\n");
			pextra += sprintf(pextra, "\t ch=%d,bw=%d,rate=%d,pwr=%d,ant=%d,tx=%d\n", channel, bandwidth, rate, txpower, ant, txmode);
			pextra += sprintf(pextra, "\n [ ch : BGN = <1~14> , A or AC = <36~165> ]");
			pextra += sprintf(pextra, "\n [ bw : Bandwidth: 0 = 20M, 1 = 40M, 2 = 80M ]");
			pextra += sprintf(pextra, "\n [ rate :	CCK: 1 2 5.5 11M X 2 = < 2 4 11 22 >]");
			pextra += sprintf(pextra, "\n [		OFDM: 6 9 12 18 24 36 48 54M X 2 = < 12 18 24 36 48 72 96 108>");
			pextra += sprintf(pextra, "\n [		HT 1S2SS MCS0 ~ MCS15 : < [MCS0]=128 ~ [MCS7]=135 ~ [MCS15]=143 >");
			pextra += sprintf(pextra, "\n [		HT 3SS MCS16 ~ MCS32 : < [MCS16]=144 ~ [MCS23]=151 ~ [MCS32]=159 >");
			pextra += sprintf(pextra, "\n [		VHT 1SS MCS0 ~ MCS9 : < [MCS0]=160 ~ [MCS9]=169 >");
			pextra += sprintf(pextra, "\n [ txpower : 1~63 power index");
			pextra += sprintf(pextra, "\n [ ant : <A = 1, B = 2, C = 4, D = 8> ,2T ex: AB=3 BC=6 CD=12");
			pextra += sprintf(pextra, "\n [ txmode : < 0 = CONTINUOUS_TX, 1 = PACKET_TX, 2 = SINGLE_TONE_TX, 3 = CARRIER_SUPPRISSION_TX, 4 = SINGLE_CARRIER_TX>\n");
			wrqu->data.length = strlen(extra);
			return status;

		} else {
			char *pextra = extra;
			RTW_INFO("Got format [ch=%d,bw=%d,rate=%d,pwr=%d,ant=%d,tx=%d]\n", channel, bandwidth, rate, txpower, ant, txmode);
			_rtw_memset(extra, 0, wrqu->data.length);
			snprintf(extra, RTW_EXTRA_MAX_LEN, "Change Current channel %d to channel %d", padapter->mppriv.channel , channel);
			padapter->mppriv.channel = channel;
			SetChannel(padapter);
			pHalData->current_channel = channel;

			if (bandwidth == 1)
				bandwidth = CHANNEL_WIDTH_40;
			else if (bandwidth == 2)
				bandwidth = CHANNEL_WIDTH_80;
			pextra = extra + strlen(pextra);
			pextra += sprintf(pextra, "\nChange Current Bandwidth %d to Bandwidth %d", padapter->mppriv.bandwidth, bandwidth);
			padapter->mppriv.bandwidth = (u8)bandwidth;
			padapter->mppriv.preamble = sg;
			SetBandwidth(padapter);
			pHalData->current_channel_bw = bandwidth;

			pextra += sprintf(pextra, "\nSet power level :%d", txpower);
			padapter->mppriv.txpoweridx = (u8)txpower;
			pMptCtx->TxPwrLevel[RF_PATH_A] = (u8)txpower;
			pMptCtx->TxPwrLevel[RF_PATH_B] = (u8)txpower;
			pMptCtx->TxPwrLevel[RF_PATH_C] = (u8)txpower;
			pMptCtx->TxPwrLevel[RF_PATH_D]  = (u8)txpower;
			SetTxPower(padapter);

			RTW_INFO("%s: bw=%d sg=%d\n", __func__, bandwidth, sg);

			if (rate <= 0x7f)
				rate = wifirate2_ratetbl_inx((u8)rate);
			else if (rate < 0xC8)
				rate = (rate - 0x80 + MPT_RATE_MCS0);
			/*HT  rate 0x80(MCS0)  ~ 0x8F(MCS15) ~ 0x9F(MCS31) 128~159
			VHT1SS~2SS rate 0xA0 (VHT1SS_MCS0 44) ~ 0xB3 (VHT2SS_MCS9 #63) 160~179
			VHT rate 0xB4 (VHT3SS_MCS0 64) ~ 0xC7 (VHT2SS_MCS9 #83) 180~199
			else
			VHT rate 0x90(VHT1SS_MCS0) ~ 0x99(VHT1SS_MCS9) 144~153
			rate =(rate - MPT_RATE_VHT1SS_MCS0);
			*/
			RTW_INFO("%s: rate index=%d\n", __func__, rate);
			if (rate >= MPT_RATE_LAST)
				return -EINVAL;
			pextra += sprintf(pextra, "\nSet data rate to %d index %d", padapter->mppriv.rateidx, rate);

			padapter->mppriv.rateidx = rate;
			pMptCtx->mpt_rate_index = rate;
			SetDataRate(padapter);

			pextra += sprintf(pextra, "\nSet Antenna Path :%d", ant);
			switch (ant) {
			case 1:
				antenna = ANTENNA_A;
				break;
			case 2:
				antenna = ANTENNA_B;
				break;
			case 4:
				antenna = ANTENNA_C;
				break;
			case 8:
				antenna = ANTENNA_D;
				break;
			case 3:
				antenna = ANTENNA_AB;
				break;
			case 5:
				antenna = ANTENNA_AC;
				break;
			case 9:
				antenna = ANTENNA_AD;
				break;
			case 6:
				antenna = ANTENNA_BC;
				break;
			case 10:
				antenna = ANTENNA_BD;
				break;
			case 12:
				antenna = ANTENNA_CD;
				break;
			case 7:
				antenna = ANTENNA_ABC;
				break;
			case 14:
				antenna = ANTENNA_BCD;
				break;
			case 11:
				antenna = ANTENNA_ABD;
				break;
			case 15:
				antenna = ANTENNA_ABCD;
				break;
			}
			RTW_INFO("%s: antenna=0x%x\n", __func__, antenna);
			padapter->mppriv.antenna_tx = antenna;
			padapter->mppriv.antenna_rx = antenna;
			pHalData->antenna_tx_path = antenna;
			SetAntenna(padapter);

			if (txmode == 0)
				pmp_priv->mode = MP_CONTINUOUS_TX;
			else if (txmode == 1) {
				pmp_priv->mode = MP_PACKET_TX;
				pmp_priv->tx.count = count;
			} else if (txmode == 2)
				pmp_priv->mode = MP_SINGLE_TONE_TX;
			else if (txmode == 3)
				pmp_priv->mode = MP_CARRIER_SUPPRISSION_TX;
			else if (txmode == 4)
				pmp_priv->mode = MP_SINGLE_CARRIER_TX;

			status = rtw_mp_pretx_proc(padapter, bStartTest, extra);
		}

	}

	wrqu->data.length = strlen(extra);
	return status;
}


int rtw_mp_rx(struct net_device *dev,
	      struct iw_request_info *info,
	      union iwreq_data *wrqu, char *extra)
{
	PADAPTER padapter = rtw_netdev_priv(dev);
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(padapter);
	struct mp_priv *pmp_priv = &padapter->mppriv;
	char *pextra = extra;
	u32 bandwidth = 0, sg = 0, channel = 6, ant = 0;
	u16 antenna = 0;
	u8 bStartRx = 0;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

#ifdef CONFIG_CONCURRENT_MODE
	if (!is_primary_adapter(padapter)) {
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Error: MP mode can't support Virtual Adapter, Please to use main Adapter.\n");
		wrqu->data.length = strlen(extra);
		return 0;
	}
#endif

	if (strncmp(extra, "stop", 4) == 0) {
		_rtw_memset(extra, 0, wrqu->data.length);
		SetPacketRx(padapter, bStartRx, _FALSE);
		pmp_priv->bmac_filter = _FALSE;
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Received packet OK:%d CRC error:%d ,Filter out:%d", padapter->mppriv.rx_pktcount, padapter->mppriv.rx_crcerrpktcount, padapter->mppriv.rx_pktcount_filter_out);
		wrqu->data.length = strlen(extra);
		return 0;

	} else if (sscanf(extra, "ch=%d,bw=%d,ant=%d", &channel, &bandwidth, &ant) < 3) {
		RTW_INFO("Invalid format [ch=%d,bw=%d,ant=%d]\n", channel, bandwidth, ant);
		_rtw_memset(extra, 0, wrqu->data.length);
		pextra += sprintf(pextra, "\n Please input correct format as bleow:\n");
		pextra += sprintf(pextra, "\t ch=%d,bw=%d,ant=%d\n", channel, bandwidth, ant);
		pextra += sprintf(pextra, "\n [ ch : BGN = <1~14> , A or AC = <36~165> ]");
		pextra += sprintf(pextra, "\n [ bw : Bandwidth: 0 = 20M, 1 = 40M, 2 = 80M ]");
		pextra += sprintf(pextra, "\n [ ant : <A = 1, B = 2, C = 4, D = 8> ,2T ex: AB=3 BC=6 CD=12");
		wrqu->data.length = strlen(extra);
		return 0;

	} else {
		char *pextra = extra;
		bStartRx = 1;
		RTW_INFO("Got format [ch=%d,bw=%d,ant=%d]\n", channel, bandwidth, ant);
		_rtw_memset(extra, 0, wrqu->data.length);
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Change Current channel %d to channel %d", padapter->mppriv.channel , channel);
		padapter->mppriv.channel = channel;
		SetChannel(padapter);
		pHalData->current_channel = channel;

		if (bandwidth == 1)
			bandwidth = CHANNEL_WIDTH_40;
		else if (bandwidth == 2)
			bandwidth = CHANNEL_WIDTH_80;
		pextra = extra + strlen(extra);
		pextra += sprintf(pextra, "\nChange Current Bandwidth %d to Bandwidth %d", padapter->mppriv.bandwidth, bandwidth);
		padapter->mppriv.bandwidth = (u8)bandwidth;
		padapter->mppriv.preamble = sg;
		SetBandwidth(padapter);
		pHalData->current_channel_bw = bandwidth;

		pextra += sprintf(pextra, "\nSet Antenna Path :%d", ant);
		switch (ant) {
		case 1:
			antenna = ANTENNA_A;
			break;
		case 2:
			antenna = ANTENNA_B;
			break;
		case 4:
			antenna = ANTENNA_C;
			break;
		case 8:
			antenna = ANTENNA_D;
			break;
		case 3:
			antenna = ANTENNA_AB;
			break;
		case 5:
			antenna = ANTENNA_AC;
			break;
		case 9:
			antenna = ANTENNA_AD;
			break;
		case 6:
			antenna = ANTENNA_BC;
			break;
		case 10:
			antenna = ANTENNA_BD;
			break;
		case 12:
			antenna = ANTENNA_CD;
			break;
		case 7:
			antenna = ANTENNA_ABC;
			break;
		case 14:
			antenna = ANTENNA_BCD;
			break;
		case 11:
			antenna = ANTENNA_ABD;
			break;
		case 15:
			antenna = ANTENNA_ABCD;
			break;
		}
		RTW_INFO("%s: antenna=0x%x\n", __func__, antenna);
		padapter->mppriv.antenna_tx = antenna;
		padapter->mppriv.antenna_rx = antenna;
		pHalData->antenna_tx_path = antenna;
		SetAntenna(padapter);

		strcat(extra, "\nstart Rx");
		SetPacketRx(padapter, bStartRx, _FALSE);
	}
	wrqu->data.length = strlen(extra);
	return 0;
}


int rtw_mp_hwtx(struct net_device *dev,
		struct iw_request_info *info,
		union iwreq_data *wrqu, char *extra)
{
	PADAPTER padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv = &padapter->mppriv;
	PMPT_CONTEXT		pMptCtx = &(padapter->mppriv.mpt_ctx);
	char *pch;

#if defined(CONFIG_RTL8814A) || defined(CONFIG_RTL8821B) || defined(CONFIG_RTL8822B) \
	|| defined(CONFIG_RTL8821C) || defined(CONFIG_RTL8822C) || defined(CONFIG_RTL8723F) \
	|| defined(CONFIG_RTL8822E)
	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;
	*(extra + wrqu->data.length) = '\0';

	_rtw_memset(&pMptCtx->PMacTxInfo, 0, sizeof(RT_PMAC_TX_INFO));
	_rtw_memcpy((void *)&pMptCtx->PMacTxInfo, (void *)extra, sizeof(RT_PMAC_TX_INFO));
	_rtw_memset(extra, 0, wrqu->data.length);
	pch = extra;

	if (pMptCtx->PMacTxInfo.bEnPMacTx == 1 && pmp_priv->mode != MP_ON) {
		pch += snprintf(pch, RTW_EXTRA_MAX_LEN, "MP Tx Running, Please Set PMac Tx Mode Stop\n");
		RTW_INFO("Error !!! MP Tx Running, Please Set PMac Tx Mode Stop\n");
	} else {
		RTW_INFO("To set MAC Tx mode\n");
		if (mpt_ProSetPMacTx(padapter))
			pch += snprintf(pch, RTW_EXTRA_MAX_LEN, "Set PMac Tx Mode OK\n");
		else
			pch += snprintf(pch, RTW_EXTRA_MAX_LEN, "Set PMac Tx Mode Error\n");
	}
	wrqu->data.length = strlen(extra);
#endif
	return 0;

}

int rtw_mp_pwrlmt(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra)
{
	PADAPTER padapter = rtw_netdev_priv(dev);
	struct registry_priv  *registry_par = &padapter->registrypriv;
	char *pch;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	*(extra + wrqu->data.length) = '\0';
	pch = extra;

#if CONFIG_TXPWR_LIMIT
	if (strncmp(extra, "off", 3) == 0 && strlen(extra) < 4) {
		padapter->registrypriv.RegEnableTxPowerLimit = 0;
		pch += snprintf(pch, RTW_EXTRA_MAX_LEN, "Turn off Power Limit\n");

	} else if (strncmp(extra, "on", 2) == 0 && strlen(extra) < 3) {
		padapter->registrypriv.RegEnableTxPowerLimit = 1;
		pch += snprintf(pch, RTW_EXTRA_MAX_LEN, "Turn on Power Limit\n");

	} else
#endif
		pch += sprintf(pch, "Get Power Limit Status:%s\n", (registry_par->RegEnableTxPowerLimit == 1) ? "ON" : "OFF");


	wrqu->data.length = strlen(extra);
	return 0;
}

int rtw_mp_pwrbyrate(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra)
{
	PADAPTER padapter = rtw_netdev_priv(dev);

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	*(extra + wrqu->data.length) = '\0';
	if (strncmp(extra, "off", 3) == 0 && strlen(extra) < 4) {
		padapter->registrypriv.RegEnableTxPowerByRate = 0;
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Turn off Tx Power by Rate\n");

	} else if (strncmp(extra, "on", 2) == 0 && strlen(extra) < 3) {
		padapter->registrypriv.RegEnableTxPowerByRate = 1;
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Turn On Tx Power by Rate\n");

	} else {
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Get Power by Rate Status:%s\n", (padapter->registrypriv.RegEnableTxPowerByRate == 1) ? "ON" : "OFF");
	}

	wrqu->data.length = strlen(extra);
	return 0;
}


int rtw_mp_dpk_track(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra)
{
	PADAPTER padapter = rtw_netdev_priv(dev);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct dm_struct		*pDM_Odm = &pHalData->odmpriv;
	char *pch;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	*(extra + wrqu->data.length) = '\0';
	pch = extra;

	if (strncmp(extra, "off", 3) == 0 ) {
		halrf_set_dpk_track(pDM_Odm, FALSE);
		pch += sprintf(pch, "set dpk track off\n");

	} else if (strncmp(extra, "on", 2) == 0) {
		halrf_set_dpk_track(pDM_Odm, TRUE);
		pch += sprintf(pch, "set dpk track on\n");
	}

	wrqu->data.length = strlen(extra);
	return 0;
}


int rtw_bt_efuse_mask_file(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra)
{
	char *rtw_efuse_mask_file_path;
	u8	*pch;
	char	*ptmp, tmp;
	u32 retcnt;
	PADAPTER padapter = rtw_netdev_priv(dev);

	_rtw_memset(btmaskfileBuffer, 0x00, sizeof(btmaskfileBuffer));

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	*(extra + wrqu->data.length) = '\0';
	ptmp = extra;

	if (strncmp(extra, "data,", 5) == 0) {
		u8	count = 0;
		u8	i = 0;

		pch = strsep(&ptmp, ",");

		if ((pch == NULL) || (strlen(pch) == 0)) {
			RTW_INFO("%s: parameter error(no cmd)!\n", __func__);
			return -EFAULT;
		}

		do {
			pch = strsep(&ptmp, ":");
			if ((pch == NULL) || (strlen(pch) == 0))
				break;
			if (strlen(pch) != 2
				|| IsHexDigit(*pch) == _FALSE
				|| IsHexDigit(*(pch + 1)) == _FALSE
				|| sscanf(pch, "%hhx", &tmp) != 1
			) {
				RTW_INFO("%s: invalid 8-bit hex! input format: data,01:23:45:67:89:ab:cd:ef...\n", __func__);
				return -EFAULT;
			}
			btmaskfileBuffer[count++] = tmp;

		 } while (count < 64);

		_rtw_memset(extra, '\0' , strlen(extra));

		for (i = 0; i < count; i++)
			ptmp += sprintf(ptmp, "%02x:", btmaskfileBuffer[i]);

		padapter->registrypriv.bBTFileMaskEfuse = _TRUE;

		ptmp += sprintf(ptmp, "\nLoad BT Efuse Mask data %d hex ok\n", count);
		wrqu->data.length = strlen(extra);
		return 0;
	}
	rtw_efuse_mask_file_path = extra;

	if (rtw_is_file_readable(rtw_efuse_mask_file_path) == _TRUE) {
		RTW_INFO("%s do rtw_is_file_readable = %s! ,sizeof BT maskfileBuffer %zu\n", __func__, rtw_efuse_mask_file_path, sizeof(btmaskfileBuffer));
		retcnt = rtw_efuse_file_read(padapter, rtw_efuse_mask_file_path, btmaskfileBuffer, sizeof(btmaskfileBuffer));
		_rtw_memset(extra, '\0' , strlen(extra));
		if (retcnt > 0) {
			padapter->registrypriv.bBTFileMaskEfuse = _TRUE;
			ptmp += sprintf(ptmp, "BT efuse mask file read OK\n");
		} else {
			padapter->registrypriv.bBTFileMaskEfuse = _FALSE;
			ptmp += sprintf(ptmp, "read BT efuse mask file FAIL\n");
			RTW_INFO("%s rtw_efuse_file_read BT mask fail!\n", __func__);
		}
	} else {
		padapter->registrypriv.bBTFileMaskEfuse = _FALSE;
		ptmp += sprintf(ptmp, "BT efuse mask file readable FAIL\n");
		RTW_INFO("%s rtw_is_file_readable BT Mask file fail!\n", __func__);
	}
	wrqu->data.length = strlen(extra);
	return 0;
}


int rtw_efuse_mask_file(struct net_device *dev,
			struct iw_request_info *info,
			union iwreq_data *wrqu, char *extra)
{
	char *rtw_efuse_mask_file_path;
	u32 retcnt;
	PADAPTER padapter = rtw_netdev_priv(dev);
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(padapter);
	PEFUSE_HAL pEfuseHal = &pHalData->EfuseHal;

	_rtw_memset(maskfileBuffer, 0x00, sizeof(maskfileBuffer));

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	*(extra + wrqu->data.length) = '\0';
	if (strncmp(extra, "off", 3) == 0 && strlen(extra) < 4) {
		padapter->registrypriv.boffefusemask = 1;
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Turn off Efuse Mask\n");
		wrqu->data.length = strlen(extra);
		return 0;
	}
	if (strncmp(extra, "on", 2) == 0 && strlen(extra) < 3) {
		padapter->registrypriv.boffefusemask = 0;
		snprintf(extra, RTW_EXTRA_MAX_LEN, "Turn on Efuse Mask\n");
		wrqu->data.length = strlen(extra);
		return 0;
	}
	if (strncmp(extra, "data,", 5) == 0) {
		u8	*pch;
		char	*ptmp, tmp;
		u8	count = 0;
		u8	i = 0;
		u8	tmp_len = 0, total_tmp_len = 0;

		ptmp = extra;
		pch = strsep(&ptmp, ",");

		if ((pch == NULL) || (strlen(pch) == 0)) {
			RTW_INFO("%s: parameter error(no cmd)!\n", __func__);
			return -EFAULT;
		}

		do {
			pch = strsep(&ptmp, ":");
			if ((pch == NULL) || (strlen(pch) == 0))
				break;
			if (strlen(pch) != 2
				|| IsHexDigit(*pch) == _FALSE
				|| IsHexDigit(*(pch + 1)) == _FALSE
				|| sscanf(pch, "%hhx", &tmp) != 1
			) {
				RTW_INFO("%s: invalid 8-bit hex! input format: data,01:23:45:67:89:ab:cd:ef...\n", __func__);
				return -EFAULT;
			}
			maskfileBuffer[count++] = tmp;

		} while (count < 64);

		_rtw_memset(extra, '\0' , strlen(extra));
		ptmp = extra;

		for (i = 0; i < count; i++){
			tmp_len = sprintf(ptmp, "%02x:", maskfileBuffer[i]);
			ptmp += tmp_len;
			total_tmp_len += tmp_len;
		}

		padapter->registrypriv.bFileMaskEfuse = _TRUE;

		snprintf(ptmp, RTW_EXTRA_MAX_LEN - total_tmp_len, "\nLoad Efuse Mask data %d hex ok\n", count);
		wrqu->data.length = strlen(extra);
		return 0;
	}
	rtw_efuse_mask_file_path = extra;

	if (rtw_is_file_readable(rtw_efuse_mask_file_path) == _TRUE) {
		RTW_INFO("%s do rtw_efuse_mask_file_read = %s! ,sizeof maskfileBuffer %zu\n", __func__, rtw_efuse_mask_file_path, sizeof(maskfileBuffer));
		retcnt = rtw_efuse_file_read(padapter, rtw_efuse_mask_file_path, maskfileBuffer, sizeof(maskfileBuffer));
		if (retcnt > 0) {
			pEfuseHal->EfuseMaskUsedBytes = retcnt;
			RTW_INFO("%s ret used cnt:%d!\n", __func__, pEfuseHal->EfuseMaskUsedBytes);
			padapter->registrypriv.bFileMaskEfuse = _TRUE;
			snprintf(extra, RTW_EXTRA_MAX_LEN, "efuse mask file read OK\n");
		} else {
			padapter->registrypriv.bFileMaskEfuse = _FALSE;
			snprintf(extra, RTW_EXTRA_MAX_LEN, "read efuse mask file FAIL\n");
			RTW_INFO("%s rtw_efuse_file_read mask fail!\n", __func__);
		}
	} else {
		padapter->registrypriv.bFileMaskEfuse = _FALSE;
		snprintf(extra, RTW_EXTRA_MAX_LEN, "efuse mask file readable FAIL\n");
		RTW_INFO("%s rtw_is_file_readable fail!\n", __func__);
	}
	wrqu->data.length = strlen(extra);
	return 0;
}


int rtw_efuse_file_map(struct net_device *dev,
		       struct iw_request_info *info,
		       union iwreq_data *wrqu, char *extra)
{
	char *rtw_efuse_file_map_path;
	u32 retcnt;
	PEFUSE_HAL pEfuseHal;
	PADAPTER padapter = rtw_netdev_priv(dev);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct mp_priv *pmp_priv = &padapter->mppriv;

	pEfuseHal = &pHalData->EfuseHal;
	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;
	extra[wrqu->data.length] = '\0';

	rtw_efuse_file_map_path = extra;

	_rtw_memset(pEfuseHal->fakeEfuseModifiedMap, 0xFF, EFUSE_MAX_MAP_LEN);

	if (rtw_is_file_readable(rtw_efuse_file_map_path) == _TRUE) {
		RTW_INFO("%s do rtw_efuse_mask_file_read = %s!\n", __func__, rtw_efuse_file_map_path);
		retcnt = rtw_efuse_file_read(padapter, rtw_efuse_file_map_path,
				pEfuseHal->fakeEfuseModifiedMap, sizeof(pEfuseHal->fakeEfuseModifiedMap));

		if (retcnt > 0) {
			pEfuseHal->fakeEfuseUsedBytes = retcnt;
			RTW_INFO("%s  Efuse Used Bytes = %d!\n", __func__, pEfuseHal->fakeEfuseUsedBytes);
			pmp_priv->bloadefusemap = _TRUE;
			snprintf(extra, RTW_EXTRA_MAX_LEN, "%s", "efuse file file_read OK\n");
		} else {
			pmp_priv->bloadefusemap = _FALSE;
			snprintf(extra, RTW_EXTRA_MAX_LEN, "efuse file file_read FAIL\n");
		}
	} else {
		snprintf(extra, RTW_EXTRA_MAX_LEN, "%s", "efuse file readable FAIL\n");
		RTW_INFO("%s rtw_is_file_readable fail!\n", __func__);
	}
	wrqu->data.length = strlen(extra);
	return 0;
}


#if !defined(CONFIG_RTW_ANDROID_GKI)
int rtw_efuse_file_map_store(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra)
{
	char *rtw_efuse_file_map_path;
	u8 Status;
	u16 mapLen;
	PEFUSE_HAL pEfuseHal;
	PADAPTER padapter = rtw_netdev_priv(dev);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct mp_priv *pmp_priv = &padapter->mppriv;

	pEfuseHal = &pHalData->EfuseHal;
	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	rtw_efuse_file_map_path = extra;
	RTW_INFO("%s rtw_is_file_readable! %s\n", __func__, rtw_efuse_file_map_path);

	EFUSE_GetEfuseDefinition(padapter, EFUSE_WIFI, TYPE_EFUSE_MAP_LEN , (void *)&mapLen, _FALSE);

	if (mapLen != 0) {
		RTW_INFO("%s, efuse store path = %s! mapLen = %d\n", __func__, rtw_efuse_file_map_path, mapLen);
		Status = rtw_efuse_file_store(padapter, rtw_efuse_file_map_path, pEfuseHal->fakeEfuseModifiedMap, mapLen);
		if (Status) {
			snprintf(extra, RTW_EXTRA_MAX_LEN, "efuse file restore OK\n");
		} else {
			snprintf(extra, RTW_EXTRA_MAX_LEN, "efuse file restore FAIL\n");
		}
	} else {
		snprintf(extra, RTW_EXTRA_MAX_LEN, "efuse file readable FAIL\n");
		RTW_INFO("%s rtw_is_file_readable fail! map Len %d\n", __func__, mapLen);
	}

	wrqu->data.length = strlen(extra);
	return 0;
}
#endif /* !defined(CONFIG_RTW_ANDROID_GKI) */

int rtw_bt_efuse_file_map(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra)
{
	char *rtw_efuse_file_map_path;
	u32 retcnt;
	PEFUSE_HAL pEfuseHal;
	PADAPTER padapter = rtw_netdev_priv(dev);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct mp_priv *pmp_priv = &padapter->mppriv;

	pEfuseHal = &pHalData->EfuseHal;
	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;
	extra[wrqu->data.length] = '\0';

	rtw_efuse_file_map_path = extra;

	_rtw_memset(pEfuseHal->fakeBTEfuseModifiedMap, 0xFF, EFUSE_BT_MAX_MAP_LEN);

	if (rtw_is_file_readable(rtw_efuse_file_map_path) == _TRUE) {
		RTW_INFO("%s do rtw_efuse_mask_file_read = %s!\n", __func__, rtw_efuse_file_map_path);
		retcnt = rtw_efuse_file_read(padapter, rtw_efuse_file_map_path, pEfuseHal->fakeBTEfuseModifiedMap, sizeof(pEfuseHal->fakeBTEfuseModifiedMap));
		if (retcnt > 0) {
			pmp_priv->bloadBTefusemap = _TRUE;
			snprintf(extra, RTW_EXTRA_MAX_LEN, "BT efuse file file_read OK\n");
		} else {
			pmp_priv->bloadBTefusemap = _FALSE;
			snprintf(extra, RTW_EXTRA_MAX_LEN, "BT efuse file file_read FAIL\n");
		}
	} else {
		snprintf(extra, RTW_EXTRA_MAX_LEN, "BT efuse file readable FAIL\n");
		RTW_INFO("%s rtw_is_file_readable fail!\n", __func__);
	}
	wrqu->data.length = strlen(extra);
	return 0;
}


static inline void dump_buf(u8 *buf, u32 len)
{
	u32 i;

	RTW_INFO("-----------------Len %d----------------\n", len);
	for (i = 0; i < len; i++)
		RTW_INFO("%2.2x-", *(buf + i));
	RTW_INFO("\n");
}

int rtw_mp_link(struct net_device *dev,
			struct iw_request_info *info,
			struct iw_point *wrqu, char *extra)
{
	PADAPTER padapter = rtw_netdev_priv(dev);
	struct mp_priv *pmp_priv;
	char *input = NULL;
	int		bgetrxdata = 0, btxdata = 0, bsetbt = 0;
	u8 err = 0;
	u32 i = 0, datalen = 0,jj, kk, waittime = 0;
	u16 val = 0x00, ret = 0;
	char *pextra = NULL;
	u8 *setdata = NULL;
	char *pch, *ptmp, *token, *tmp[4] = {0x00, 0x00, 0x00};

	pmp_priv = &padapter->mppriv;

	if (rtw_do_mp_iwdata_len_chk(__func__, wrqu->length))
		return -EFAULT;

	input = (char *)rtw_zmalloc(RTW_IWD_MAX_LEN);
	if (!input)
		return -ENOMEM;
	if (copy_from_user(input, wrqu->pointer, wrqu->length)) {
		err = -EFAULT;
		goto exit;
	}
	_rtw_memset(extra, 0, wrqu->length);

	RTW_INFO("%s: in=%s\n", __func__, input);

	bgetrxdata =  (strncmp(input, "rxdata", 6) == 0) ? 1 : 0; /* strncmp TRUE is 0*/
	btxdata =  (strncmp(input, "txdata", 6) == 0) ? 1 : 0; /* strncmp TRUE is 0*/
	bsetbt =  (strncmp(input, "setbt", 5) == 0) ? 1 : 0; /* strncmp TRUE is 0*/

	if (bgetrxdata) {
		RTW_INFO("%s: in= 1 \n", __func__);
		if (pmp_priv->mplink_brx == _TRUE) {
			pch = extra;
				while (waittime < 100 && pmp_priv->mplink_brx == _FALSE) {
						if (pmp_priv->mplink_brx == _FALSE)
							rtw_msleep_os(10);
						else
							break;
						waittime++;
				}
				if (pmp_priv->mplink_brx == _TRUE) {
					pch += sprintf(pch, "\n");

					for (i = 0; i < pmp_priv->mplink_rx_len; i ++) {
						pch += sprintf(pch, "%02x:", pmp_priv->mplink_buf[i]);
					}
					_rtw_memset(pmp_priv->mplink_buf, '\0' , sizeof(pmp_priv->mplink_buf));
					pmp_priv->mplink_brx = _FALSE;
				}
		}
	} else if (btxdata) {
		struct pkt_attrib *pattrib;

		pch = input;
		setdata = rtw_zmalloc(1024);
		if (setdata == NULL) {
			err = -ENOMEM;
			goto exit;
		}

		i = 0;
		while ((token = strsep(&pch, ",")) != NULL) {
			if (i > 2)
				break;
			tmp[i] = token;
			i++;
		}

		/* tmp[0],[1],[2] */
		/* txdata,00e04c871200........... */
		if (strcmp(tmp[0], "txdata") == 0) {
			if (tmp[1] == NULL) {
				err = -EINVAL;
				goto exit;
			}
		}

		datalen = strlen(tmp[1]);
		if (datalen % 2) {
			err = -EINVAL;
			goto exit;
		}
		datalen /= 2;
		if (datalen == 0) {
			err = -EINVAL;
			goto exit;
		}

		RTW_INFO("%s: data len=%d\n", __FUNCTION__, datalen);
		RTW_INFO("%s: tx data=%s\n", __FUNCTION__, tmp[1]);

		for (jj = 0, kk = 0; jj < datalen; jj++, kk += 2)
			setdata[jj] = key_2char2num(tmp[1][kk], tmp[1][kk + 1]);

		dump_buf(setdata, datalen);
		_rtw_memset(pmp_priv->mplink_buf, '\0' , sizeof(pmp_priv->mplink_buf));
		_rtw_memcpy(pmp_priv->mplink_buf, setdata, datalen);

		pattrib = &pmp_priv->tx.attrib;
		pattrib->pktlen = datalen;
		pmp_priv->tx.count = 1;
		pmp_priv->tx.stop = 0;
		pmp_priv->mplink_btx = _TRUE;
		SetPacketTx(padapter);
		pmp_priv->mode = MP_PACKET_TX;

	} else if (bsetbt) {

#ifdef CONFIG_BT_COEXIST
		pch = input;
		i = 0;

		while ((token = strsep(&pch, ",")) != NULL) {
			if (i > 3)
				break;
			tmp[i] = token;
			i++;
		}

		if (tmp[1] == NULL) {
			err = -EINVAL;
			goto exit;
		}

		if (strcmp(tmp[1], "scbd") == 0) {
			u16 org_val = 0x8002, pre_val, read_score_board_val;
			u8 state;

			pre_val = (rtw_read16(padapter,(0xaa))) & 0x7fff;

			if (tmp[2] != NULL) {
				state = simple_strtoul(tmp[2], &ptmp, 10);

				if (state)
						org_val = org_val | BIT6;
				else
						org_val = org_val & (~BIT6);

				if (org_val != pre_val) {
					pre_val = org_val;
					rtw_write16(padapter, 0xaa, org_val);
					RTW_INFO("%s,setbt scbd write org_val = 0x%x , pre_val = 0x%x\n", __func__, org_val, pre_val);
				} else {
					RTW_INFO("%s,setbt scbd org_val = 0x%x ,pre_val = 0x%x\n", __func__, org_val, pre_val);
				}
			} else {
					read_score_board_val = (rtw_read16(padapter,(0xaa))) & 0x7fff;
					RTW_INFO("%s,read_score_board_val = 0x%x\n", __func__, read_score_board_val);
			}
			goto exit;

		} else if (strcmp(tmp[1], "testmode") == 0) {

			if (tmp[2] == NULL) {
				err = -EINVAL;
				goto exit;
			}

			val = simple_strtoul(tmp[2], &ptmp, 16);
			RTW_INFO("get tmp, type  %s, val =0x%x!\n", tmp[1], val);

			if (tmp[2] != NULL) {
				_rtw_memset(extra, 0, wrqu->length);
				pch = extra;
				ret = rtw_btcoex_btset_testmode(padapter, val);
				if (!CHECK_STATUS_CODE_FROM_BT_MP_OPER_RET(ret, BT_STATUS_BT_OP_SUCCESS)) {
					RTW_INFO("%s: BT_OP fail = 0x%x!\n", __FUNCTION__, val);
					pch += sprintf(pch, "BT_OP fail  0x%x!\n", val);
				} else
					pch += sprintf(pch, "Set BT_OP 0x%x done!\n", val);
			}

		}
#endif /* CONFIG_BT_COEXIST */
	}

exit:
	if (setdata)
		rtw_mfree(setdata, 1024);
	if (input)
		rtw_mfree(input, RTW_IWD_MAX_LEN);
	wrqu->length = strlen(extra);
	return err;

}

#if defined(CONFIG_RTL8723B)
int rtw_mp_SetBT(struct net_device *dev,
		 struct iw_request_info *info,
		 union iwreq_data *wrqu, char *extra)
{
	PADAPTER padapter = rtw_netdev_priv(dev);
	struct hal_ops *pHalFunc = &padapter->hal_func;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);

	BT_REQ_CMD	BtReq;
	PMPT_CONTEXT	pMptCtx = &(padapter->mppriv.mpt_ctx);
	PBT_RSP_CMD	pBtRsp = (PBT_RSP_CMD)&pMptCtx->mptOutBuf[0];
	char	input[128];
	char *pch, *ptmp, *token, *tmp[2] = {0x00, 0x00};
	u8 setdata[100];
	u8 resetbt = 0x00;
	u8 tempval, BTStatus;
	u8 H2cSetbtmac[6];
	u8 u1H2CBtMpOperParm[4] = {0x01};
	int testmode = 1, ready = 1, trxparam = 1, setgen = 1, getgen = 1, testctrl = 1, testbt = 1, readtherm = 1, setbtmac = 1;
	u32 i = 0, ii = 0, jj = 0, kk = 0, cnts = 0, status = 0;
	PRT_MP_FIRMWARE pBTFirmware = NULL;

	if (copy_from_user(extra, wrqu->data.pointer, wrqu->data.length))
		return -EFAULT;

	*(extra + wrqu->data.length) = '\0';
	pch = extra;

	if (strlen(extra) < 1)
		return -EFAULT;

	RTW_INFO("%s:iwpriv in=%s\n", __func__, extra);
	ready = strncmp(extra, "ready", 5);
	testmode = strncmp(extra, "testmode", 8); /* strncmp TRUE is 0*/
	trxparam = strncmp(extra, "trxparam", 8);
	setgen = strncmp(extra, "setgen", 6);
	getgen = strncmp(extra, "getgen", 6);
	testctrl = strncmp(extra, "testctrl", 8);
	testbt = strncmp(extra, "testbt", 6);
	readtherm = strncmp(extra, "readtherm", 9);
	setbtmac = strncmp(extra, "setbtmac", 8);

	if (strncmp(extra, "dlbt", 4) == 0) {
		pHalData->LastHMEBoxNum = 0;
		pHalData->bBTFWReady = _FALSE;
		rtw_write8(padapter, 0xa3, 0x05);
		BTStatus = rtw_read8(padapter, 0xa0);
		RTW_INFO("%s: btwmap before read 0xa0 BT Status =0x%x\n", __func__, BTStatus);
		if (BTStatus != 0x04) {
			pch += sprintf(pch, "BT Status not Active DLFW FAIL\n");
			goto exit;
		}

		tempval = rtw_read8(padapter, 0x6B);
		tempval |= BIT7;
		rtw_write8(padapter, 0x6B, tempval);

		/* Attention!! Between 0x6A[14] and 0x6A[15] setting need 100us delay*/
		/* So don't write 0x6A[14]=1 and 0x6A[15]=0 together!*/
		rtw_usleep_os(100);
		/* disable BT power cut*/
		/* 0x6A[14] = 0*/
		tempval = rtw_read8(padapter, 0x6B);
		tempval &= ~BIT6;
		rtw_write8(padapter, 0x6B, tempval);
		rtw_usleep_os(100);
		MPT_PwrCtlDM(padapter, 0);
		rtw_write32(padapter, 0xcc, (rtw_read32(padapter, 0xcc) | 0x00000004));
		rtw_write32(padapter, 0x6b, (rtw_read32(padapter, 0x6b) & 0xFFFFFFEF));
		rtw_msleep_os(600);
		rtw_write32(padapter, 0x6b, (rtw_read32(padapter, 0x6b) | 0x00000010));
		rtw_write32(padapter, 0xcc, (rtw_read32(padapter, 0xcc) & 0xFFFFFFFB));
		rtw_msleep_os(1200);
		pBTFirmware = (PRT_MP_FIRMWARE)rtw_zmalloc(sizeof(RT_MP_FIRMWARE));
		if (pBTFirmware == NULL)
			goto exit;
		pHalData->bBTFWReady = _FALSE;
		FirmwareDownloadBT(padapter, pBTFirmware);
		if (pBTFirmware)
			rtw_mfree((u8 *)pBTFirmware, sizeof(RT_MP_FIRMWARE));

		RTW_INFO("Wait for FirmwareDownloadBT fw boot!\n");
		rtw_msleep_os(2000);
		_rtw_memset(extra, '\0', wrqu->data.length);
		BtReq.opCodeVer = 1;
		BtReq.OpCode = 0;
		BtReq.paraLength = 0;
		mptbt_BtControlProcess(padapter, &BtReq);
		rtw_msleep_os(100);

		RTW_INFO("FirmwareDownloadBT ready = 0x%x 0x%x", pMptCtx->mptOutBuf[4], pMptCtx->mptOutBuf[5]);
		if ((pMptCtx->mptOutBuf[4] == 0x00) && (pMptCtx->mptOutBuf[5] == 0x00)) {

			if (padapter->mppriv.bTxBufCkFail == _TRUE)
				pch += sprintf(pch, "check TxBuf Fail.\n");
			else
				pch += sprintf(pch, "download FW Fail.\n");
		} else {
			pch += sprintf(pch, "download FW OK.\n");
			goto exit;
		}
		goto exit;
	}
	if (strncmp(extra, "dlfw", 4) == 0) {
		pHalData->LastHMEBoxNum = 0;
		pHalData->bBTFWReady = _FALSE;
		rtw_write8(padapter, 0xa3, 0x05);
		BTStatus = rtw_read8(padapter, 0xa0);
		RTW_INFO("%s: btwmap before read 0xa0 BT Status =0x%x\n", __func__, BTStatus);
		if (BTStatus != 0x04) {
			pch += sprintf(pch, "BT Status not Active DLFW FAIL\n");
			goto exit;
		}

		tempval = rtw_read8(padapter, 0x6B);
		tempval |= BIT7;
		rtw_write8(padapter, 0x6B, tempval);

		/* Attention!! Between 0x6A[14] and 0x6A[15] setting need 100us delay*/
		/* So don't write 0x6A[14]=1 and 0x6A[15]=0 together!*/
		rtw_usleep_os(100);
		/* disable BT power cut*/
		/* 0x6A[14] = 0*/
		tempval = rtw_read8(padapter, 0x6B);
		tempval &= ~BIT6;
		rtw_write8(padapter, 0x6B, tempval);
		rtw_usleep_os(100);

		MPT_PwrCtlDM(padapter, 0);
		rtw_write32(padapter, 0xcc, (rtw_read32(padapter, 0xcc) | 0x00000004));
		rtw_write32(padapter, 0x6b, (rtw_read32(padapter, 0x6b) & 0xFFFFFFEF));
		rtw_msleep_os(600);
		rtw_write32(padapter, 0x6b, (rtw_read32(padapter, 0x6b) | 0x00000010));
		rtw_write32(padapter, 0xcc, (rtw_read32(padapter, 0xcc) & 0xFFFFFFFB));
		rtw_msleep_os(1200);

#if defined(CONFIG_PLATFORM_SPRD) && (MP_DRIVER == 1)
		/* Pull up BT reset pin.*/
		RTW_INFO("%s: pull up BT reset pin when bt start mp test\n", __func__);
		rtw_wifi_gpio_wlan_ctrl(WLAN_BT_PWDN_ON);
#endif
		RTW_INFO(" FirmwareDownload!\n");

#if defined(CONFIG_RTL8723B)
		status = rtl8723b_FirmwareDownload(padapter, _FALSE);
#endif
		RTW_INFO("Wait for FirmwareDownloadBT fw boot!\n");
		rtw_msleep_os(1000);
#ifdef CONFIG_BT_COEXIST
		rtw_btcoex_HaltNotify(padapter);
		RTW_INFO("SetBT btcoex HaltNotify !\n");
		/*hal_btcoex1ant_SetAntPath(padapter);*/
		rtw_btcoex_SetManualControl(padapter, _TRUE);
#endif
		_rtw_memset(extra, '\0', wrqu->data.length);
		BtReq.opCodeVer = 1;
		BtReq.OpCode = 0;
		BtReq.paraLength = 0;
		mptbt_BtControlProcess(padapter, &BtReq);
		rtw_msleep_os(200);

		RTW_INFO("FirmwareDownloadBT ready = 0x%x 0x%x", pMptCtx->mptOutBuf[4], pMptCtx->mptOutBuf[5]);
		if ((pMptCtx->mptOutBuf[4] == 0x00) && (pMptCtx->mptOutBuf[5] == 0x00)) {
			if (padapter->mppriv.bTxBufCkFail == _TRUE)
				pch += sprintf(pch, "check TxBuf Fail.\n");
			else
				pch += sprintf(pch, "download FW Fail.\n");
		} else {
#ifdef CONFIG_BT_COEXIST
			rtw_btcoex_SwitchBtTRxMask(padapter);
#endif
			rtw_msleep_os(200);
			pch += sprintf(pch, "download FW OK.\n");
			goto exit;
		}
		goto exit;
	}

	if (strncmp(extra, "down", 4) == 0) {
		RTW_INFO("SetBT down for to hal_init !\n");
#ifdef CONFIG_BT_COEXIST
		rtw_btcoex_SetManualControl(padapter, _FALSE);
		rtw_btcoex_Initialize(padapter);
#endif
		pHalFunc->read_adapter_info(padapter);
		pHalFunc->hal_deinit(padapter);
		pHalFunc->hal_init(padapter);
		rtw_pm_set_ips(padapter, IPS_NONE);
		LeaveAllPowerSaveMode(padapter);
		MPT_PwrCtlDM(padapter, 0);
		rtw_write32(padapter, 0xcc, (rtw_read32(padapter, 0xcc) | 0x00000004));
		rtw_write32(padapter, 0x6b, (rtw_read32(padapter, 0x6b) & 0xFFFFFFEF));
		rtw_msleep_os(600);
		/*rtw_write32(padapter, 0x6a, (rtw_read32(padapter, 0x6a)& 0xFFFFFFFE));*/
		rtw_write32(padapter, 0x6b, (rtw_read32(padapter, 0x6b) | 0x00000010));
		rtw_write32(padapter, 0xcc, (rtw_read32(padapter, 0xcc) & 0xFFFFFFFB));
		rtw_msleep_os(1200);
		goto exit;
	}
	if (strncmp(extra, "disable", 7) == 0) {
		RTW_INFO("SetBT disable !\n");
		rtw_write32(padapter, 0x6a, (rtw_read32(padapter, 0x6a) & 0xFFFFFFFB));
		rtw_msleep_os(500);
		goto exit;
	}
	if (strncmp(extra, "enable", 6) == 0) {
		RTW_INFO("SetBT enable !\n");
		rtw_write32(padapter, 0x6a, (rtw_read32(padapter, 0x6a) | 0x00000004));
		rtw_msleep_os(500);
		goto exit;
	}
	if (strncmp(extra, "h2c", 3) == 0) {
		RTW_INFO("SetBT h2c !\n");
		pHalData->bBTFWReady = _TRUE;
		rtw_hal_fill_h2c_cmd(padapter, 0x63, 1, u1H2CBtMpOperParm);
		goto exit;
	}
	if (strncmp(extra, "2ant", 4) == 0) {
		RTW_INFO("Set BT 2ant use!\n");
		phy_set_mac_reg(padapter, 0x67, BIT5, 0x1);
		rtw_write32(padapter, 0x948, 0000);

		goto exit;
	}

	if (ready != 0 && testmode != 0 && trxparam != 0 && setgen != 0 && getgen != 0 && testctrl != 0 && testbt != 0 && readtherm != 0 && setbtmac != 0)
		return -EFAULT;

	if (testbt == 0) {
		BtReq.opCodeVer = 1;
		BtReq.OpCode = 6;
		BtReq.paraLength = cnts / 2;
		goto todo;
	}
	if (ready == 0) {
		BtReq.opCodeVer = 1;
		BtReq.OpCode = 0;
		BtReq.paraLength = 0;
		goto todo;
	}

	i = 0;
	while ((token = strsep(&pch, ",")) != NULL) {
		if (i > 1)
			break;
		tmp[i] = token;
		i++;
	}

	if ((tmp[0] != NULL) && (tmp[1] != NULL)) {
		cnts = strlen(tmp[1]);
		if (cnts < 1)
			return -EFAULT;

		RTW_INFO("%s: cnts=%d\n", __func__, cnts);
		RTW_INFO("%s: data=%s\n", __func__, tmp[1]);

		for (jj = 0, kk = 0; jj < cnts; jj++, kk += 2) {
			BtReq.pParamStart[jj] = key_2char2num(tmp[1][kk], tmp[1][kk + 1]);
			/*			RTW_INFO("BtReq.pParamStart[%d]=0x%02x\n", jj, BtReq.pParamStart[jj]);*/
		}
	} else
		return -EFAULT;

	if (testmode == 0) {
		BtReq.opCodeVer = 1;
		BtReq.OpCode = 1;
		BtReq.paraLength = 1;
	}
	if (trxparam == 0) {
		BtReq.opCodeVer = 1;
		BtReq.OpCode = 2;
		BtReq.paraLength = cnts / 2;
	}
	if (setgen == 0) {
		RTW_INFO("%s: BT_SET_GENERAL\n", __func__);
		BtReq.opCodeVer = 1;
		BtReq.OpCode = 3;/*BT_SET_GENERAL	3*/
		BtReq.paraLength = cnts / 2;
	}
	if (getgen == 0) {
		RTW_INFO("%s: BT_GET_GENERAL\n", __func__);
		BtReq.opCodeVer = 1;
		BtReq.OpCode = 4;/*BT_GET_GENERAL	4*/
		BtReq.paraLength = cnts / 2;
	}
	if (readtherm == 0) {
		RTW_INFO("%s: BT_GET_GENERAL\n", __func__);
		BtReq.opCodeVer = 1;
		BtReq.OpCode = 4;/*BT_GET_GENERAL	4*/
		BtReq.paraLength = cnts / 2;
	}

	if (testctrl == 0) {
		RTW_INFO("%s: BT_TEST_CTRL\n", __func__);
		BtReq.opCodeVer = 1;
		BtReq.OpCode = 5;/*BT_TEST_CTRL	5*/
		BtReq.paraLength = cnts / 2;
	}

	RTW_INFO("%s: Req opCodeVer=%d OpCode=%d paraLength=%d\n",
		 __func__, BtReq.opCodeVer, BtReq.OpCode, BtReq.paraLength);

	if (BtReq.paraLength < 1)
		goto todo;
	for (i = 0; i < BtReq.paraLength; i++) {
		RTW_INFO("%s: BtReq.pParamStart[%d] = 0x%02x\n",
			 __func__, i, BtReq.pParamStart[i]);
	}

todo:
	_rtw_memset(extra, '\0', wrqu->data.length);

	if (pHalData->bBTFWReady == _FALSE) {
		pch += sprintf(pch, "BTFWReady = FALSE.\n");
		goto exit;
	}

	mptbt_BtControlProcess(padapter, &BtReq);

	if (readtherm == 0) {
		pch += sprintf(pch, "BT thermal=");
		for (i = 4; i < pMptCtx->mptOutLen; i++) {
			if ((pMptCtx->mptOutBuf[i] == 0x00) && (pMptCtx->mptOutBuf[i + 1] == 0x00))
				goto exit;

			pch += sprintf(pch, " %d ", (pMptCtx->mptOutBuf[i] & 0x1f));
		}
	} else {
		for (i = 4; i < pMptCtx->mptOutLen; i++)
			pch += sprintf(pch, " 0x%x ", pMptCtx->mptOutBuf[i]);
	}

exit:
	wrqu->data.length = strlen(extra) + 1;
	RTW_INFO("-%s: output len=%d data=%s\n", __func__, wrqu->data.length, extra);

	return status;
}

#endif /*#ifdef CONFIG_RTL8723B*/

#ifdef RTW_HALMAC
int rtw_mp_gpio(struct net_device *dev,
		struct iw_request_info *info,
		struct iw_point *wrqu, char *extra)
{
#define GPIO_FUNC_OFFSET 2
#define GPIO_OUTPUT_MODE 1

	PADAPTER padapter = rtw_netdev_priv(dev);
	u8 input[RTW_IWD_MAX_LEN];
	u8 gpio_id = 0 , gpio_enable = 0;
	int ret;

	if (rtw_do_mp_iwdata_len_chk(__func__, wrqu->length))
		return -EFAULT;

	if (copy_from_user(input, wrqu->pointer, wrqu->length))
		return -EFAULT;

	_rtw_memset(extra, 0, wrqu->length);

	ret = sscanf(input, "%hhd,%hhd", &gpio_id, &gpio_enable);

	if (ret < 2) {
		sprintf(extra, "Invalid command format, please input mp_gpio x,y\n");
		wrqu->length = strlen(extra);
		return -EINVAL;
	}
	else if (gpio_id < 0 || gpio_id > 15) {
		sprintf(extra, "Invalid gpio_id, please input gpio_id : 0 ~ 15\n");
		wrqu->length = strlen(extra);
		return -EINVAL;
	}
	else if (gpio_enable != 0 && gpio_enable != 1) {
		sprintf(extra, "Invalid gpio_enable, please input gpio_enable : 0 or 1\n");
		wrqu->length = strlen(extra);
		return -EINVAL;
	}

	RTW_INFO("%s: gpio_id = %hhd, gpio_enable = %hhd\n", __func__, gpio_id , gpio_enable);

	ret = SetGpio(padapter, gpio_id, gpio_enable, GPIO_FUNC_OFFSET, GPIO_OUTPUT_MODE);

	sprintf(extra, "Set gpio_id:%d, gpio_enable:%d : %s\n", gpio_id, gpio_enable, (ret < 0) ? "fail":"success");
	wrqu->length = strlen(extra);

	return 0;
}
#endif

#endif
