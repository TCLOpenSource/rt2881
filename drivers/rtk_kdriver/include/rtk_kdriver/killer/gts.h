/**
 * This is white list which can not be killed.
 * Otherwise, GTS would be not passed.
 */

#ifndef __KILLER_GTS_H__
#define __KILLER_GTS_H__

char *gts_disable_kill_list[] = {
    /***** CtsAlarmManagerTestCases *****/
    "com.google.android.gts.security",
};

char *gts_no_kill_list[] = {
    /* com.android.rkpdapp */
    "rkpdapp",
};

inline bool should_disable_kill_gts_task(const char *task_name)
{
    bool ret = false;
    int i = 0;

    /* This kill list is from GTS test */
    for (i = 0; i < sizeof(gts_disable_kill_list) / sizeof(gts_disable_kill_list[0]); i++) {
        if (strstr(gts_disable_kill_list[i], task_name)) {
            ret = true;
            break;
        }
    }
    return ret;
}

inline bool is_gts_task(const char *task_name)
{
    bool ret = false;
    int i = 0;

    /* This no kill list is from GTS test */
    for (i = 0; i < sizeof(gts_no_kill_list) / sizeof(gts_no_kill_list[0]); i++) {
        if (strstr(task_name, gts_no_kill_list[i])) {
            ret = true;
            break;
        }
    }
    return ret;
}
#endif /* __KILLER_GTS_H__ */
