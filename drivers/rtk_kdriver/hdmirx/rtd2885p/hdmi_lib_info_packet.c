/*=============================================================
 * Copyright (c)      Realtek Semiconductor Corporation, 2016
 *
 * All rights reserved.
 *
 *============================================================*/

/*======================= Description ============================
 *
 * file:    hdmi_lib_info_packet.c
 *
 * author:  qibin_huang@apowertec.com
 * date:2024/01/08
 * version:     1.0
 *
 *============================================================*/

/*========================Header Files============================*/
#include "hdmi_common.h"
#include "hdmi_reg.h"
#include "hdmi_lib_info_packet.h"
extern spinlock_t hdmi_spin_lock;

/*---------------------------------------------------
 * Func : lib_hdmi_read_packet_sram
 *
 * Desc : read packet SRAM
 *
 * Para : nport : HDMI port number
 *        start_addr : start address of packet sram
 *        len   : read length
 *        pbuf  : buffer
 *
 * Retn : N/A
 *--------------------------------------------------*/
void lib_hdmi_read_packet_sram(
            unsigned char           nport,
            unsigned int            start_addr,
            unsigned char           len,
            unsigned char          *pbuf
)
{
    unsigned char i;
    unsigned long flags = 0;

    spin_lock_irqsave(&hdmi_spin_lock, flags);  // add spinlock to prevent racing

    if (hdmi_rx[nport].hdmi_2p1_en) {
        for (i = 0; i < len; i++) {
            hdmi_out(HD21_HDMI_PSAP_reg, start_addr + i);
            pbuf[i] = hdmi_in(HD21_HDMI_PSDP_reg);
        }
    }
    else {
        for (i = 0; i < len; i++) {
            hdmi_out(HDMI_HDMI_PSAP_reg, start_addr + i);
            pbuf[i] = hdmi_in(HDMI_HDMI_PSDP_reg);
        }
    }

    spin_unlock_irqrestore(&hdmi_spin_lock, flags);
}

#ifndef UT_flag
/*---------------------------------------------------
 * Func : lib_hdmi_infoframe_packet_read
 *
 * Desc : read packet SRAM and check the checksum result
 *
 * Para :
 *        type_code : which infoframe is read
 *        nport : HDMI port number
 *        start_addr : start address of packet sram
 *        len   : read length
 *        pbuf  : buffer
 *
 * Retn : 0 : checksum failed, 1 : success
 *--------------------------------------------------*/
int lib_hdmi_infoframe_packet_read(
            unsigned char           type_code,
            unsigned char           nport,
            unsigned int            start_addr,
            unsigned char           len,
            unsigned char          *pbuf
)
{
    lib_hdmi_read_packet_sram(nport, start_addr, len, pbuf);
    return newbase_hdmi_infoframe_checksum_verification(type_code, len, pbuf);
}

//-------------------------------------------------------------------------
// FVS
//-------------------------------------------------------------------------

/*---------------------------------------------------
 * Func : lib_hdmi_is_fvs_received
 *
 * Desc : check if FVS packet received
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_is_fvs_received(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        return HD21_HDMI_GPVS_get_fvsps(hdmi_in(HD21_HDMI_GPVS_reg));
    }
    else {
        return HDMI_HDMI_GPVS_get_fvsps(hdmi_in(HDMI_HDMI_GPVS_reg));
    }
}


/*---------------------------------------------------
 * Func : lib_hdmi_clear_fvs_received_status
 *
 * Desc : clear FVS packet receive status
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_clear_fvs_received_status(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        hdmi_out(HD21_HDMI_GPVS_reg, HD21_HDMI_GPVS_fvsps_mask);
    }
    else {
        hdmi_out(HDMI_HDMI_GPVS_reg, HDMI_HDMI_GPVS_fvsps_mask);
    }
    return 0;
}


/*---------------------------------------------------
 * Func : lib_hdmi_read_fvs_info_packet
 *
 * Desc : read HDMI formum VSIF info packet
 *
 * Para : nport : HDMI port number
 *        pkt_buf : avi output buffer
 *
 * Retn : N/A
 *--------------------------------------------------*/
INFOFRAME_READ_RESULT lib_hdmi_read_fvs_info_packet(
            unsigned char               port,
            unsigned char               *pkt_buf
)
{
    int checksum_pass;
    if (!lib_hdmi_is_fvs_received(port) || pkt_buf == NULL) {
        return FVS_READ_NOT_READY;
    }

    lib_hdmi_clear_fvs_received_status(port);
    checksum_pass = lib_hdmi_infoframe_packet_read(TYPE_CODE_VS_PACKET, port, FVS_INFO_OFST + 1, FVS_INFO_LEN - 1, pkt_buf + 1);
    pkt_buf[0] = TYPE_CODE_VS_PACKET;
    if (checksum_pass) {
        return INFOFRAME_READ_SUCCESS;
    }
    else { return FVS_READ_CHECKSUM_ERROR; }
}

