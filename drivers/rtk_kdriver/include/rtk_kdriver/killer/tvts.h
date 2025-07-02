/**
 * This is white list which can not be killed.
 * Otherwise, TVTS would be not passed.
 */

#ifndef __KILLER_TVTS_H__
#define __KILLER_TVTS_H__

char *tvts_task_list[] = {
    /***** TvtsAssistant4TestCases *****/
    // "com.google.android.assistant.tvts",
    "assistant.tvts",

    /***** TvtsAppStartTimesHostTestCases *****/
    // "com.google.android.leanbackjank",
    "leanbackjank",

    /***** TvtsAppStartTimesHostTestCases *****/
    // "com.google.android.tvts.testscorecardexoplayer",
    "ExoPlayer",
};

char *tvts_no_kill_list[] = {
    /***** TvtsAssistant4TestCases *****/
    "katniss",

    /***** TvtsAppStartTimesHostTestCases *****/
    "youtube",
    "netflix",
    "vending",

    /***** TvtsAppStartTimesHostTestCases *****/
    "launcherx",
    "leanbackjank",
};

inline bool should_not_kill_tvts_task(const char *task_name)
{
    bool ret = false;
    int i = 0;

    for (i = 0; i < sizeof(tvts_no_kill_list) / sizeof(tvts_no_kill_list[0]); i++) {
        if (strstr(task_name, tvts_no_kill_list[i])) {
            ret = true;
            break;
        }
    }
    return ret;
}

inline bool is_tvts_task(const char *task_name)
{
    bool ret = false;
    int i = 0;

    for (i = 0; i < sizeof(tvts_task_list) / sizeof(tvts_task_list[0]); i++) {
        if (strstr(task_name, tvts_task_list[i])) {
            ret = true;
            break;
        }
    }
    return ret;
}
#endif /* __KILLER_TVTS_H__ */
