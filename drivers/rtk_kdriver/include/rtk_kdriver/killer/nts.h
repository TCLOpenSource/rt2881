/**
 * This is white list which can not be killed.
 * Otherwise, NTS would be not passed.
 */

#ifndef __KILLER_CTS_H__
#define __KILLER_CTS_H__
char *nts_task_list[] = {
    /***** HDMI-SINK-CEC-ON-STANDBY *****/
    "netflix",
};
#endif /* __KILLER_CTS_H__ */
