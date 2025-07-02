#ifndef __DPRX_EXT_FEAT_H__
#define __DPRX_EXT_FEAT_H__
/********************************************************************************/
/*   The  Software  is  proprietary,  confidential,  and  valuable to Realtek   */
/*   Semiconductor  Corporation  ("Realtek").  All  rights, including but not   */
/*   limited  to  copyrights,  patents,  trademarks, trade secrets, mask work   */
/*   rights, and other similar rights and interests, are reserved to Realtek.   */
/*   Without  prior  written  consent  from  Realtek,  copying, reproduction,   */
/*   modification,  distribution,  or  otherwise  is strictly prohibited. The   */
/*   Software  shall  be  kept  strictly  in  confidence,  and  shall  not be   */
/*   disclosed to or otherwise accessed by any third party.                     */
/*   c<2003> - <2011>                                                           */
/*   The Software is provided "AS IS" without any warranty of any kind,         */
/*   express, implied, statutory or otherwise.                                  */
/********************************************************************************/

//--------------------------------------------------
// Func  : dprx_ext_feat_clear_hdcp_status
//
// Desc  : clear hdcp status
//
// Param : N/A
//
// Retn  : N/A
//--------------------------------------------------
void dprx_ext_feat_clear_hdcp_status(void);

//--------------------------------------------------
// Func  : dprx_ext_feat_set_hdcp_protect
//
// Desc  : set hdcp protect
//
// Param : port : dprx port
//         enable   : 0 : protect 0, others : protect 1
//
// Retn  : N/A
//--------------------------------------------------
void dprx_ext_feat_updata_hdcp_status(RTK_DPRX_ADP* p_dprx);


#endif //__DPRX_EXT_FEAT_H__
