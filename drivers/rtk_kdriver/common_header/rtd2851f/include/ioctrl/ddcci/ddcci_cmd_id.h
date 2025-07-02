#ifndef __DDCCI_CMD_ID__
#define __DDCCI_CMD_ID__


#define DDC_BUF_LENGTH      32

typedef struct {
    unsigned char      buf[DDC_BUF_LENGTH];
    unsigned char      len;
}ddcci_msg;

#define DDCCI_IOC_MAGIC                  'y'
#define DDCCI_IOC_ENABLE                  _IOW(DDCCI_IOC_MAGIC, 1, unsigned char)
#define DDCCI_IOC_SEND_MESSAGE            _IOW(DDCCI_IOC_MAGIC, 2, ddcci_msg)
#define DDCCI_IOC_RCV_MESSAGE             _IOR(DDCCI_IOC_MAGIC, 3, ddcci_msg)
#define DDCCI_IOC_DISABLE                 _IOW(DDCCI_IOC_MAGIC, 4, unsigned char)


#endif /* __DDCCI_CMD_ID__ */
