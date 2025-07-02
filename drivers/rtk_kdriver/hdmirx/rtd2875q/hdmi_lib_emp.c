/*=============================================================
 * Copyright (c)      Realtek Semiconductor Corporation, 2016
 *
 * All rights reserved.
 *
 *============================================================*/

/*======================= Description ============================
 *
 * file:    hdmi_lib_emp.c
 *
 * author:  qibin_huang@apowertec.com
 * date:2024/01/08
 * version:     1.0
 *
 *============================================================*/

/*========================Header Files============================*/
#include "hdmi_common.h"
#include "hdmi_reg.h"
#include "hdmi_lib_emp.h"
#include "hdmi_dsc.h"

extern spinlock_t hdmi_spin_lock;
#define NOT_PER_PORT  0
#define PER_PORT      1

void _dump_cvtem_info(CVTEM_INFO *p_info)
{
    int i;

    HDMI_EMP_INFO("CVTEM: hfront = %d, hsync = %d, hback=%d, hcative_bytes=%d\n",
                  p_info->hfront, p_info->hsync, p_info->hback, p_info->hcative_bytes);

    for (i = 0; i < sizeof(p_info->pps); i += 16) {
        unsigned char *pps = &p_info->pps[i];

        HDMI_EMP_INFO("CVTEM:PPS[%d] = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
                      i,
                      pps[0], pps[1], pps[2], pps[3],
                      pps[4], pps[5], pps[6], pps[7],
                      pps[8], pps[9], pps[10], pps[11],
                      pps[12], pps[13], pps[14], pps[15]);
    }
}

int _parse_cvtem_info(unsigned char pkt_buf[EM_CVTEM_INFO_LEN], CVTEM_INFO *p_info)
{
    EMP_PKT *p_emp = (EMP_PKT *) pkt_buf;

    // copy pps
    memcpy(&p_info->pps[0],   &p_emp[0].pb[7], 21);
    memcpy(&p_info->pps[21],  &p_emp[1].pb[0], 28);
    memcpy(&p_info->pps[49],  &p_emp[2].pb[0], 28);
    memcpy(&p_info->pps[77],  &p_emp[3].pb[0], 28);
    memcpy(&p_info->pps[105], &p_emp[4].pb[0], 23);

    // parse hfront
    p_info->hfront = (p_emp[4].pb[24] << 8) + p_emp[4].pb[23];
    p_info->hsync  = (p_emp[4].pb[26] << 8) + p_emp[4].pb[25];
    p_info->hback  = (p_emp[5].pb[0] << 8)  + p_emp[4].pb[27];
    p_info->hcative_bytes = (p_emp[5].pb[2] << 8)  + p_emp[5].pb[1];

    if ((p_info->hfront == 0) || (p_info->hsync == 0) || (p_info->hback == 0)) {
        HDMI_DSC_INFO("[_parse_cvtem_info] cvtem parse fail, hfront=%d, hsync=%d, hback=%d\n", p_info->hfront, p_info->hsync, p_info->hback);
        return FALSE;
    }
    else {
        return TRUE;
    }
}