/*---------------------------------------------------
 * Func : lib_hdmi_read_fvs_allm_mode
 *
 * Desc : read ALLM from HDMI forum VSIF info packet
 *
 * Para : nport : HDMI port number
 *        p_allm : allm output buffer
 *
 * Retn : N/A
 *--------------------------------------------------*/
unsigned char lib_hdmi_read_fvs_allm_mode(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        return HD21_HDMI_FVS_get_allm_mode(hdmi_in(HD21_HDMI_FVS_reg));
    }
    else {
        return HDMI_HDMI_FVS_get_allm_mode(hdmi_in(HDMI_HDMI_FVS_reg));
    }
}

/*---------------------------------------------------
 * Func : lib_hdmi_read_fvs_pa_status
 *
 * Desc : read fvs packet abscence status
 *
 * Para : nport : HDMI port number
 *
 * Retn : 1: no fvs packet over specific frame count
 *        0: no packet absence happened yet
 *--------------------------------------------------*/
unsigned char lib_hdmi_read_fvs_pa_status(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        return HD21_HDMI_PA_IRQ_STA_get_hfvs_absence_irq_sta(hdmi_in(HD21_HDMI_PA_IRQ_STA_reg));
    }
    else {
        return HDMI_HDMI_PA_IRQ_STA_get_hfvs_absence_irq_sta(hdmi_in(HDMI_HDMI_PA_IRQ_STA_reg));
    }
}

/*---------------------------------------------------
 * Func : lib_hdmi_clear_fvs_pa_status
 *
 * Desc : clear fvs packet abscence status
 *
 * Para : nport : HDMI port number
 *
 * Retn : N/A
 *--------------------------------------------------*/
void lib_hdmi_clear_fvs_pa_status(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        hdmi_mask(HD21_HDMI_PA_IRQ_STA_reg, ~HD21_HDMI_PA_IRQ_STA_hfvs_absence_irq_sta_mask, HD21_HDMI_PA_IRQ_STA_hfvs_absence_irq_sta_mask);
    }
    else {
        hdmi_mask(HDMI_HDMI_PA_IRQ_STA_reg, ~HDMI_HDMI_PA_IRQ_STA_hfvs_absence_irq_sta_mask, HDMI_HDMI_PA_IRQ_STA_hfvs_absence_irq_sta_mask);
    }
}

//-------------------------------------------------------------------------
// DVS - Dolby Vendor Specific Info frame
//-------------------------------------------------------------------------


/*---------------------------------------------------
 * Func : lib_hdmi_is_dvs_received
 *
 * Desc : check if Dolby Vendor Specific packet received
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_is_dvs_received(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        return HD21_HDMI_GPVS_get_dvsps(hdmi_in(HD21_HDMI_GPVS_reg));
    }
    else {
        return HDMI_HDMI_GPVS_get_dvsps(hdmi_in(HDMI_HDMI_GPVS_reg));
    }
}


/*---------------------------------------------------
 * Func : lib_hdmi_clear_dvs_received_status
 *
 * Desc : clear Dolby Vendor Specific packet receive status
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_clear_dvs_received_status(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        hdmi_out(HD21_HDMI_GPVS_reg, HD21_HDMI_GPVS_dvsps_mask);
    }
    else {
        hdmi_out(HDMI_HDMI_GPVS_reg, HDMI_HDMI_GPVS_dvsps_mask);
    }
    return 0;
}


/*---------------------------------------------------
 * Func : lib_hdmi_read_dvs_info_packet
 *
 * Desc : read Dolby Vendor Specific info packet
 *
 * Para : nport : HDMI port number
 *        pkt_buf : avi output buffer
 *
 * Retn : N/A
 *--------------------------------------------------*/
INFOFRAME_READ_RESULT lib_hdmi_read_dvs_info_packet(
            unsigned char               port,
            unsigned char               *pkt_buf
)
{
    int checksum_pass;
    if (!lib_hdmi_is_dvs_received(port) || pkt_buf == NULL) {
        return DVS_READ_NOT_READY;
    }

    lib_hdmi_clear_dvs_received_status(port);
    checksum_pass = lib_hdmi_infoframe_packet_read(TYPE_CODE_VS_PACKET, port, DVS_INFO_OFST + 1, DVS_INFO_LEN - 1, pkt_buf + 1);

    pkt_buf[0] = TYPE_CODE_VS_PACKET;
    if (checksum_pass) { return INFOFRAME_READ_SUCCESS; }
    else { return DVS_READ_CHECKSUM_ERROR; }
}

