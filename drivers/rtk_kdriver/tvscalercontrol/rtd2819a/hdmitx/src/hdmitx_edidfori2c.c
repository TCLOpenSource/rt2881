/********************************************************************************/
/*   The  Software  is  proprietary,  confidential,  and  valuable to Realtek   */
/*   Semiconductor  Corporation  ("Realtek").  All  rights, including but not   */
/*   limited  to  copyrights,  patents,  trademarks, trade secrets, mask work   */
/*   rights, and other similar rights and interests, are reserved to Realtek.   */
/*   Without  prior  written  consent  from  Realtek,  copying, reproduction,   */
/*   modification,  distribution,  or  otherwise  is strictly prohibited. The   */
/*   Software  shall  be  kept  strictly  in  confidence,  and  shall  not be   */
/*   disclosed to or otherwise accessed by any third party.                     */
/*   c<2003> - <2012>                                                           */
/*   The Software is provided "AS IS" without any warranty of any kind,         */
/*   express, implied, statutory or otherwise.                                  */
/********************************************************************************/

//----------------------------------------------------------------------------------------------------
// ID Code      : ScalerHdmiPhyTx.c No.0000
// Update Note  :
//----------------------------------------------------------------------------------------------------

#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_platform.h>
#include <rtk_kdriver/tvscalercontrol/hdmitx/hdmitx_common.h>
//#include <rtk_kdriver/rtk_i2c.h>
#include "../include/ScalerFunctionInclude.h"
#include "../include/ScalerTx.h"
#include "../include/hdmitx_phy_struct_def.h"
#include "../include/HdmiEdid/hdmitx_edid.h"
#include "../include/EdidCommon.h"
#include <rtk_kdriver/pcbMgr.h>


//extern EnumOutputPort ScalerHdmi21MacTxPxMapping(BYTE ucTxIndex);
#if(_HDMI_TX_SUPPORT == _ON)
#define EdidForI2cMacTxPxMapping(x) (_P0_OUTPUT_PORT)
static UINT16 u8TmpValue = 0;
static UINT16 usSinkEdidMaxSize = 0;
static UINT16 usTxEdidMaxSize = 0;
static UINT8 pucEdidResidual[16] = {0};
static UINT8 *pucDdcRamAddr = 0;
extern UINT16 pusCtaDataBlockAddr[_CTA_TOTAL_DB + 1];
extern UINT8 EDID_HDMI20[EDID_BUFFER_SIZE];
//extern PCB_ENUM_VAL HDMI_TX_I2C_BUSID;
extern I2C_HDMITX_PARA_T i2c_para_info;
extern UINT8 g_edid_policy;
UINT8 ucEEODBNum = 0;
UINT8 ucHaveEEODB = 0;

extern INT32 ScalerMcuHwIICSegmentRead(UINT8 segmentPoint, UINT8 id, UINT8 addr,
    UINT8 *p_sub_addr, UINT8 sub_addr_len,
    UINT8 *p_read_buff, UINT32 read_len, UINT16 flags, UINT8 port);
extern void moveEdidByteForward(UINT8 ucDBLengthDeviation,UINT16 usModifyStartDBAddr, UINT16 usModifyEndDBAddr);
extern void parseOversizeBlockVIC(INT8 blockNum);
extern void insertExtVIC(void);

UINT8* _ScalerOutputGetPxPortDdcRamAddr(void)
{
    return EDID_HDMI20;
}


UINT16 _ScalerEdidCheckSumCal(UINT8 *pucDdcRamAddr,UINT16 usStartAddr)
{
    UINT8 count =0;
    UINT32  checkSum =0;

    for(count =0 ;count < 0x7F; count++)
    {
        checkSum += pucDdcRamAddr[usStartAddr + count] ;
    }

    return (0xFF & (0x100 - (checkSum &0xFF)));
}

UINT8 ScalerEdidCheckSumCorrectCheck(UINT8 *pucDdcRamAddr,UINT16 usStartAddr)
{
    UINT16 checkSum;
    checkSum = _ScalerEdidCheckSumCal(pucDdcRamAddr, usStartAddr);

    if(checkSum == pucDdcRamAddr[usStartAddr+0x7F])
        return _TRUE;

    FatalMessageHDMITx("[HDMI_TX] EDID checksum iccorect %d %d\n",(UINT32)checkSum,(UINT32)pucDdcRamAddr[usStartAddr+0x7F]);
    return _FALSE;
}