/*---------------------------------------------------
 * Func : lib_hdmi_read_emp_sram
 *
 * Desc : read EM packet SRAM
 *
 * Para : nport : HDMI port number
 *        start_addr : start address of packet sram
 *        len   : read length
 *        pbuf  : buffer
 *        is_pp: is per port
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
void lib_hdmi_read_emp_sram(
            unsigned char           nport,
            unsigned int            start_addr,
            unsigned char           len,
            unsigned char          *pbuf,
            unsigned char           is_pp
)
{
    unsigned int i;
    unsigned long flags;

    spin_lock_irqsave(&hdmi_spin_lock, flags);
    if (is_pp == NOT_PER_PORT) {
        for (i = 0; i < len; i++) {
            hdmi_out(HDMI_HDMI_EMAP_reg, start_addr + i);
            pbuf[i] = hdmi_in(HDMI_HDMI_EMDP_reg);
        }
    }
    else {
        for (i = 0; i < len; i++) {
            hdmi_out(HDMI_HDMI_EMAP_PP_reg, start_addr + i);
            pbuf[i] = hdmi_in(HDMI_HDMI_EMDP_PP_reg);
        }
    }

    spin_unlock_irqrestore(&hdmi_spin_lock, flags);
}


/*---------------------------------------------------
 * Func : lib_hdmi_emp_enable_all_mode
 *
 * Desc : enable/disable emp all mode.
 *
 * Para : nport : HDMI port number
 *        enable : 0 : disable  (only receive data within fapa region)
 *                 1 : enable
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
void lib_hdmi_emp_enable_all_mode(
            unsigned char           nport,
            unsigned int            enable
)
{
    hdmi_mask(HDMI_fapa_ct_tmp_reg, ~HDMI_fapa_ct_tmp_all_mode_mask,
              (enable) ? HDMI_fapa_ct_tmp_all_mode_mask : 0);
}

//----------------------------------------------------------------------------------------
// VCTEM
//---------------------------------------------------------------------------------------

/*---------------------------------------------------
 * Func : lib_hdmi_is_em_cvtem_received
 *
 * Desc : check if CVT EM packet received
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_is_em_cvtem_received(unsigned char nport)
{
    return 0;
}

/*---------------------------------------------------
 * Func : lib_hdmi_clear_em_cvtem_received_status
 *
 * Desc : clear CVTEM packet receive status
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_clear_em_cvtem_received_status(unsigned char nport)
{
    return 0;
}

/*---------------------------------------------------
 * Func : lib_hdmi_clear_em_cvtem_sram
 *
 * Desc : clear CVTEM sram
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_clear_em_cvtem_sram(unsigned char nport)
{
    return 0;
}

/*---------------------------------------------------
 * Func : lib_hdmi_read_em_cvtem_packet
 *
 * Desc : read CVTEM packet receive status
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_read_em_cvtem_packet(
            unsigned char           nport,
            unsigned char           pkt_buf[EM_CVTEM_INFO_LEN]
)
{
    return 0;
}



//-------------------------------------------------------------------------
// VTEM (Video Timing Extended Metadata)
//-------------------------------------------------------------------------

/*---------------------------------------------------
 * Func : lib_hdmi_is_em_vtem_received
 *
 * Desc : check if VRR EM packet has been received
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_is_em_vtem_received(unsigned char nport)
{
    if (HDMI_VRREM_ST_get_em_vrr_no(hdmi_in(HDMI_VRREM_ST_reg))) { // No vsem
        hdmi_out(HDMI_VRREM_ST_reg, HDMI_VRREM_ST_em_vrr_no_mask);
        return 0;
    }

    if (!HDMI_VRREM_ST_get_em_vrr_first(hdmi_in(HDMI_VRREM_ST_reg))) { // No First Packet
        return 0;
    }

    if (!HDMI_VRREM_ST_get_em_vrr_last(hdmi_in(HDMI_VRREM_ST_reg))) { // No Last Packet
        return 0;
    }

    return 1;
}


/*---------------------------------------------------
 * Func : lib_hdmi_clear_em_vtem_received_status
 *
 * Desc : clear VRR packet receive status
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : successed, others : failed
 *--------------------------------------------------*/
int lib_hdmi_clear_em_vtem_received_status(unsigned char nport)
{
    // write 1 clear no/first/last flag of emp
    hdmi_mask(HDMI_VRREM_ST_reg, ~HDMI_VRREM_ST_em_vrr_no_mask, HDMI_VRREM_ST_em_vrr_no_mask);
    hdmi_mask(HDMI_VRREM_ST_reg, ~(HDMI_VRREM_ST_em_vrr_first_mask | HDMI_VRREM_ST_em_vrr_last_mask), (HDMI_VRREM_ST_em_vrr_first_mask | HDMI_VRREM_ST_em_vrr_last_mask));

    return 0;
}