/*---------------------------------------------------
 * Func : lib_hdmi_set_hfvs_absence_irq_en
 *
 * Desc : enable/disable hfvs absence detection
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_set_hfvs_absence_irq_en(unsigned char nport, unsigned en)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        return HD21_HDMI_GPVS_get_fvsps(hdmi_in(HD21_HDMI_GPVS_reg));
    }
    else {
        return HDMI_HDMI_GPVS_get_fvsps(hdmi_in(HDMI_HDMI_GPVS_reg));
    }
}



//-------------------------------------------------------------------------
// HDR10+ VS - HDR10+ Vendor Specific Info frame
//-------------------------------------------------------------------------


/*---------------------------------------------------
 * Func : lib_hdmi_is_hdr10pvs_received
 *
 * Desc : check if HDR10+ Vendor Specific packet received
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_is_hdr10pvs_received(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        return HD21_HDMI_GPVS_get_hdr10pvsps(hdmi_in(HD21_HDMI_GPVS_reg));
    }
    else {
        return HDMI_HDMI_GPVS_get_hdr10pvsps(hdmi_in(HDMI_HDMI_GPVS_reg));
    }
}


/*---------------------------------------------------
 * Func : lib_hdmi_clear_hdr10pvs_received_status
 *
 * Desc : clear HDR10+ Vendor Specific packet receive status
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_clear_hdr10pvs_received_status(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        hdmi_out(HD21_HDMI_GPVS_reg, HD21_HDMI_GPVS_hdr10pvsps_mask);
    }
    else {
        hdmi_out(HDMI_HDMI_GPVS_reg, HDMI_HDMI_GPVS_hdr10pvsps_mask);
    }
    return 0;
}


/*---------------------------------------------------
 * Func : lib_hdmi_read_hdr10pvs_info_packet
 *
 * Desc : read HDR10+ Vendor Specific info packet
 *
 * Para : nport : HDMI port number
 *        pkt_buf : avi output buffer
 *
 * Retn : N/A
 *--------------------------------------------------*/
INFOFRAME_READ_RESULT lib_hdmi_read_hdr10pvs_info_packet(
            unsigned char               port,
            unsigned char               *pkt_buf
)
{
    int checksum_pass;
    if (!lib_hdmi_is_hdr10pvs_received(port) || pkt_buf == NULL) {
        return HDR10PVS_READ_NOT_READY;
    }

    lib_hdmi_clear_hdr10pvs_received_status(port);
    checksum_pass = lib_hdmi_infoframe_packet_read(TYPE_CODE_VS_PACKET, port, HDR10PVS_INFO_OFST + 1, HDR10PVS_INFO_LEN - 1, pkt_buf + 1);

    pkt_buf[0] = TYPE_CODE_VS_PACKET;
    if (checksum_pass) { return INFOFRAME_READ_SUCCESS; }
    else { return HDR10PVS_READ_CHECKSUM_ERROR; }
}


//-------------------------------------------------------------------------
// VSI
//-------------------------------------------------------------------------


/*---------------------------------------------------
 * Func : lib_hdmi_is_vsi_received
 *
 * Desc : check if VSI packet received
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_is_vsi_received(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        return HD21_HDMI_GPVS_get_vsps(hdmi_in(HD21_HDMI_GPVS_reg));
    }
    else {
        return HDMI_HDMI_GPVS_get_vsps(hdmi_in(HDMI_HDMI_GPVS_reg));
    }
}


/*---------------------------------------------------
 * Func : lib_hdmi_clear_vsi_received_status
 *
 * Desc : clear VSI packet receive status
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_clear_vsi_received_status(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        hdmi_out(HD21_HDMI_GPVS_reg, HD21_HDMI_GPVS_vsps_mask);
    }
    else {
        hdmi_out(HDMI_HDMI_GPVS_reg, HDMI_HDMI_GPVS_vsps_mask);
    }
    return 0;
}


/*---------------------------------------------------
 * Func : lib_hdmi_read_vsi_info_packet
 *
 * Desc : read vsi info packet
 *
 * Para : nport : HDMI port number
 *        pkt_buf : avi output buffer
 *
 * Retn : N/A
 *--------------------------------------------------*/
