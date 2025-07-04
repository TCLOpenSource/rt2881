#include <init.h>
#include "ros_command.h"
#include <rtk_io.h>
#include <mm/mm.h>
#include <scaler/scaler_types.h>
#include <rpc/rpcdriver.h>
#include <tvscalercontrol/hdmitx/hdmitx.h>
#include <fs.h>

//#define ErrorMessageHDMITx(fmt, ...) ROS_Printf_Module(LOGLEVEL_ERR, RTD_LOG_MODULE_SCPU1_SCALER, fmt, ##__VA_ARGS__)
//#define NoteMessageHDMITx(fmt, ...) ROS_Printf_Module(LOGLEVEL_NOTICE, RTD_LOG_MODULE_SCPU1_SCALER, fmt, ##__VA_ARGS__)
//#define DebugMessageHDMITx(fmt, ...)  ROS_Printf_Module(LOGLEVEL_DEBUG, RTD_LOG_MODULE_SCPU1_SCALER, fmt, ##__VA_ARGS__) // (x,y...) //

static HdmiTxConfigPara1 para1;
static HdmiTxConfigPara2 para2;

#ifdef CONFIG_SHELL_CMD_LINE

void hdmitx_config_dump_help_info (void)
{
    shell_printf(1,"====================HDMITX_CONFIG HELP INFO====================\n");

    //.set mem
    shell_printf(1,"[*]set hdmitx_cfg\n");
    shell_printf(1,"\t set <cmd>,<value>\n");
    shell_printf(1,"\t hdmitx_cfg platform 2 (Set platform LG_O20)\n");
    shell_printf(1,"\t hdmitx_cfg port 0 (no HDMI port num check)\n");
    shell_printf(1,"\t hdmitx_cfg check_edid 0 (no EDID Info check)\n");
    shell_printf(1,"\t hdmitx_cfg bypass_lt 1 (bypass link training)\n");
    shell_printf(1,"\t hdmitx_cfg lane_src b1 (b1 for one board, e4 for Dongle)\n");
    shell_printf(1,"\t hdmitx_cfg pn_swap f (swap all lanes for Mac6p combo board)\n");
    shell_printf(1,"\t hdmitx_cfg ptgEn 1 (Show colorbar PTG)\n");
    shell_printf(1,"\t hdmitx_cfg ptgToggle 1 (Toggle colorbar PTG)\n");
    shell_printf(1,"\t hdmitx_cfg hdcp2En 1 (HDCP enable)\n");
    shell_printf(1,"\t hdmitx_cfg hdcp2RcOff 1 (HDCP RC disable)\n");
    shell_printf(1,"\t hdmitx_cfg dscMode 1 (bbp:1=default, 2=8, 3=9.93, 4=12, 5=15)\n");
    shell_printf(1,"\t hdmitx_cfg vrr_fr 60 (VRR TX output frame rate: 47~60)\n");
    shell_printf(1,"\t hdmitx_cfg vrr_mode 1 127 (VRR Mode: 0: disable, 1/2: loop mode, Delay: 0~127 vsync)\n");
    shell_printf(1,"\t hdmitx_cfg vrr_24hz 1 (VRR Min frame rate: 0: 48Hz, 1: 24Hz)\n");

    return;
}


