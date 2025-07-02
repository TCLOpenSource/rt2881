#ifndef _UVC_DEVICE_ID_H_
#define _UVC_DEVICE_ID_H_
#define UVC_DEVICE_NUM                  1
#define UVC_DEVICE_NAME                 "uvcdev"
#define UVC_IOC_MAGIC                   'g'
#define UVC_MAJOR                       0   /* dynamic major by default */
#define UVC_NR_DEVS                     1    /* se0 through se3 */
#define UVC_IOC_CONNECT                 _IOW(UVC_IOC_MAGIC, 0, UVC_CONNECT_PARA_T *)
#define UVC_IOC_SET_WINBLANK            _IOW(UVC_IOC_MAGIC, 1, UVC_WINBLANK_PARA_T *)
#define UVC_IOC_SET_REGION_CTRL         _IOW(UVC_IOC_MAGIC, 2, UVC_REGION_CTRL_PARA_T *)
#define UVC_IOC_MAXNR 					3

typedef enum _UVC_CAPTURE_FORMAT_TYPE
{
    _NONE = 0,
    _NV12,
    _M420,
    _I420,
    _YUYV,
    _RGB24,
    _RGB32,
    _P010,
    _MotionJPEG,
    _IYU2,
    _AYUV,
    _V410,
    _Y410,
    _UYVY,
    _V210,
    _Y210,
    _YV12,
    _RGB565,
    _RGB555,
} UVC_CAPTURE_FORMAT_TYPE;

/*
UVC_IOC_SET_WINBLANK
*/
typedef enum
{
    KADP_UVC_WIN_COLOR_BLACK,		/**< window color black */
    KADP_UVC_COLOR_COLOR_RGB_API,		/**< window color form RGB API VBE_IOC_SET_FORCEBG_COLOR*/
} KADP_UVC_WIN_COLOR_T;

typedef struct
{
    bool winBlankBEnable;
    KADP_UVC_WIN_COLOR_T winBlankColor;
} UVC_WINBLANK_PARA_T;

#ifndef VIDEO_RECT_T
typedef struct
{
    unsigned short		x;	/**< horizontal Start Position in pixel [0,].*/
    unsigned short		y;	/**< vertical	Start Position in pixel [0,].*/
    unsigned short		w;	/**< horizontal Size(Width)    in pixel [0,].*/
    unsigned short		h;	/**< vertical	Size(Height)   in pixel [0,].*/
} __VIDEO_RECT_T;
#define VIDEO_RECT_T __VIDEO_RECT_T
#endif

#ifndef VSC_OUTPUT_POSITION_T
typedef enum
{
    VSC_OUTPUT_POSITION_MAIN = 0,
    VSC_OUTPUT_POSITION_SUB,
    VSC_OUTPUT_POSITION_MAIN_SUB,
    VSC_OUTPUT_POSITION_OSD1,
    VSC_OUTPUT_POSITION_OSD2,
    VSC_OUTPUT_POSITION_OSD3,
    VSC_OUTPUT_POSITION_OSD4,
    VSC_OUTPUT_POSITION_OSD5,
} _VSC_OUTPUT_POSITION_T;
#define VSC_OUTPUT_POSITION_T _VSC_OUTPUT_POSITION_T
#endif

#ifndef UVC_WINDOW_ZOOM_TYPE
typedef enum
{
    FIT_WINDOW = 0,
    KEEP_ASPECT_RATIO
} _UVC_WINDOW_ZOOM_TYPE;
#define UVC_WINDOW_ZOOM_TYPE _UVC_WINDOW_ZOOM_TYPE
#endif

/*
UVC_IOC_CONNECT
*/
typedef struct
{
    bool bConnect;
    VSC_OUTPUT_POSITION_T inputPosition;
    UVC_CAPTURE_FORMAT_TYPE captureFormat;
} UVC_CONNECT_PARA_T;

/*
UVC_IOC_SET_REGION_CTRL
*/
typedef struct
{
    unsigned int UID;
    VIDEO_RECT_T cropWin;
    VIDEO_RECT_T oriWin;
    VIDEO_RECT_T outputWin;
    VIDEO_RECT_T zoomWin;
    UVC_WINDOW_ZOOM_TYPE zoomType;
    unsigned int outputFrameRate;
} UVC_REGION_CTRL_PARA_T;

#endif // #ifndef _UVC_DEVICE_ID_H_