//extern UINT32 dma_hdmitx_edid_phys_addr;
extern UINT8 pucExtVicLen;
extern UINT8 pucEdidVDBVIC[31];
UINT8 edid_i;
void rtdcrtl_write_edid_to_dram(void)
{
#if 0 //fix by zhaodong
    tdCtrl_Write(hdmitx_edid_tmp_addr, ptr_edid_data[edid_i]);
#endif
}
void save_edid_to_dram(UINT32 offset)
{
   // UINT32 hdmitx_edid_tmp_addr = 0;
    //UINT32 *ptr_edid_data;
    UINT16 u16EdidLen = 0;
    UINT16 u16EdidNeedWriteLen = 0;

    //hdmitx_edid_tmp_addr = dma_hdmitx_edid_phys_addr + offset;
    //hdmitx_edid_tmp_addr = offset; //fix by zhaodong
    //ptr_edid_data = (UINT32 *)EDID_HDMI20;
    u16EdidLen = 0x80 *(ucEEODBNum+1);
    if(offset)
        u16EdidNeedWriteLen = u16EdidLen - EDID_BUFFER_SIZE;
    else
        u16EdidNeedWriteLen = EDID_BUFFER_SIZE;
    for (edid_i = 0; edid_i < u16EdidNeedWriteLen/4; edid_i++){
        rtdcrtl_write_edid_to_dram();
        //hdmitx_edid_tmp_addr += 4;
    }
}
BOOLEAN readBlock(INT8 blockNum)
{
    UINT16 u16TmpValue = 0;
    UINT16 u16PrintValue = 0;
    UINT16 usEdidByteCnt = 0;
    UINT16 edidnum;

    while (u16PrintValue <= 0x70){
        usEdidByteCnt = u16PrintValue+(0x80*blockNum);
        subEdidAddr[0] = usEdidByteCnt;
        u16TmpValue = (usEdidByteCnt % 0x200);
        if(ScalerMcuHwIICSegmentRead(blockNum/2,  HDMI_TX_I2C_BUSID, _EDID_ADDRESS, subEdidAddr, 0x1, &pucDdcRamAddr[u16TmpValue], 16, 0x01, _HDMI_TX0_IIC) == _FAIL)
        {
            FatalMessageHDMITx("[HDMITX]SegRead Addr[%x] fail@readBlock()\n", (UINT32)u16PrintValue);
            return _FALSE;
        }
    #ifdef CONFIG_DUMP_EDIDINFO
            //FatalMessageHDMITx("[%x] ", (UINT32)usEdidByteCnt);

            for(edidnum = 0; edidnum < 16; edidnum++)
            {
                InfoMessageHDMITx("%x, ", (UINT32)pucDdcRamAddr[u16TmpValue + edidnum]);
            }
            InfoMessageHDMITx("\n ");
    #endif
        u16PrintValue += 0x10;
        HDMITX_MDELAY(1);
    }
    return _TRUE;
}

