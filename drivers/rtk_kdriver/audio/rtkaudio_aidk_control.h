#ifndef _LINUX_RTKAUDIO_AIDK_CONTROL_H
#define _LINUX_RTKAUDIO_AIDK_CONTROL_H

#include "AudioInbandAPI.h"

void rtkaudio_aidk_init(void);
long aidk_ioctl_set(aidk_cmd_info aidk_info);
long aidk_ioctl_set_compat(aidk_cmd_info aidk_info);
long aidk_ioctl_get(aidk_cmd_info aidk_info);
long aidk_ioctl_get_compat(aidk_cmd_info aidk_info);

#endif