//hdmitx_cfg "set 18061900-18061920"
int hdmitx_config_set (char* str, char* value_str, char* value_str2)
{
    int ret;
    long result = -1;
    SYSTEM_RPC_KERNEL_PARAMETER parameter;
    unsigned int value, value2;

    if (memcmp(str,"platform",8)==0)
    {
        sscanf(value_str, "%d", &value);
        shell_printf(1,"\t platform %d\n", value);
        para2.ucPlatformType = value;
    }
    else if (memcmp(str,"port",4)==0)
    {
        sscanf(value_str, "%d", &value);
        shell_printf(1,"\t port %d\n", value);
        para1.uclimitPortNum = value;
    }
    else if (memcmp(str,"check_edid", 10)==0)
    {
        sscanf(value_str, "%d", &value);
        shell_printf(1,"\t check_edid %d\n", value);
        para1.bCheckEdidInfo = value;
    }
    else if (memcmp(str,"bypass_lt", 9)==0)
    {
        sscanf(value_str, "%d", &value);
        shell_printf(1,"\t bypass_lt %d\n", value);
        para1.bBypassLinkTraining = value;
    }
    else if (memcmp(str,"lane_src", 8)==0)
    {
        sscanf(value_str, "%x", &value);
        shell_printf(1,"\t lane_src %x\n", value);
        para2.ucLaneSrcCfg = value;
    }
    else if (memcmp(str,"pn_swap", 7)==0)
    {
        sscanf(value_str, "%x", &value);
        shell_printf(1,"\t pn_swap %x\n", value);
        para2.ucTxPnSwapCfg = value;
    }
    else if (memcmp(str,"ptgEn", 5)==0)
    {
        sscanf(value_str, "%d", &value);
        shell_printf(1,"\t ptgEn %d\n", value);
        para1.bEnablePtg = value;
    }
    else if (memcmp(str,"ptgToggle", 9)==0)
    {
        sscanf(value_str, "%d", &value);
        shell_printf(1,"\t ptgToggle %d\n", value);
        para1.bTogglePtg= value;
    }
    else if (memcmp(str,"hdcp2En", 7)==0)
    {
        sscanf(value_str, "%d", &value);
        shell_printf(1,"\t hdcp2En %d\n", value);
        para1.bEnableHdcp22= value;
    }
    else if (memcmp(str,"hdcp2RcOff", 7)==0)
    {
        sscanf(value_str, "%d", &value);
        shell_printf(1,"\t hdcp2RcOff %d\n", value);
        para1.bDisableHdcp22Rc= value;
    }
    else if (memcmp(str,"dscMode", 7)==0)
    {
        sscanf(value_str, "%d", &value);
        shell_printf(1,"\t dscMode %d\n", value);
        para1.ucDscMode= value;
    }
    else if (memcmp(str,"vrr_fr", 6)==0)
    { // VRR : set TX output frame rate: 47~60
        sscanf(value_str, "%d", &value);
        shell_printf(1,"\t vrr_fr %d\n", value);
        para2.vrr_frame_rate = value;
    }
    else if (memcmp(str,"vrr_mode", 8)==0)
    { // VRR <mode> <delay>:
        sscanf(value_str, "%d", &value);
        sscanf(value_str2, "%d", &value2);
        shell_printf(1,"\t vrr_mode %d %d\n", value, value2);
        para2.vrr_mode = value;
        para2.vrr_loop_delay = value2;
    }
    else if (memcmp(str,"vrr_24", 6)==0)
    { // VRR : set TX min output frame rate: 24hz
        sscanf(value_str, "%d", &value);
        shell_printf(1,"\t vrr_24 %d\n", value);
        para2.vrr_24hz = value;
    }
    else
    {
        ret=1;
        goto FAIL;
    }

    parameter.command = SYSTEM2_RPC_SCPU_HDMITX_CONFIG;
    parameter.param1 = para1.regValue;
    parameter.param2 = para2.regValue;
#if defined(CONFIG_RTK_H5X_RPC)
    result = sendKernelRPCtoSCPU1(parameter, NONBLOCK_MODE);
#endif

    if(result == -1) {
        ErrorMessageHDMITx("[HDMITX] command: SYSTEM2_RPC_SCPU_HDMITX_TIMING, RPC  fail ...\n");
        return -1;
    }
    else
        NoteMessageHDMITx("[HDMITX] command: SYSTEM2_RPC_SCPU_HDMITX_TIMING, RPC  success ...\n");

    return 0;

FAIL:
    shell_printf(1, "[HDMITX] fail!  invalid command! hdmitx_cfg \e[1;31m%s %s\e[0m\n",str, value_str);
    return ret;
}

#ifndef CONFIG_OTHER_TEST_DBG_CMD_DISABLE
int hdmitx_config(int argc, char **argv){
    if ((argc != 3)&& (argc != 4))
    {
        goto FAIL;
    }

    if (hdmitx_config_set(argv[1], argv[2], ((argc == 4? argv[3]: 0))))
    {
        goto FAIL;
    }
    else
    {
        goto OK;
    }

FAIL:
    hdmitx_config_dump_help_info();
    return 1;

OK:
    return 0;
}