/*---------------------------------------------------
 * Func : lib_hdmi_clear_em_vtem_sram
 *
 * Desc : clear VTEM sram
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_clear_em_vtem_sram(unsigned char nport)
{
    hdmi_mask(HDMI_HDMI_VRR_EMC_reg, ~HDMI_HDMI_VRR_EMC_vrr_clr_mask, HDMI_HDMI_VRR_EMC_vrr_clr_mask);
    hdmi_mask(HDMI_HDMI_VRR_EMC_reg, ~HDMI_HDMI_VRR_EMC_vrr_clr_mask, 0);
    return 0;
}


/*---------------------------------------------------
 * Func : lib_hdmi_read_em_vtem_packet
 *
 * Desc : read VRR packet receive status
 *
 * Para : nport : HDMI port number
 *
 * Retn : > 0 : number of byte received, <0 : failed
 *--------------------------------------------------*/
int lib_hdmi_read_em_vtem_packet(
            unsigned char           nport,
            unsigned char           pkt_buf[EM_VTEM_INFO_LEN]
)
{
    unsigned char sram_buf[EM_VTEM_INFO_LEN];
    memset(sram_buf, 0, EM_VTEM_INFO_LEN);

    if (pkt_buf == NULL || !lib_hdmi_is_em_vtem_received(nport)) {
        return -1;
    }

    lib_hdmi_read_emp_sram(nport, EM_VTEM_INFO_OFST + EM_VTEM_INFO_LEN - 4, 4, sram_buf, PER_PORT); // HB0~2 + BCH

    if (sram_buf[3] != 0) {
        HDMI_WARN("VTEM BCH Error= %02x\n", sram_buf[3]);
    }

    lib_hdmi_read_emp_sram(nport, EM_VTEM_INFO_OFST, EM_VTEM_INFO_LEN - 4, sram_buf + 3, PER_PORT); // PB0~PB27

    lib_hdmi_clear_em_vtem_received_status(nport);

    memcpy(pkt_buf, sram_buf, EM_VTEM_INFO_LEN);

    HDMI_EMP_DBG("VTEM= %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n",
                 pkt_buf[0], pkt_buf[1],
                 pkt_buf[2], pkt_buf[3],
                 pkt_buf[4], pkt_buf[5],
                 pkt_buf[6], pkt_buf[7],
                 pkt_buf[8], pkt_buf[9],
                 pkt_buf[10], pkt_buf[11],
                 pkt_buf[12], pkt_buf[13]);

    return 0;
}

//-------------------------------------------------------------------------
// VSEM
//-------------------------------------------------------------------------


/*---------------------------------------------------
 * Func : lib_hdmi_set_em_vsem_type
 *
 * Desc : set oui of vendor specific em packet
 *
 * Para : nport : HDMI port number
 *        oui   : 24 bits orgination ID
 *        data_set_tag : 16 bits data set tag
 *        sync  : sync of EMP
 *           0  : non sync (able to receive all the time)
 *           1  : sync mode (only received in FAPA region)
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_set_em_vsem_type(
            unsigned char           nport,
            const EMP_PARAM        *p_emp_cfg
)
{
    if (p_emp_cfg == NULL) {
        return -1;
    }
    hdmi_out(HDMI_HDMI_VSEM_EMC_reg,  HDMI_HDMI_VSEM_EMC_set_tag_msb(p_emp_cfg->data_set_tag[1]) |
             HDMI_HDMI_VSEM_EMC_set_tag_lsb(p_emp_cfg->data_set_tag[0]) |
             HDMI_HDMI_VSEM_EMC_recognize_tag_en(p_emp_cfg->recognize_tag));

    hdmi_out(HDMI_HDMI_VSEM_EMC2_reg, HDMI_HDMI_VSEM_EMC2_sync(p_emp_cfg->sync) |
             HDMI_HDMI_VSEM_EMC2_recognize_oui_en(p_emp_cfg->recognize_oui) |
             HDMI_HDMI_VSEM_EMC2_oui_1st(p_emp_cfg->oui[0]) |
             HDMI_HDMI_VSEM_EMC2_oui_2nd(p_emp_cfg->oui[1]) |
             HDMI_HDMI_VSEM_EMC2_oui_3rd(p_emp_cfg->oui[2]));

    return 0;
}


/*---------------------------------------------------
 * Func : lib_hdmi_is_em_vsem_received
 *
 * Desc : check if Vendor Specific EM packet has been received
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_is_em_vsem_received(unsigned char nport)
{
    if (HDMI_VSEM_ST_get_em_vsem_no(hdmi_in(HDMI_VSEM_ST_reg))) { // No vsem
        hdmi_out(HDMI_VSEM_ST_reg, HDMI_VSEM_ST_em_vsem_no_mask);
        return 0;
    }

    if (!HDMI_VSEM_ST_get_em_vsem_first(hdmi_in(HDMI_VSEM_ST_reg))) { // No First Packet
        return 0;
    }

    if (!HDMI_VSEM_ST_get_em_vsem_last(hdmi_in(HDMI_VSEM_ST_reg))) { // No Last Packet
        return 0;
    }

    return 1;
}


/*---------------------------------------------------
 * Func : lib_hdmi_clear_em_vsem_received_status
 *
 * Desc : clear VSEM packet receive status
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_clear_em_vsem_received_status(unsigned char nport)
{
    hdmi_mask(HDMI_VSEM_ST_reg, ~HDMI_VSEM_ST_em_vsem_no_mask, HDMI_VSEM_ST_em_vsem_no_mask);
    hdmi_mask(HDMI_VSEM_ST_reg, ~HDMI_VSEM_ST_em_vsem_first_mask, HDMI_VSEM_ST_em_vsem_first_mask);
    hdmi_mask(HDMI_VSEM_ST_reg, ~HDMI_VSEM_ST_em_vsem_last_mask, HDMI_VSEM_ST_em_vsem_last_mask);
    return 0;
}


/*---------------------------------------------------
 * Func : lib_hdmi_clear_em_vsem_sram
 *
 * Desc : clear  if Vendor Specific EM packet has been received
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : suc
 *--------------------------------------------------*/