BOOLEAN readOversizeEdid(void)
{
        BOOLEAN btmpret = _FALSE;
        INT8 i = 0;
        pucExtVicLen = 0;
        memset(pucEdidVDBVIC, 0, EDID_VDB_VIC_LEN);//pucEdidVDBVIC :Share this buffer for dmem
        //from 8 block to 0 block
         for(i = ucEEODBNum; i >= 0; i--){
            btmpret = readBlock(i);
            if(!btmpret){
                FatalMessageHDMITx("[HDMITX]readBlock[%d] fail\n", (UINT32)i);
                return _FALSE;
            }
            if(i == 4){
                //write 4-7 block edid to share mem,offset 512
                save_edid_to_dram(EDID_BUFFER_SIZE);
                //FatalMessageHDMITx("[HDMITX]move Block[%d] to dram\n", (UINT32)i);
            }
#ifdef HDMITX_SW_CONFIG
            if(g_edid_policy  != _EDID_RX_EDID_Policy_RX_mixed_default_and_TX_by_RTD2801){
                if(i >= 4){
                    parseOversizeBlockVIC(i);
                }
                if(i == 3){
                    if(pucExtVicLen)
                        insertExtVIC();
                }
            }
#endif
         }
         //write 0-3 block edid to share mem,offset 0
         save_edid_to_dram(0);
         return _TRUE;
}
BOOLEAN getEdidBlockNum(void)
{
        UINT16 usEdidByteCnt = 0;
        usEdidByteCnt = 0x80;
        subEdidAddr[0] = usEdidByteCnt;
        if(ScalerMcuHwIICSegmentRead(_FALSE,  HDMI_TX_I2C_BUSID, _EDID_ADDRESS, subEdidAddr, 0x1, &pucDdcRamAddr[usEdidByteCnt], 16, 0x01, _HDMI_TX0_IIC) == _FAIL)
        {
            FatalMessageHDMITx("[HDMITX]SegRead Addr[%x] fail@Fail_01\n", (UINT32)usEdidByteCnt);
            return _FALSE;
        }
        if(((pucDdcRamAddr[0x84] & 0xe0) == 0xe0) && (pucDdcRamAddr[0x85] == 0x78))//_CTA_HF_EEODB
        {
            ucEEODBNum = pucDdcRamAddr[0x86];
            ucHaveEEODB = 1;
            return _TRUE;
        }
        if(!ucEEODBNum){
            usEdidByteCnt = 0x70;
            subEdidAddr[0] = usEdidByteCnt;
            if(ScalerMcuHwIICSegmentRead(_FALSE,  HDMI_TX_I2C_BUSID, _EDID_ADDRESS, subEdidAddr, 0x1, &pucDdcRamAddr[usEdidByteCnt], 16, 0x01, _HDMI_TX0_IIC) == _FAIL)
            {
                FatalMessageHDMITx("[HDMITX]SegRead Addr[%x] fail@.Fail_02\n", (UINT32)usEdidByteCnt);
                return _FALSE;
            }
            ucEEODBNum = pucDdcRamAddr[0x7E];
        }
        return _TRUE;
}
BOOLEAN getSinkEdidMaxSize(UINT16 usEdidByteCnt){
#if 0 // Ignore HF-EEODB
                    if(GET_HDMI_MAC_TX0_EDID_EXT_BLOCK_COUNT() > 0) // HF-EEODB
                        u8TmpValue = GET_HDMI_MAC_TX0_EDID_EXT_BLOCK_COUNT();
#endif
                    usSinkEdidMaxSize = 0x80 * (u8TmpValue + 1) - 0x10;
                    if(usSinkEdidMaxSize > (EDID_MAX_BUFFER_SIZE - 0x10)){
                        FatalMessageHDMITx("[HDMITX] Invalid EDID size %x\n", (UINT32)usSinkEdidMaxSize);
                        usSinkEdidMaxSize = (EDID_MAX_BUFFER_SIZE - 0x10);
                    }

                    // EDID Effective Block 0
                    if(pucDdcRamAddr[0x7E] == 0x00)
                    {
                        // Clr EDID DDCRAM that No load Value
                        memset(&pucDdcRamAddr[usEdidByteCnt + 16], 0x00, usTxEdidMaxSize - 128);
                        NoteMessageHDMITx("[HDMITX] Block[%d] SegmentRead Size=%d",(UINT32)usEdidByteCnt / 128, (UINT32)usEdidByteCnt + 16);
                        NoteMessageHDMITx("/%d/%d\n",(UINT32)usSinkEdidMaxSize, (UINT32)usTxEdidMaxSize);
                        return _FALSE;
                    }
        return _TRUE;
}
//--------------------------------------------------
// Description  : Read Downstream port EDID
// Input Value  : Reaf Type
// Output Value : None
//--------------------------------------------------
BOOLEAN ScalerHdmiMacTx0EdidIICRead(UINT8 ucReadType)
{
    UINT16 u16TmpValue = 0;
    UINT16 u16PrintValue = 0;
    UINT16 usEdidByteCnt = 0;
    //UINT16 edidnum;
    BOOLEAN btmpret = _FALSE;
    UINT8 ucSegmentPoint = 0;
    BOOLEAN bDdcRamOverflow = _FALSE;
    UINT8 *pu8TmpPoint = NULL;

    pucDdcRamAddr = NULL;
    ucHaveEEODB = 0;
    usSinkEdidMaxSize = (EDID_MAX_BUFFER_SIZE - 0x10); // SET 1024 As default
    usTxEdidMaxSize = 0;
    usTxEdidMaxSize = 0x200;
    ucEEODBNum = 0;
    pucDdcRamAddr = _ScalerOutputGetPxPortDdcRamAddr();
    getEdidBlockNum();
     if(ucEEODBNum>0x3){
        //if(g_edid_policy  == _EDID_RX_EDID_Policy_RX_mixed_default_and_TX_by_RTD2801){
            btmpret = readOversizeEdid();
            if(!btmpret){
                FatalMessageHDMITx("[HDMITX]readOversizeEdid[%d] fail\n", (UINT32)btmpret);
                return _FALSE;
            }
        //}

     }else{
        usEdidByteCnt = 0;
        if(ucReadType == _EDID_ORIGIN_READ)
        {
            FatalMessageHDMITx("[HDMITX]_EDID_ORIGIN_READ\n");
            return _FALSE;
#ifdef NOT_USED_NOW
            while (usEdidByteCnt <= 0xF0)///read 256bytes
            {

                subEdidAddr[0] = usEdidByteCnt;
                if(ScalerMcuHwIICRead(HDMI_TX_I2C_BUSID,_EDID_ADDRESS,  subEdidAddr, 0x01, &pucDdcRamAddr[usEdidByteCnt],16, 0x01,_HDMI_TX0_IIC) ==_FAIL )
                {
                    FatalMessageHDMITx("[HDMI_TX] EDID READ FAIL -01\n");
                    return _FALSE;
                }

                if(usEdidByteCnt == 0x70)
                {
                    if(ScalerEdidCheckSumCorrectCheck(pucDdcRamAddr, 0x0000) == _FALSE)
                    {
                        NoteMessageHDMITx("[HDMI_TX] EDID CRC FAIL\n");
                    }

                    // EDID Effective Block 0
                    if(pucDdcRamAddr[0x7E] == 0x00)
                    {
                        // Clr EDID index 0x80~0xFF
                        u16PrintValue = usEdidByteCnt + 16;
                        u16TmpValue = usTxEdidMaxSize - 128;
                        pu8TmpPoint = &pucDdcRamAddr[0]+u16PrintValue;
                        memset(pu8TmpPoint, 0x00, u16TmpValue);

                        NoteMessageHDMITx("[HDMI_TX] EDID Rd Size@%d\n", (UINT32)(u16PrintValue));
                        break;
                    }
                }

                if (usEdidByteCnt == 0xF0)
                {
                    if(ScalerEdidCheckSumCorrectCheck(pucDdcRamAddr, 0x0080) == _FALSE)
                    {
                        NoteMessageHDMITx("[HDMI_TX] EDID CTA CRC FAIL\n");
                    }
                    u16PrintValue = usEdidByteCnt + 16;
                    NoteMessageHDMITx("[HDMI_TX] EDID Rd Size@%d\n", (UINT32)(u16PrintValue));
                    break; // Avoid ucEdidIndex Overflow
                }
                usEdidByteCnt += 0x10;
                HDMITX_MDELAY(1);
            }
#endif
        }
        else // ucReadType == _EDID_SEGMENT_READ
        {
            while(usEdidByteCnt <= usSinkEdidMaxSize)
            {
                if(bDdcRamOverflow != _TRUE)
                {
                    subEdidAddr[0] = usEdidByteCnt;
                    if(ScalerMcuHwIICSegmentRead(ucSegmentPoint, HDMI_TX_I2C_BUSID, _EDID_ADDRESS, subEdidAddr, 0x1, &pucDdcRamAddr[usEdidByteCnt], 16, 0x01, _HDMI_TX0_IIC) == _FAIL)
                    {
                        FatalMessageHDMITx("[HDMITX] SegRead Addr[%x] fail@Read().Fail_0a\n", (UINT32)usEdidByteCnt);
                        return _FALSE;
                    }
                }
                else
                {
                    subEdidAddr[0] = usEdidByteCnt;
                    memset(pucEdidResidual, 0x00, 16);
                    if(ScalerMcuHwIICSegmentRead(ucSegmentPoint, HDMI_TX_I2C_BUSID, _EDID_ADDRESS, subEdidAddr, 0x01, &pucEdidResidual[0],16,0x01, _HDMI_TX0_IIC) == _FAIL)
                    {
                        FatalMessageHDMITx("[HDMITX]SegRead Addr[%x] fail@Fail_0b\n", (UINT32)usEdidByteCnt);
                        return _FALSE;
                    }
                }

        #ifdef CONFIG_DUMP_EDIDINFO
                //FatalMessageHDMITx("[%x] ", (UINT32)usEdidByteCnt);

                //for(edidnum = 0; edidnum < 16; edidnum++)
                {
                    InfoMessageHDMITx("0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x\n", (UINT32)pucDdcRamAddr[usEdidByteCnt],
                        (UINT32)pucDdcRamAddr[usEdidByteCnt + 1],(UINT32)pucDdcRamAddr[usEdidByteCnt + 2],(UINT32)pucDdcRamAddr[usEdidByteCnt + 3],
                        (UINT32)pucDdcRamAddr[usEdidByteCnt + 4],(UINT32)pucDdcRamAddr[usEdidByteCnt + 5],(UINT32)pucDdcRamAddr[usEdidByteCnt + 6],
                        (UINT32)pucDdcRamAddr[usEdidByteCnt + 7],(UINT32)pucDdcRamAddr[usEdidByteCnt + 8],(UINT32)pucDdcRamAddr[usEdidByteCnt + 9],
                        (UINT32)pucDdcRamAddr[usEdidByteCnt + 10],(UINT32)pucDdcRamAddr[usEdidByteCnt + 11],(UINT32)pucDdcRamAddr[usEdidByteCnt + 12],
                        (UINT32)pucDdcRamAddr[usEdidByteCnt + 13],(UINT32)pucDdcRamAddr[usEdidByteCnt + 14],(UINT32)pucDdcRamAddr[usEdidByteCnt + 15]);
                }
        #endif
                 if((usEdidByteCnt == 0x70) || (usEdidByteCnt == 0xF0) || (usEdidByteCnt == 0x170) || (usEdidByteCnt == 0x1F0))
                {
                    u16TmpValue = usEdidByteCnt - 0x70;
                    if(ScalerEdidCheckSumCorrectCheck(pucDdcRamAddr, u16TmpValue) == _FALSE){
                        return _FALSE;
                    }
                }
                // [RTKREQ-854] HDMI 1.4b CTS 4 block (512byte) EDID read test
                if(usEdidByteCnt == 0x70)
                {
                    // Check Extersion Block Number when in Base Block
                    u8TmpValue = pucDdcRamAddr[0x7E];
                    btmpret = getSinkEdidMaxSize(usEdidByteCnt);
                    if(!btmpret)
                        break;
                }
                //EEODB case
                if(usEdidByteCnt == 0x80)
                {
                     if(((pucDdcRamAddr[0x84] & 0xe0) == 0xe0) && (pucDdcRamAddr[0x85] == 0x78))//_CTA_HF_EEODB
                    {
                        // Check Extersion Block Number when in Base Block
                        u8TmpValue = pucDdcRamAddr[0x86];
                        ucEEODBNum = u8TmpValue;

                        btmpret = getSinkEdidMaxSize(usEdidByteCnt);
                    if(!btmpret)
                        break;

                    }
                }
                if((usSinkEdidMaxSize != 0x0000) && (usEdidByteCnt == usSinkEdidMaxSize))
                {
                    if(bDdcRamOverflow == _FALSE)
                    {
                        // If DDCRam not Full, clear EDID DDCRAM that No load Value
                        u16PrintValue = usEdidByteCnt + 16;
                        u16TmpValue = ((usTxEdidMaxSize - 16) - usSinkEdidMaxSize);
                        pu8TmpPoint = &pucDdcRamAddr[0]+u16PrintValue;
                        memset(pu8TmpPoint, 0x00, u16TmpValue);
                    }

                    NoteMessageHDMITx("[TX]Size Rd/Sk/Max=%d/%d",(UINT32)(usEdidByteCnt + 16), (UINT32)(usSinkEdidMaxSize + 16));
                    NoteMessageHDMITx("/%d\n",(UINT32)usTxEdidMaxSize);
                    if(usEdidByteCnt > 384) {// 4 block EDID
                        NoteMessageHDMITx("[TX]BlkTag 1/2/3=%x/%x", (UINT32)pucDdcRamAddr[EDID_BLOCK1_TAG_BASE], (UINT32)pucDdcRamAddr[EDID_BLOCK2_TAG_BASE]);
                        NoteMessageHDMITx("/%x\n", (UINT32)pucDdcRamAddr[EDID_BLOCK3_TAG_BASE]);
                    }
                    break;
                }
                else if(usEdidByteCnt == (usTxEdidMaxSize - 16))
                {
                    bDdcRamOverflow = _TRUE;
                    FatalMessageHDMITx("[HDMI_TX] EDID Over DDCRAM Size!=%d\n", (UINT32)(usSinkEdidMaxSize + 16));
                }
#if 0 // Ignore HF-EEODB
                else if(GET_HDMI_MAC_TX0_EDID_EXT_BLOCK_COUNT() && (usEdidByteCnt > 0x100) && ((usEdidByteCnt & 0xff) == 0x10)) // [FIX-ME] HF-OOEDB: read 1st 16 bytes only
                {
                    FatalMessageHDMITx("[HDMI_TX] EEODB Read 16 byte@%x\n", (UINT32)usEdidByteCnt);
                    break;
                }
#endif
                usEdidByteCnt += 0x10;
            #if 1 // [RTKREQ-854] segment pointer = usEdidByteCnt / 0x100
                ucSegmentPoint = usEdidByteCnt / 0x100;
            #else
                if(usEdidByteCnt % 0x100 == 0)
                {
                    ucSegmentPoint++;
                }
            #endif
            }
        }
    }
    // Set HDMI Tx0 Output Port Edid Read Finished
    SET_OUTPUT_DFP_DEVICE_EDID_READ_FLG(EdidForI2cMacTxPxMapping(_TX0));
    NoteMessageHDMITx("[TX]Finish,set _EDID_FLG\n");
    //#endif

    return _TRUE;
}
#endif
void ScalerHdmiMacTx0RepeaterEdidModifyVSVDB_HDR10(void)
{
        UINT16 usModifyEndDBAddr = 0;
        UINT8 ucDBLengthDeviation;
        UINT16 usModifyStartDBAddr = 0;
        UINT16 usExtDetailTimeBase = 0x82; // Detailed Timing address of Extension Block
        if(pusCtaDataBlockAddr[_CTA_VSVDB_HDR10] == 0x00)
            return;
        ucDBLengthDeviation = pucDdcRamAddr[pusCtaDataBlockAddr[_CTA_VSVDB_HDR10]]& 0x1F;
        ucDBLengthDeviation = ucDBLengthDeviation + 1;
        usModifyStartDBAddr = pusCtaDataBlockAddr[_CTA_VSVDB_HDR10];
        usExtDetailTimeBase = usModifyStartDBAddr / 0x80;
        usExtDetailTimeBase = usExtDetailTimeBase * 0x80;
        usModifyEndDBAddr = usExtDetailTimeBase + 0x7f;
        // 2.modify DTD offset
        if(usExtDetailTimeBase == EDID_EXTENSION_TAG){
            usExtDetailTimeBase = usExtDetailTimeBase + 0x2;
            pucDdcRamAddr [usExtDetailTimeBase] -= ucDBLengthDeviation;
        }
        moveEdidByteForward(ucDBLengthDeviation, usModifyStartDBAddr, usModifyEndDBAddr);
        pusCtaDataBlockAddr[_CTA_VSVDB_HDR10] = 0x00;
}
void ScalerHdmiMacTx0RepeaterEdidModifySHDR_DB(void)
{
        UINT16 usModifyEndDBAddr = 0;
        UINT8 ucDBLengthDeviation;
        UINT16 usExtDetailTimeBase = 0x82; // Detailed Timing address of Extension Block
        UINT16 usModifyStartDBAddr = 0;
        if(pusCtaDataBlockAddr[_CTA_SHDR_DB] == 0x00)
            return;
        ucDBLengthDeviation = pucDdcRamAddr[pusCtaDataBlockAddr[_CTA_SHDR_DB]]& 0x1F;
        ucDBLengthDeviation = ucDBLengthDeviation + 1;
        usModifyStartDBAddr = pusCtaDataBlockAddr[_CTA_SHDR_DB];
        usModifyEndDBAddr = usModifyStartDBAddr / 0x80;
        usModifyEndDBAddr = usModifyEndDBAddr * 0x80;
        usModifyEndDBAddr = usModifyEndDBAddr + 0x7f;
        // 2.modify DTD offset
        if(usExtDetailTimeBase == EDID_EXTENSION_TAG){
            usExtDetailTimeBase = usExtDetailTimeBase + 0x2;
            pucDdcRamAddr [usExtDetailTimeBase] -= ucDBLengthDeviation;
        }
        moveEdidByteForward(ucDBLengthDeviation, usModifyStartDBAddr, usModifyEndDBAddr);
        pusCtaDataBlockAddr[_CTA_SHDR_DB] = 0x00;
}