INFOFRAME_READ_RESULT lib_hdmi_read_vsi_info_packet(
            unsigned char               port,
            unsigned char               *pkt_buf
)
{
    int checksum_pass;
    if (!lib_hdmi_is_vsi_received(port) || pkt_buf == NULL) {
        return VSI_READ_NOT_READY;
    }

    lib_hdmi_clear_vsi_received_status(port);

    lib_hdmi_read_packet_sram(port, VS_INFO_CNT_OFST, VS_INFO_CNT_LEN, pkt_buf + VS_INFO_LEN);  // HDMI HW will update whoe Packet, so we just copy all ot it directly
    checksum_pass = lib_hdmi_infoframe_packet_read(TYPE_CODE_VS_PACKET, port, VS_INFO_OFST + 1, VS_INFO_LEN - 1, pkt_buf + 1);

    pkt_buf[0] = TYPE_CODE_VS_PACKET;
    if (checksum_pass) { return INFOFRAME_READ_SUCCESS; }
    else { return VSI_READ_CHECKSUM_ERROR; }
}


//-------------------------------------------------------------------------
// AVI
//-------------------------------------------------------------------------


/*---------------------------------------------------
 * Func : lib_hdmi_is_avi_received
 *
 * Desc : check if AVI packet received
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
unsigned char lib_hdmi_is_avi_received(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        return HD21_HDMI_GPVS_get_avips(hdmi_in(HD21_HDMI_GPVS_reg));
    }
    else {
        return HDMI_HDMI_GPVS_get_avips(hdmi_in(HDMI_HDMI_GPVS_reg));
    }
}


/*---------------------------------------------------
 * Func : lib_hdmi_clear_avi_received_status
 *
 * Desc : clear AVI packet receive status
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
void lib_hdmi_clear_avi_received_status(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        hdmi_out(HD21_HDMI_GPVS_reg, HD21_HDMI_GPVS_avips_mask);
    }
    else {
        hdmi_out(HDMI_HDMI_GPVS_reg, HDMI_HDMI_GPVS_avips_mask);
    }
}


/*---------------------------------------------------
 * Func : lib_hdmi_read_avi_info_packet
 *
 * Desc : read avi info packet
 *
 * Para : nport : HDMI port number
 *        pkt_buf : avi output buffer
 *
 * Retn : 0 pass, others failed
 *--------------------------------------------------*/
INFOFRAME_READ_RESULT lib_hdmi_read_avi_info_packet(
            unsigned char               port,
            unsigned char               *pkt_buf,
            unsigned char              *p_bch
)
{
    unsigned char len_extend;
    int checksum_pass = 0;

    if (!lib_hdmi_is_avi_received(port) || pkt_buf == NULL || p_bch == NULL) {
        return AVI_READ_NOT_READY;
    }

    lib_hdmi_clear_avi_received_status(port);

    lib_hdmi_read_packet_sram(port, AVI_INFO_OFST, 1, p_bch);
    lib_hdmi_read_packet_sram(port, AVI_INFO_OFST + 1, AVI_INFO_LEN - 1, pkt_buf);


    // copy extra bytes
    if (pkt_buf[1] > 13) {
        len_extend = pkt_buf[1] - 13;

        if (len_extend >= AVI_INFO_CNT_LEN) {
            lib_hdmi_read_packet_sram(port, AVI_INFO_CNT_OFST, AVI_INFO_CNT_LEN, &pkt_buf[16]);
            checksum_pass = newbase_hdmi_infoframe_checksum_verification(TYPE_CODE_AVI_PACKET, AVI_INFO_LEN - 1 + AVI_INFO_CNT_LEN, pkt_buf);
        }
        else {// if(len_extend < AVI_PKT_CONT_LEN)
            lib_hdmi_read_packet_sram(port, AVI_INFO_CNT_OFST, len_extend, &pkt_buf[16]);
            checksum_pass = newbase_hdmi_infoframe_checksum_verification(TYPE_CODE_AVI_PACKET, AVI_INFO_LEN - 1 + len_extend, pkt_buf);
        }
    }
    else {
        checksum_pass = newbase_hdmi_infoframe_checksum_verification(TYPE_CODE_AVI_PACKET, AVI_INFO_LEN - 1, pkt_buf);
    }

    pkt_buf[2] = pkt_buf[1];    //len
    pkt_buf[1] = pkt_buf[0];    //version
    pkt_buf[0] = TYPE_CODE_AVI_PACKET;

    if (checksum_pass) { return INFOFRAME_READ_SUCCESS; }
    else { return AVI_READ_CHECKSUM_ERROR; }
}


//-------------------------------------------------------------------------
// SPD
//-------------------------------------------------------------------------