int lib_hdmi_clear_em_vsem_sram(unsigned char nport)
{
    hdmi_mask(HDMI_HDMI_VSEM_EMC_reg, ~HDMI_HDMI_VSEM_EMC_vsem_clr_mask, HDMI_HDMI_VSEM_EMC_vsem_clr_mask);
    hdmi_mask(HDMI_HDMI_VSEM_EMC_reg, ~HDMI_HDMI_VSEM_EMC_vsem_clr_mask, 0);

    return 0;
}


/*---------------------------------------------------
 * Func : lib_hdmi_read_em_vsem_packet
 *
 * Desc : read VRR packet receive status
 *
 * Para : nport : HDMI port number
 *
 * Retn : > 0 : number of byte received, <0 : failed
 *--------------------------------------------------*/
int lib_hdmi_read_em_vsem_packet(
            unsigned char           nport,
            unsigned char           pkt_buf[EM_VSEM_INFO_LEN]
)
{
    if (pkt_buf == NULL || !lib_hdmi_is_em_vsem_received(nport)) {
        return -1;
    }

    lib_hdmi_read_emp_sram(nport, EM_VSEM_INFO_OFST + EM_VSEM_INFO_LEN - 4, 4, pkt_buf, NOT_PER_PORT); // HB0~2 + BCH

    if (pkt_buf[3] != 0) {
        HDMI_WARN("VSEM BCH Error= %02x\n", pkt_buf[3]);
    }

    // Note: HDMI 2.0 MAC has it own SRAM to restore VSEM, but uses the same SRAM address as HD21 MAC's.
    // such that we use the same Offset address for both HD20 and HD21
    lib_hdmi_read_emp_sram(nport, EM_VSEM_INFO_OFST, EM_VSEM_INFO_LEN - 4, pkt_buf + 3, NOT_PER_PORT); // PB0~PB27

    lib_hdmi_clear_em_vsem_received_status(nport);

    HDMI_EMP_DBG("VSEM= %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n",
                 pkt_buf[0], pkt_buf[1],
                 pkt_buf[2], pkt_buf[3],
                 pkt_buf[4], pkt_buf[5],
                 pkt_buf[6], pkt_buf[7],
                 pkt_buf[8], pkt_buf[9],
                 pkt_buf[10], pkt_buf[11],
                 pkt_buf[12], pkt_buf[13]);

    return 0;
}


