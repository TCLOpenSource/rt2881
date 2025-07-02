/**
 * This is white list which can not be killed.
 * Otherwise, CTS would be not passed.
 */

#ifndef __KILLER_CTS_H__
#define __KILLER_CTS_H__

char *cts_disable_kill_list[] = {
    /***** CtsAppExitTestCases *****/
    "android.app.stubs",
    "android.externalservice.service",
    "android.app.cts.appexit",
    "com.android.cts.launcherapps.simpleapp",

    /***** CtsAppTestCases *****/
    "com.android.app1",
    "com.android.app2",
    "com.android.app3",
    "com.android.test.cantsavestate1",
    "com.android.test.cantsavestate2",
    "com.example.wallpapertest",

    /***** CtsKeystoreTestCases *****/
    "android.keystore.cts",
};

char *cts_no_kill_list[] = {
    /***** CtsAlarmManagerTestCases *****/
    "alarmmanager",
    // "android.alarmmanager.alarmtestapp.cts.sdk30",
    // "android.alarmmanager.alarmtestapp.cts",
    // "android.alarmmanager.alarmtestapp.cts.policy_permission_32",
    // "android.alarmmanager.alarmtestapp.cts.user_permission_32",
    // "android.alarmmanager.cts",

    /***** CtsAppExitTestCases *****/
    "cts",
    "stubs",
    "externalservice",
    // "android.app.stubs",
    // "android.externalservice.service",
    // "com.android.cts.launcherapps.simpleapp",
    // "android.app.cts.appexit",

    /***** CtsAppTestCases *****/
    "app1",
    "app2",
    "app3",
    "app4",
    "cantsavestate",
    "storagedelegator",
    "wallpapertest",
    // "android.app.cts.activitymanager.api29",
    // "com.android.app4",
    // "android.app.stubs",
    // "com.android.app1",
    // "com.android.app2",
    // "com.android.app3",
    // "com.android.cts.stubbad",
    // "com.android.test.cantsavestate1",
    // "com.android.test.cantsavestate2",
    // "com.android.cts.launcherapps.simpleapp",
    // "com.android.test.storagedelegator",
    // "com.example.wallpapertest",
    // "android.app.cts",

    /***** CtsAutoFillServiceTestCases *****/
    // "com.android.cts.mockime",
    // "android.autofill.cts2",
    // "android.autofillservice.cts",

    /***** CtsBroadcastTestCases *****/
    // "com.android.app.cts.broadcasts.helper2",
    // "com.android.app.cts.broadcasts.helper",
    // "com.android.cts.broadcasts",

    /***** CtsDisplayTestCases *****/
    // "android.app.stubs",
    // "android.display.cts",

    /***** CtsDynamicMimeHostTestCases *****/
    "dynamicmime",
    // "android.dynamicmime.helper",
    // "android.dynamicmime.preferred",
    // "android.dynamicmime.testapp",
    // "android.dynamicmime.update",

    /***** CtsHostsideNetworkTests *****/
    // "com.android.cts.net.hostside.networkslicingtestapp",
    // "com.android.cts.net.hostside.app2",
    // "com.android.cts.net.hostside",

    /***** CtsKeystoreTestCases *****/
    // "android.keystore.cts",

    /***** CtsMediaAudioTestCases *****/
    // "android.media.audio.cts",

    /***** CtsSurfaceControlTests *****/
    // "android.view.surfacecontrol.cts",

    /***** CtsSyncManagerTestsCases *****/
    // "android.content.syncmanager.cts.app1",
    // "android.content.syncmanager.cts.app2",
    // "android.content.syncmanager.cts",

    /***** CtsWindowManagerBackgroundActivityTestCases *****/
    "server.wm",
    // "android.server.wm.backgroundactivity.appa33",
    // "android.server.wm.backgroundactivity.appa",
    // "android.server.wm.backgroundactivity.appb33",
    // "android.server.wm.backgroundactivity.appb",
    // "android.server.wm.backgroundactivity.appc33",
    // "android.server.wm.backgroundactivity.appc",

    /***** CtsWindowManagerDeviceTestCases *****/
    // "android.server.wm.alertwindowservice",
    // "android.server.wm.backlegacyapp",
    // "android.server.wm.scvh",
    // "android.server.wm.alertwindowapp",
    // "android.server.wm.alertwindowappsdk25",
    // "android.server.wm.deprecatedabi",
    // "android.server.wm.deprecatedsdk",
    // "android.server.wm.deskresources",
    // "android.server.wm.displaysize",
    // "android.server.wm.prerelease",
    // "android.server.wm.profileable",
    // "android.server.wm.app27",
    // "android.server.wm.app30",
    // "android.server.wm.app",
    // "android.server.wm.second",
    // "android.server.wm.shareuid.a",
    // "android.server.wm.shareuid.b",
    // "android.server.wm.third",
    // "android.server.wm.translucentapp26",
    // "android.server.wm.translucentapp",
    // "android.server.wm.dndsourceapp",
    // "android.server.wm.dndtargetapp",
    // "android.server.wm.dndtargetappsdk23",
    // "com.android.cts.mockime",
    // "android.server.wm.propertycameracompatallowforcerotation",
    // "android.server.wm.propertycameracompatallowrefresh",
    // "android.server.wm.propertycameracompatenablerefreshviapauseoptin",
    // "android.server.wm.propertycameracompatenablerefreshviapauseoptout",
    // "android.server.wm.allowdisplayorientationoverride",
    // "android.server.wm.alloworientationoverride",
    // "android.server.wm.allowsandboxingviewboundsapis",
    // "android.server.wm.enablefakefocusoptin",
    // "android.server.wm.enablefakefocusoptout",
    // "android.server.wm.optoutsandboxingviewboundsapis",
    // "android.server.wm.ignorerequestedorientationoverrideoptin",
    // "android.server.wm.ignorerequestedorientationoverrideoptout",
    // "android.server.wm.jetpack.second",

    /***** CtsMediaMiscTestCases *****/
    // "android.media.misc.cts",
};

inline bool should_disable_kill_cts_task(const char *task_name)
{
    bool ret = false;
    int i = 0;

    /* This kill list is from CTS test */
    for (i = 0; i < sizeof(cts_disable_kill_list) / sizeof(cts_disable_kill_list[0]); i++) {
        if (strstr(cts_disable_kill_list[i], task_name)) {
            ret = true;
            break;
        }
    }
    return ret;
}

inline bool is_cts_task(const char *task_name)
{
    bool ret = false;
    int i = 0;

    /* This no kill list is from CTS test */
    for (i = 0; i < sizeof(cts_no_kill_list) / sizeof(cts_no_kill_list[0]); i++) {
        if (strstr(task_name, cts_no_kill_list[i])) {
            ret = true;
            break;
        }
    }
    return ret;
}
#endif /* __KILLER_CTS_H__ */