/*---------------------------------------------------
 * Func : lib_hdmi_is_spd_received
 *
 * Desc : check if SPD packet received
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
unsigned char lib_hdmi_is_spd_received(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        return HD21_HDMI_GPVS_get_spdps(hdmi_in(HD21_HDMI_GPVS_reg));
    }
    else {
        return HDMI_HDMI_GPVS_get_spdps(hdmi_in(HDMI_HDMI_GPVS_reg));
    }
}


/*---------------------------------------------------
 * Func : lib_hdmi_clear_spd_received_status
 *
 * Desc : clear SPD packet receive status
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
void lib_hdmi_clear_spd_received_status(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        hdmi_out(HD21_HDMI_GPVS_reg, HD21_HDMI_GPVS_spdps_mask);
    }
    else {
        hdmi_out(HDMI_HDMI_GPVS_reg, HDMI_HDMI_GPVS_spdps_mask);
    }
}

/*---------------------------------------------------
 * Func : lib_hdmi_read_spd_info_packet
 *
 * Desc : read spd info packet
 *
 * Para : nport : HDMI port number
 *        pkt_buf : avi output buffer
 *
 * Retn : N/A
 *--------------------------------------------------*/
INFOFRAME_READ_RESULT lib_hdmi_read_spd_info_packet(
            unsigned char           port,
            unsigned char           *pkt_buf,
            unsigned char          *p_bch
)
{
    int checksum_pass;
    if (!lib_hdmi_is_spd_received(port)) {
        return SPD_READ_NOT_READY;
    }

    lib_hdmi_clear_spd_received_status(port);
    lib_hdmi_read_packet_sram(port, SPD_INFO_OFST, 1, p_bch);
    checksum_pass = lib_hdmi_infoframe_packet_read(TYPE_CODE_SPD_PACKET, port, SPD_INFO_OFST + 1, SPD_INFO_LEN - 1, pkt_buf);
    // skip checksum
    pkt_buf[2] = pkt_buf[1];    //len
    pkt_buf[1] = pkt_buf[0];    //version
    pkt_buf[0] = TYPE_CODE_SPD_PACKET;
    if (checksum_pass) { return INFOFRAME_READ_SUCCESS; }
    else { return SPD_READ_CHECKSUM_ERROR; }
}


//-------------------------------------------------------------------------
// AUD
//-------------------------------------------------------------------------


/*---------------------------------------------------
 * Func : lib_hdmi_is_audiosampkt_received
 *
 * Desc : check if audio sample packet received
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
unsigned char lib_hdmi_is_audiosampkt_received(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        return HD21_HDMI_GPVS_get_audps(hdmi_in(HD21_HDMI_GPVS_reg));
    }
    else {
        return HDMI_HDMI_GPVS_get_audps(hdmi_in(HDMI_HDMI_GPVS_reg));
    }
}


/*---------------------------------------------------
 * Func : lib_hdmi_clear_audiosampkt_received_status
 *
 * Desc : clear audio sample packet receive status
 *
 * Para : nport : HDMI port number
 *
 * Retn : N/A
 *--------------------------------------------------*/
void lib_hdmi_clear_audiosampkt_received_status(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        hdmi_out(HD21_HDMI_GPVS_reg, HD21_HDMI_GPVS_audps_mask);
    }
    else {
        hdmi_out(HDMI_HDMI_GPVS_reg, HDMI_HDMI_GPVS_audps_mask);
    }
}


//-------------------------------------------------------------------------
// HBR_AUD
//-------------------------------------------------------------------------


/*---------------------------------------------------
 * Func : lib_hdmi_is_hbr_audiostreampkt_received
 *
 * Desc : check if high bitrate audio stream packet received
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
unsigned char lib_hdmi_is_hbr_audiostreampkt_received(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        return HD21_HDMI_GPVS_get_hbr_audps(hdmi_in(HD21_HDMI_GPVS_reg));
    }
    else {
        return HDMI_HDMI_GPVS_get_hbr_audps(hdmi_in(HDMI_HDMI_GPVS_reg));
    }
}


/*---------------------------------------------------
 * Func : lib_hdmi_clear_hbr_audiostreampkt_received_status
 *
 * Desc : clear high bitrate audio stream packet receive status
 *
 * Para : nport : HDMI port number
 *
 * Retn : N/A
 *--------------------------------------------------*/
void lib_hdmi_clear_hbr_audiostreampkt_received_status(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        hdmi_out(HD21_HDMI_GPVS_reg, HD21_HDMI_GPVS_hbr_audps_mask);
    }
    else {
        hdmi_out(HDMI_HDMI_GPVS_reg, HDMI_HDMI_GPVS_hbr_audps_mask);
    }
}


//-------------------------------------------------------------------------
// Audio Packet
//-------------------------------------------------------------------------