//-------------------------------------------------------------------------
// SBTM
//-------------------------------------------------------------------------
/*---------------------------------------------------
 * Func : lib_hdmi_is_em_sbtm_received
 *
 * Desc : check if Vendor Specific EM packet has been received
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_is_em_sbtm_received(unsigned char nport)
{
    if (HDMI_SBTM_ST_get_em_sbtm_no(hdmi_in(HDMI_SBTM_ST_reg))) { // No vsem
        hdmi_out(HDMI_SBTM_ST_reg, HDMI_SBTM_ST_em_sbtm_no_mask);
        return 0;
    }

    if (!HDMI_SBTM_ST_get_em_sbtm_first(hdmi_in(HDMI_SBTM_ST_reg))) { // No First Packet
        return 0;
    }

    if (!HDMI_SBTM_ST_get_em_sbtm_last(hdmi_in(HDMI_SBTM_ST_reg))) { // No Last Packet
        return 0;
    }

    return 1;
}


/*---------------------------------------------------
 * Func : lib_hdmi_clear_em_sbtm_received_status
 *
 * Desc : clear SBTM packet receive status
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_clear_em_sbtm_received_status(unsigned char nport)
{
    hdmi_mask(HDMI_SBTM_ST_reg, ~HDMI_SBTM_ST_em_sbtm_no_mask, HDMI_SBTM_ST_em_sbtm_no_mask);
    hdmi_mask(HDMI_SBTM_ST_reg, ~HDMI_SBTM_ST_em_sbtm_first_mask, HDMI_SBTM_ST_em_sbtm_first_mask);
    hdmi_mask(HDMI_SBTM_ST_reg, ~HDMI_SBTM_ST_em_sbtm_last_mask, HDMI_SBTM_ST_em_sbtm_last_mask);
    return 0;
}


/*---------------------------------------------------
 * Func : lib_hdmi_clear_em_sbtm_sram
 *
 * Desc : clear  if Vendor Specific EM packet has been received
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : suc
 *--------------------------------------------------*/
int lib_hdmi_clear_em_sbtm_sram(unsigned char nport)
{
    hdmi_mask(HDMI_HDMI_SBTM_EMC_reg, ~HDMI_HDMI_SBTM_EMC_sbtm_clr_mask, HDMI_HDMI_SBTM_EMC_sbtm_clr_mask);
    hdmi_mask(HDMI_HDMI_SBTM_EMC_reg, ~HDMI_HDMI_SBTM_EMC_sbtm_clr_mask, 0);

    return 0;
}


/*---------------------------------------------------
 * Func : lib_hdmi_read_em_sbtm_packet
 *
 * Desc : read VRR packet receive status
 *
 * Para : nport : HDMI port number
 *
 * Retn : > 0 : number of byte received, <0 : failed
 *--------------------------------------------------*/
int lib_hdmi_read_em_sbtm_packet(
            unsigned char           nport,
            unsigned char           pkt_buf[EM_VSEM_INFO_LEN]
)
{
    if (pkt_buf == NULL || !lib_hdmi_is_em_sbtm_received(nport)) {
        return -1;
    }

    lib_hdmi_read_emp_sram(nport, EM_SBTM_INFO_OFST + EM_SBTM_INFO_LEN - 4, 4, pkt_buf, NOT_PER_PORT); // HB0~2 + BCH

    if (pkt_buf[3] != 0) {
        HDMI_WARN("SBTM BCH Error= %02x\n", pkt_buf[3]);
    }

    // Note: HDMI 2.0 MAC has it own SRAM to restore SBTM, but uses the same SRAM address as HD21 MAC's.
    // such that we use the same Offset address for both HD20 and HD21
    lib_hdmi_read_emp_sram(nport, EM_SBTM_INFO_OFST, EM_SBTM_INFO_LEN - 4, pkt_buf + 3, NOT_PER_PORT); // PB0~PB27

    lib_hdmi_clear_em_sbtm_received_status(nport);

    HDMI_EMP_DBG("SBTM= %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n",
                 pkt_buf[0], pkt_buf[1],
                 pkt_buf[2], pkt_buf[3],
                 pkt_buf[4], pkt_buf[5],
                 pkt_buf[6], pkt_buf[7],
                 pkt_buf[8], pkt_buf[9],
                 pkt_buf[10], pkt_buf[11],
                 pkt_buf[12], pkt_buf[13]);

    return 0;
}

