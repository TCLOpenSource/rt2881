#include <arch.h>
#include "rtk_kdriver/uart.h"
#include "rtk_io.h"
#include "rtk_kdriver/input.h"
#include "msg_queue_def.h"
#include "kconfig_ext.h"

#include "rtk_kdriver/menu.h"

PRAGMA_CODESEG(MSG_EVENT_BANK);
PRAGMA_CONSTSEG(MSG_EVENT_BANK);

#include "rtk_kdriver/static_clib.h"


/************************
// extern function
//
*************************/
extern void HDMITX_drv_menuCmd_showTimingList(void) __banked;
extern void HDMITX_drv_menuCmd_setTxPtgTimingMode(UINT8 timeMode) __banked;
extern void HDMITX_drv_menuCmd_setTxBypassTimingMode(UINT8 outTimeMode) __banked;

/************************
// MENU Function implementation
//
*************************/
/* just using a MC_COMMANDS macro to present a list of macro entries in order,
 * that will be replace with different declaration or definition in order.
 * reference to wiki page of x-macro, https://en.wikipedia.org/wiki/X_Macro
 */

/* MENU_COMMAND entries definition guide:
 * there are 3 kinds of entries:
 * MENU(kconfig, key, handler, local_banked) to define a hirachy level of menu structure:
 *   KCONFIG : the option to activate this X entry or not.
 *   KEY     : the key to the this MENU  entry
 *   LOCAL_BANKED :  whether the handler invocation is banked (IS_BANKED) or not (IS_LOCAL),
 *
 * INST(kconfig, key, handler, local_banked) to define a isntant command entry:
 *   KCONFIG : the option to activate this X entry or not.
 *   KEY     : the key to the this MENU_COMMAND entry
 *   LOCAL_BANKED :  whether the handler invocation is banked (IS_BANKED) or not (IS_LOCAL),
 *
 * INPUT(kconfig, key, handler, local_banked, prompt) to define a command entry with input line.
 *   KCONFIG : the option to activate this X entry or not.
 *   KEY     : the key to the this MENU_COMMAND entry
 *   LOCAL_BANKED :  whether the handler invocation is banked (IS_BANKED) or not (IS_LOCAL) with respect to MSG_MENU_BANKE (BANK1),
 *   PROMPT : a string to print before waiting for line input.
 *
 */

/* NOTICE: the entries should be put in ascending order of KEY. MAX of key is 255 */
/* NOTICE: the entries should be put in ascending order of KEY. MAX of key is 255 */
/* NOTICE: the entries should be put in ascending order of KEY. MAX of key is 255 */
#define HDMITX_MENU(INST, INPUT, MENU)                     \
        MC_STARTL()                                      \
                INST(1, 5, tx_showTimeList, IS_BANKED)       \
                INPUT(1, 6, tx_setPtgTimeMode, IS_BANKED, "Set TX PTG time mode: %d\n")         \
                INPUT(1, 7, tx_setBypassTimeMode, IS_BANKED, "Set TX bypass time mode: %d\n")         \
        MC_ENDL()

MENU_DEFINE(HDMITX_MENU, hdmitx_menu);



void tx_showTimeList(void *pdata) __banked
{
        //DebugMessageHDMITx(LOGGER_ERROR, "tx_showTimeList: this is a tx_showTimeList \n");
        HDMITX_drv_menuCmd_showTimingList();
}

#define argv global_argv
#define argc global_argc

void tx_setPtgTimeMode(void *pdata) __banked
{
    UINT8 value;
    UINT8 idx;

    argc = parse_opts(((UINT8 *)pdata), argv, 1);
    //DebugMessageHDMITx(LOGGER_ERROR, "input time mode: %s , argc: %x \n", (UINT8 *)pdata, (UINT32)argc);

    value = simple_strtoul(argv[0]);
    DebugMessageHDMITx("Set TX PTG Time Mode= %x\n",  (UINT32)value);

    // set TX output timing mode and TX run in PTG mode
    HDMITX_drv_menuCmd_setTxPtgTimingMode(value);
}

void tx_setBypassTimeMode(void *pdata) __banked
{
    UINT8 value;
    UINT8 idx;

    argc = parse_opts(((UINT8 *)pdata), argv, 1);
    //DebugMessageHDMITx(LOGGER_ERROR, "input time mode: %s , argc: %x \n", (UINT8 *)pdata, (UINT32)argc);

    value = simple_strtoul(argv[0]);
    DebugMessageHDMITx(LOGGER_ERROR, "Set TX Bypass Time Mode= %x\n",  (UINT32)value);

    // set TX output timing mode by force ScalerHdmiMacTx0RepeaterGetTxOutputModeTiming() return value
    HDMITX_drv_menuCmd_setTxBypassTimingMode(value);
}