/*---------------------------------------------------
 * Func : lib_hdmi_is_audiopkt_received
 *
 * Desc : check if audio packet received
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
unsigned char lib_hdmi_is_audiopkt_received(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        return HD21_HDMI_GPVS_get_aps(hdmi_in(HD21_HDMI_GPVS_reg));
    }
    else {
        return HDMI_HDMI_GPVS_get_aps(hdmi_in(HDMI_HDMI_GPVS_reg));
    }
}


/*---------------------------------------------------
 * Func : lib_hdmi_clear_audiopkt_received_status
 *
 * Desc : clear Audio packet receive status
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
void lib_hdmi_clear_audiopkt_received_status(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        hdmi_out(HD21_HDMI_GPVS_reg, HD21_HDMI_GPVS_aps_mask);
    }
    else {
        hdmi_out(HDMI_HDMI_GPVS_reg, HDMI_HDMI_GPVS_aps_mask);
    }
}


/*---------------------------------------------------
 * Func : lib_hdmi_read_audio_packet
 *
 * Desc : read audio info packet
 *
 * Para : nport : HDMI port number
 *        pkt_buf : avi output buffer
 *
 * Retn : N/A
 *--------------------------------------------------*/
INFOFRAME_READ_RESULT lib_hdmi_read_audio_packet(
            unsigned char           port,
            unsigned char           *pkt_buf
)
{
    int checksum_pass;
    if (!lib_hdmi_is_audiopkt_received(port)) {
        return AUDIO_READ_NOT_READY;
    }

    lib_hdmi_clear_audiopkt_received_status(port);
    checksum_pass = lib_hdmi_infoframe_packet_read(TYPE_CODE_AUDIO_PACKET, port, AUDIO_INFO_OFST + 1, AUDIO_INFO_LEN - 1, pkt_buf);
    // skip checksum
    pkt_buf[2] = pkt_buf[1];    //len
    pkt_buf[1] = pkt_buf[0];    //version
    pkt_buf[0] = TYPE_CODE_AUDIO_PACKET;

    if (checksum_pass) { return INFOFRAME_READ_SUCCESS; }
    else { return AUDIO_READ_CHECKSUM_ERROR; }
}



//-------------------------------------------------------------------------
// DRM
//-------------------------------------------------------------------------


/*---------------------------------------------------
 * Func : lib_hdmi_is_drm_received
 *
 * Desc : check if drm packet received
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
unsigned char lib_hdmi_is_drm_received(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        return HD21_HDMI_GPVS_get_drmps(hdmi_in(HD21_HDMI_GPVS_reg));
    }
    else {
        return HDMI_HDMI_GPVS_get_drmps(hdmi_in(HDMI_HDMI_GPVS_reg));
    }
}


/*---------------------------------------------------
 * Func : lib_hdmi_clear_drm_received_status
 *
 * Desc : clear DRM packet receive status
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
void lib_hdmi_clear_drm_received_status(unsigned char nport)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        hdmi_out(HD21_HDMI_GPVS_reg, HD21_HDMI_GPVS_drmps_mask);
    }
    else {
        hdmi_out(HDMI_HDMI_GPVS_reg, HDMI_HDMI_GPVS_drmps_mask);
    }
}


/*---------------------------------------------------
 * Func : lib_hdmi_read_drm_packet
 *
 * Desc : read drm info packet
 *
 * Para : nport : HDMI port number
 *        pkt_buf : packet output
 *
 * Retn : 0 : success, -1 : failed
 *--------------------------------------------------*/
INFOFRAME_READ_RESULT lib_hdmi_read_drm_packet(
            unsigned char           port,
            unsigned char           *pkt_buf,
            unsigned char          *p_bch
)
{
    int checksum_pass;
    if (!lib_hdmi_is_drm_received(port) || pkt_buf == NULL || p_bch == NULL) {
        return DRM_READ_NOT_READY;
    }

    lib_hdmi_clear_drm_received_status(port);
    lib_hdmi_read_packet_sram(port, DRM_INFO_OFST, 1, p_bch);

    checksum_pass = lib_hdmi_infoframe_packet_read(TYPE_CODE_DRM_PACKET, port, DRM_INFO_OFST + 1, DRM_INFO_LEN - 1, pkt_buf);
    // skip checksum
    pkt_buf[2] = pkt_buf[1]; // len
    pkt_buf[1] = pkt_buf[0]; // version
    pkt_buf[0] = TYPE_CODE_DRM_PACKET; // Info Frame Type

    if (checksum_pass) { return INFOFRAME_READ_SUCCESS; }
    else { return DRM_READ_CHECKSUM_ERROR; }
}
#endif // UT_flag


//-------------------------------------------------------------------------
// Reserved Packet
//-------------------------------------------------------------------------


/*---------------------------------------------------
 * Func : lib_hdmi_set_rsv_packet_type
 *
 * Desc : set rsv packet type (without OUI check)
 *
 * Para : nport : HDMI port number
 *        index : rsv packet index
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_set_rsv_packet_type(
            unsigned char           nport,
            unsigned char           index,
            unsigned char           pkt_type
)
{
    // for old API
    return lib_hdmi_set_rsv_packet_type_ex(nport, index, pkt_type, 0, 0);
}


/*---------------------------------------------------
 * Func : lib_hdmi_set_rsv_packet_type_ex
 *
 * Desc : set rsv packet type (with OUI check)
 *
 * Para : nport     : HDMI port number
 *        index     : rsv packet index
 *        pkt_type  : type of packet
 *        check_oui : check oui or not (only valid f
 *        oui_1     : 1st oui of the packet
 *
 * Retn : 0 : successed, -1 : failed
 *--------------------------------------------------*/