SHELL_ADD(hdmitx_cfg, hdmitx_config);
#endif
#endif

#ifdef CONFIG_ROS_H5S2
#include "rlink_slave.h"
extern RLNK_STATUS  RLink_system_check_slave_ready (void);
#endif

int hdmitx_config_init(void) {

#if defined(CONFIG_RTK_H5X_RPC)
    long result;
#endif //#if defined(CONFIG_RTK_H5X_RPC)
    SYSTEM_RPC_KERNEL_PARAMETER parameter;
    NoteMessageHDMITx("\r\n*******************hdmitx_config_init*******************\r\n");

    // HDMITX output control
    para1.regValue = 0;
#ifdef CONFIG_HDMITX_CHECK_EDID_INFO
    para1.bCheckEdidInfo = 1; // 0: HDMITX always output, 1: HDMITX output when EDID info match define
#endif
#ifdef CONFIG_HDMITX_OUTPUT_CONFIG
    para1.uclimitPortNum = CONFIG_HDMIRX_CONNECT_PORT_NUMBER; // HDMITX output connect to HDMIRX port number constrain (0 or x): 0: no limit, x: port[x]
#endif
#ifdef CONFIG_HDMITX_BYPASS_LINK_TRAINING
    para1.bBypassLinkTraining = 1; // force bypass HPD/EDID/LinkTraining control flow
#endif
#ifdef CONFIG_HDMITX_SHOW_PATTERN_ENABLE
    para1.bEnablePtg = CONFIG_HDMITX_SHOW_PATTERN_ENABLE; // 0: mute when boot up, 1: show colorbar when boot up
#endif
#ifdef CONFIG_HDMITX_TOGGLE_PATTERN_PATTERN
    para1.bTogglePtg = CONFIG_HDMITX_TOGGLE_PATTERN_PATTERN; // 1: toggle colorbar pattern when boot up
#endif
#ifdef CONFIG_HDMITX_HDCP22_ENABLE
    para1.bEnableHdcp22 = 1; // 1: enable HDCP22 function
#endif

    // HDMITX HW config
    para2.regValue = 0;
    para2.ucPlatformType = 0; // 0: General, 1: LG_O18, 2: LG_O20
#ifdef CONFIG_HDMITX_LANE_SWAP_CONFIG
    para2.ucLaneSrcCfg = CONFIG_HDMITX_LANE_SRC_SEL; // 0xe4: H5 Demo/LG Dongle, 0xb1: LG one board
#else
    para2.ucLaneSrcCfg = 0xe4;// 0xe4: H5 Demo/LG Dongle, 0xb1: LG one board
#endif
#ifdef CONFIG_HDMITX_PN_SWAP_CONFIG
    para2.ucTxPnSwapCfg = CONFIG_HDMITX_LANE_PN_SWAP_CFG; // 0x0: default not swap, 0xf: all lane need swap
#endif

	while((*((int *)RTARC_UNCACHED_ADDRESS(SYNC_FLAG_SCPU1)) == (0xffffffff))) {
		ROSTimeDly(50);
	} //while()
#ifdef CONFIG_ROS_H5S2
    while( (RLink_system_check_slave_ready() != RLNK_OK)){
        ROSTimeDly(50);
    }//while()
#endif
    //send rpc to scpu1, to set hdmi tx input timing
    parameter.command = SYSTEM2_RPC_SCPU_HDMITX_CONFIG;
    parameter.param1 =  para1.regValue;
    parameter.param2 =  para2.regValue;
#if defined(CONFIG_RTK_H5X_RPC)
    result = sendKernelRPCtoSCPU1(parameter, NONBLOCK_MODE);
    if(result == -1)
        ErrorMessageHDMITx("[TX_TIMING] send SCPU1 kernel RPC  fail ...\n");
    else
        NoteMessageHDMITx("[TX_TIMING] send SCPU1 kernel RPC  success ...\n");
#else
        NoteMessageHDMITx("[TX_TIMING] NO RPC supported  ...\n");
#endif

    return 0;
}

//late_initcall_sync(hdmitx_config_init);
defer_initcall(hdmitx_config_init);

