#ifndef __QUICK_SHOW_CMD_ID__
#define __QUICK_SHOW_CMD_ID__


enum RTK_QS_SOURCE {
    QS_SOURCE_NONE,
    QS_SOURCE_HDMI,
    QS_SOURCE_DP,
};

typedef struct {
    enum RTK_QS_SOURCE source;
    unsigned int portnum;
} RTK_QS_SOURCE_PORT_INFO;


enum RTK_QS_SND_DEV {
    QS_SND_DEV_NONE,
    QS_SND_DEV_SPEAKER,
    QS_SND_DEV_HEADPHONE,
};

typedef struct {
    enum RTK_QS_SND_DEV dev;
    unsigned int volume;
    int is_mute;
} RTK_QS_SND_DEV_INFO;



#define RTK_QS_IOC_MAGIC                  't'
#define RTK_QS_IOC_GET_ACTIVE_STATE      _IOR(RTK_QS_IOC_MAGIC, 1, int)
#define RTK_QS_IOC_SET_INACTIVE          _IO(RTK_QS_IOC_MAGIC, 2)
#define RTK_QS_IOC_GET_SOURCE_PORT       _IOR(RTK_QS_IOC_MAGIC, 3, RTK_QS_SOURCE_PORT_INFO)
#define RTK_QS_IOC_GET_VOLUME            _IOR(RTK_QS_IOC_MAGIC, 4, RTK_QS_SND_DEV_INFO)
#define RTK_QS_IOC_SET_NOT_HANDLE_KEY    _IO(RTK_QS_IOC_MAGIC, 5)
#define RTK_QS_IOC_STOP_SM               _IO(RTK_QS_IOC_MAGIC, 6)
#define RTK_QS_IOC_GET_DP_EOTFTYPE       _IOR(RTK_QS_IOC_MAGIC, 7, unsigned int)
#define RTK_QS_IOC_GET_HDMI_EOTFTYPE     _IOR(RTK_QS_IOC_MAGIC, 8, unsigned int)



#endif /* __QUICK_SHOW_CMD_ID__ */