int lib_hdmi_set_rsv_packet_type_ex(
            unsigned char           nport,
            unsigned char           index,
            unsigned char           pkt_type,
            unsigned char           check_oui,
            unsigned char           oui_1
)
{
    if (index > 3) {
        return -1;
    }

    check_oui = (pkt_type == TYPE_CODE_VS_PACKET && check_oui) ? 1 : 0;

    if (check_oui) {
        check_oui = 1;

        if (pkt_type != TYPE_CODE_VS_PACKET) {
            HDMI_WARN("set reserved packet type with OUI check failed, packet type (%02x) is not VSI, force disable OUI check\n", pkt_type);
            check_oui = 0;
            oui_1 = 0;
        }
    }

    if (hdmi_rx[nport].hdmi_2p1_en) {
        // Packet Type
        hdmi_mask(HD21_HDMI_PTRSV1_reg, ~(HD21_HDMI_PTRSV1_pt0_mask << (index << 3)), pkt_type << (index << 3));

        // OUI pattern
        hdmi_mask(HD21_HDMI_PTRSV2_reg, ~(HD21_HDMI_PTRSV2_pt0_oui_1st_mask << (index << 3)),
                  (HD21_HDMI_PTRSV2_pt0_oui_1st(oui_1) << (index << 3)));
        hdmi_mask(HD21_HDMI_PTRSV4_reg, ~(HD21_HDMI_PTRSV4_pt0_oui_2st_mask << (index << 3)),
                  (HD21_HDMI_PTRSV4_pt0_oui_2st(oui_1) << (index << 3)));
        hdmi_mask(HD21_HDMI_PTRSV5_reg, ~(HD21_HDMI_PTRSV5_pt0_oui_3st_mask << (index << 3)),
                  (HD21_HDMI_PTRSV5_pt0_oui_3st(oui_1) << (index << 3)));

        // OUI check enable
        hdmi_mask(HD21_HDMI_PTRSV3_reg, ~(HD21_HDMI_PTRSV3_pt0_recognize_oui_en_mask << (index)),
                  (HD21_HDMI_PTRSV3_pt0_recognize_oui_en(check_oui) << (index)));
        // clear 2st/3st OUI pattern
        hdmi_mask(HD21_HDMI_PTRSV6_reg, ~(HD21_HDMI_PTRSV6_pt0_recognize_oui23_en_mask << (index)),
                  (HD21_HDMI_PTRSV6_pt0_recognize_oui23_en(check_oui) << (index)));
    }
    else {
        // Packet Type
        hdmi_mask(HDMI_HDMI_PTRSV1_reg, ~(HDMI_HDMI_PTRSV1_pt0_mask << (index << 3)), pkt_type << (index << 3));

        // OUI pattern
        hdmi_mask(HDMI_HDMI_PTRSV2_reg, ~(HDMI_HDMI_PTRSV2_pt0_oui_1st_mask << (index << 3)),
                  (HDMI_HDMI_PTRSV2_pt0_oui_1st(oui_1) << (index << 3)));
        hdmi_mask(HDMI_HDMI_PTRSV4_reg, ~(HDMI_HDMI_PTRSV4_pt0_oui_2st_mask << (index << 3)),
                  (HDMI_HDMI_PTRSV4_pt0_oui_2st(oui_1) << (index << 3)));
        hdmi_mask(HDMI_HDMI_PTRSV5_reg, ~(HDMI_HDMI_PTRSV5_pt0_oui_3st_mask << (index << 3)),
                  (HDMI_HDMI_PTRSV5_pt0_oui_3st(oui_1) << (index << 3)));

        // clear 1st OUI pattern
        hdmi_mask(HDMI_HDMI_PTRSV3_reg, ~(HDMI_HDMI_PTRSV3_pt0_recognize_oui_en_mask << (index)),
                  (HDMI_HDMI_PTRSV3_pt0_recognize_oui_en(check_oui) << (index)));
        // clear 2st/3st OUI pattern
        hdmi_mask(HDMI_HDMI_PTRSV6_reg, ~(HDMI_HDMI_PTRSV6_pt0_recognize_oui23_en_mask << (index)),
                  (HDMI_HDMI_PTRSV6_pt0_recognize_oui23_en(check_oui) << (index)));
    }
    return 0;
}

