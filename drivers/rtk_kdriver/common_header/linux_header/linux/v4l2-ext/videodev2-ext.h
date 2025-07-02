#ifndef _VIDEODEV2_EXT_H
#define _VIDEODEV2_EXT_H

/* FIXME: gstreamer-good uses uapi/linux/videodev2.h,
 * so add to check _UAPI_XX to prevent redefinition error
 */
#ifndef _UAPI__LINUX_VIDEODEV2_H
#include <linux/videodev2.h>
#endif

#include <linux/v4l2-ext/v4l2-ext-broadcast.h>

#endif