//-------------------------------------------------------------------------
// HDR
//-------------------------------------------------------------------------


/*---------------------------------------------------
 * Func : lib_hdm_hdr_emp_sram_sel
 *
 * Desc : share sram for HDMI2.0 or HDMI2.1
 *
 * Para : nport : HDMI port number
 *        hd21  : 0 : share SRAM for HDMI 2.0
 *                1 : share SRAM for HDMI 2.1
 *
 * Retn : 0 : success
 *--------------------------------------------------*/
int lib_hdm_hdr_emp_sram_sel(
            unsigned char           nport,
            unsigned char           hd21
)
{
    hdmi_mask(HDMI_EM_CT_reg, HDMI_em_ct_hdr_hd20_hd21_sel_mask, (hd21) ? HDMI_em_ct_hdr_hd20_hd21_sel_mask : 0);
    return 0;
}

/*---------------------------------------------------
 * Func : lib_hdmi_set_em_hdr_config
 *
 * Desc : set HDR EM packet configuration
 *
 * Para : nport : HDMI port number
 *        p_emp_cfg : emp config
 *
 * Retn : 0 : successed
 *--------------------------------------------------*/
int lib_hdmi_set_em_hdr_config(
            unsigned char           nport,
            const EMP_PARAM        *p_emp_cfg
)
{
    hdmi_mask(HDMI_EM_CT_reg,                                                     // switch to hdmi 2.0
              ~(HDMI_em_ct_hdr_hd20_hd21_sel_mask |
                HDMI_em_ct_org_id_mask |
                HDMI_em_ct_set_tag_msb_mask |
                HDMI_em_ct_set_tag_lsb_mask),
              (HDMI_em_ct_hdr_hd20_hd21_sel(0) |
               HDMI_em_ct_org_id(p_emp_cfg->org_id) |
               HDMI_em_ct_set_tag_msb(p_emp_cfg->data_set_tag[1]) |
               HDMI_em_ct_set_tag_msb(p_emp_cfg->data_set_tag[0])));

    hdmi_out(HDMI_HDR_EM_CT2_reg,
             HDMI_HDR_EM_CT2_sync(p_emp_cfg->sync) |
             HDMI_HDR_EM_CT2_recognize_oui_en(p_emp_cfg->recognize_oui) |
             HDMI_HDR_EM_CT2_oui_3rd(p_emp_cfg->oui[2]) |
             HDMI_HDR_EM_CT2_oui_2nd(p_emp_cfg->oui[1]) |
             HDMI_HDR_EM_CT2_oui_1st(p_emp_cfg->oui[0]));

    return 0;
}

void _lib_hdmi_read_hdr_emp_block_sel(unsigned char nport)
{
    unsigned int block_sel = 0;
    block_sel = HDMI_hdr_st_get_em_hdr_done_block(hdmi_in(HDMI_HDRST_reg));
    hdmi_mask(HDMI_EM_CT_reg, ~(HDMI_em_ct_hdr_block_sel_mask), HDMI_em_ct_hdr_block_sel(block_sel));
}

/*---------------------------------------------------
 * Func : _lib_hdmi_read_hdr_emp_sram
 *
 * Desc : read HDR EM packet SRAM
 *
 * Para : nport : HDMI port number
 *        start_addr : start address of packet sram
 *        len   : read length
 *        pbuf  : buffer
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
void _lib_hdmi_read_hdr_emp_sram(
            unsigned char           nport,
            unsigned int            start_addr,
            unsigned short          len,
            unsigned char          *pbuf
)
{
    unsigned int i;
    unsigned int *p_val;

    if ((start_addr & 0x3) || (len & 0x3) || (((unsigned long)pbuf) & 0x3)) {
        HDMI_WARN("read hdr emp sram failed, start_addr (%d), len (%d), pbuf(%p) should be 4 bytes align\n",
                  start_addr, len, pbuf);
        return;
    }

    start_addr >>= 2;
    len >>= 2;
    p_val = (unsigned int *) pbuf;

    _lock_info_pack_sem();

    _lib_hdmi_read_hdr_emp_block_sel(nport);

    for (i = 0; i < len; i++) {
        hdmi_out(HDMI_HDRAP_reg, start_addr + i);
        *p_val++ = hdmi_in(HDMI_HDRDP_reg);
    }

    _unlock_info_pack_sem();
}


/*---------------------------------------------------
 * Func : lib_hdmi_is_em_hdr_received
 *
 * Desc : check if HDR EM packet has been received
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : not reveived, 1 : received
 *--------------------------------------------------*/