#ifndef UT_flag
/*---------------------------------------------------
 * Func : lib_hdmi_is_rsv_packet_received
 *
 * Desc : check if rsv packet received
 *
 * Para : nport : HDMI port number
 *        index : rsv packet index
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_is_rsv_packet_received(
            unsigned char           nport,
            unsigned char           index
)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        switch (index) {
            case 0: return HD21_HDMI_GPVS_get_rsv0ps(hdmi_in(HD21_HDMI_GPVS_reg));
            case 1: return HD21_HDMI_GPVS_get_rsv1ps(hdmi_in(HD21_HDMI_GPVS_reg));
            case 2: return HD21_HDMI_GPVS_get_rsv2ps(hdmi_in(HD21_HDMI_GPVS_reg));
            case 3: return HD21_HDMI_GPVS_get_rsv3ps(hdmi_in(HD21_HDMI_GPVS_reg));
        }
    }
    else {
        switch (index) {
            case 0: return HDMI_HDMI_GPVS_get_rsv0ps(hdmi_in(HDMI_HDMI_GPVS_reg));
            case 1: return HDMI_HDMI_GPVS_get_rsv1ps(hdmi_in(HDMI_HDMI_GPVS_reg));
            case 2: return HDMI_HDMI_GPVS_get_rsv2ps(hdmi_in(HDMI_HDMI_GPVS_reg));
            case 3: return HDMI_HDMI_GPVS_get_rsv3ps(hdmi_in(HDMI_HDMI_GPVS_reg));
        }
    }

    return 0;
}


/*---------------------------------------------------
 * Func : lib_hdmi_clear_rsv_packet_status
 *
 * Desc : read reserved info packet
 *
 * Para : nport : HDMI port number
 *        index : rsv packet index
 *
 * Retn : N/A
 *--------------------------------------------------*/
int lib_hdmi_clear_rsv_packet_status(
            unsigned char           nport,
            unsigned char           index
)
{
    if (hdmi_rx[nport].hdmi_2p1_en) {
        switch (index) {
            case 0: hdmi_out(HD21_HDMI_GPVS_reg, HD21_HDMI_GPVS_rsv0ps_mask); break;
            case 1: hdmi_out(HD21_HDMI_GPVS_reg, HD21_HDMI_GPVS_rsv1ps_mask); break;
            case 2: hdmi_out(HD21_HDMI_GPVS_reg, HD21_HDMI_GPVS_rsv2ps_mask); break;
            case 3: hdmi_out(HD21_HDMI_GPVS_reg, HD21_HDMI_GPVS_rsv3ps_mask); break;
            default:
                return -1;
        }
    }
    else {
        switch (index) {
            case 0: hdmi_out(HDMI_HDMI_GPVS_reg, HDMI_HDMI_GPVS_rsv0ps_mask); break;
            case 1: hdmi_out(HDMI_HDMI_GPVS_reg, HDMI_HDMI_GPVS_rsv1ps_mask); break;
            case 2: hdmi_out(HDMI_HDMI_GPVS_reg, HDMI_HDMI_GPVS_rsv2ps_mask); break;
            case 3: hdmi_out(HDMI_HDMI_GPVS_reg, HDMI_HDMI_GPVS_rsv3ps_mask); break;
            default:
                return -1;
        }
    }

    return 0;
}


/*---------------------------------------------------
 * Func : lib_hdmi_read_reserved_packet
 *
 * Desc : read reserved packet
 *
 * Para : nport : HDMI port number
 *        index : rsv packet index
 *        pkt_buf : packet output
 *
 * Retn : N/A
 *--------------------------------------------------*/
INFOFRAME_READ_RESULT lib_hdmi_read_reserved_packet(
            unsigned char           port,
            unsigned char           index,
            unsigned char          *pkt_buf,
            unsigned char           len
)
{
    if (pkt_buf == NULL || !lib_hdmi_is_rsv_packet_received(port, index)) {
        return RESERVED_READ_NOT_READY;
    }

    if (len > 31) {
        len = 31;
    }

    switch (index) {
        case 0:  lib_hdmi_read_packet_sram(port, RSV0_OFST, len, pkt_buf); break;
        case 1:  lib_hdmi_read_packet_sram(port, RSV1_OFST, len, pkt_buf); break;
        case 2:  lib_hdmi_read_packet_sram(port, RSV2_OFST, len, pkt_buf); break;
        case 3:  lib_hdmi_read_packet_sram(port, RSV3_OFST, len, pkt_buf); break;
        default:
            return -1;
    }

    lib_hdmi_clear_rsv_packet_status(port, index);

    return INFOFRAME_READ_SUCCESS; //no need to check checksum on reserved packet because
}

#endif  // UT_flag