int lib_hdmi_is_em_hdr_received(unsigned char nport)
{
    if (HDMI_hdr_st_get_em_hdr_no(hdmi_in(HDMI_HDRST_reg))) { // No EMP
        hdmi_out(HDMI_HDRST_reg, HDMI_hdr_st_em_hdr_no_mask);
        return 0;
    }

    if (!HDMI_hdr_st_get_em_hdr_first(hdmi_in(HDMI_HDRST_reg))) { // No First Packet
        return 0;
    }

    if (!HDMI_hdr_st_get_em_hdr_last(hdmi_in(HDMI_HDRST_reg))) { // No Last Packet
        return 0;
    }

    // switch to write complete SRAM block.
    _lock_info_pack_sem();
    _lib_hdmi_read_hdr_emp_block_sel(nport);
    _unlock_info_pack_sem();

    return 1;
}


/*---------------------------------------------------
 * Func : lib_hdmi_clear_em_hdr_received_status
 *
 * Desc : clear HDR EM packet status
 *
 * Para : nport : HDMI port number
 *
 * Retn : 0 : suc
 *--------------------------------------------------*/
int lib_hdmi_clear_em_hdr_received_status(unsigned char nport)
{
    hdmi_out(HDMI_HDRST_reg, hdmi_in(HDMI_HDRST_reg));
    return 0;
}


/*---------------------------------------------------
 * Func : lib_hdmi_read_em_hdr_packet
 *
 * Desc : read eM packet receive status
 *
 * Para : nport : HDMI port number
 *
 * Retn : > 0 : number of byte received, <0 : failed
 *--------------------------------------------------*/
int lib_hdmi_read_em_hdr_packet(
            unsigned char           nport,
            unsigned char           pkt_buf[MAX_EM_HDR_INFO_LEN]
)
{
    int i;
    unsigned int pkt_cnt = 0;
    unsigned int len = MAX_EM_HDR_INFO_LEN;

    if (pkt_buf == NULL || !lib_hdmi_is_em_hdr_received(nport)) {
        return -1;
    }

    _lib_hdmi_read_hdr_emp_sram(nport, EM_HDR_EMP_CNT_OFST, 4, (unsigned char *) &pkt_cnt); // get EMP packet counts
    len = pkt_cnt * 32;

    HDMI_EMP_DBG("pkt_cnt=%d, len=%d\n", pkt_cnt, len);

    if (len > MAX_EM_HDR_INFO_LEN) {
        len = MAX_EM_HDR_INFO_LEN;
    }

    for (i = 0; i < len; i += 32) {
        unsigned char *p_emp = &pkt_buf[i];
        unsigned char tmp_emp[32];

        _lib_hdmi_read_hdr_emp_sram(nport, EM_HDR_EMP_INFO_OFST + i + 28, 4, p_emp); // 28~31 : HB0~2 + BCH

        // check emp BCH error
        if (p_emp[3] != 0) {
            HDMI_WARN("HDMI[p%d] HDR_EMP BCH Error detected (%p)= %02x %02x %02x %02x\n",
                      nport, p_emp, p_emp[0], p_emp[1], p_emp[2], p_emp[3]);
        }

        _lib_hdmi_read_hdr_emp_sram(nport, EM_HDR_EMP_INFO_OFST + i, 28, tmp_emp); // 0~27  : PB0~PB27
        memcpy(&p_emp[3], tmp_emp, 28);

        HDMI_EMP_DBG("HDMI[p%d] HDR_EMP[%d] Data= %02x %02x %02x %02x\n", nport, p_emp - pkt_buf, p_emp[0], p_emp[1], p_emp[2], p_emp[3]);
    }
    lib_hdmi_clear_em_hdr_received_status(nport);
    return pkt_cnt;
}
