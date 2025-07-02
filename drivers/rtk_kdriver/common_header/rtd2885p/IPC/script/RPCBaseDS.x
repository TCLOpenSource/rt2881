
.insert <rpc_common.h>

typedef rpc_s32_t HRESULT;

/* The "data" holds the instance ID of VP filters */
struct RPCRES_LONG {
    HRESULT     result;
    rpc_s32_t   data;
};

struct USB_DEVICE_CONFIG_INFO
{
    HRESULT ret;
    rpc_s32_t usb_ai_samplerate; /* bit 0~13: 32/44.1/48/88.2/96/176.4/192/22.05/24/64/16/8/11.025/12 kHz */
    rpc_s32_t usb_ai_format;     /* ref to USB_AUDIO_IN_SAMPLE_FORMAT, always big-endian */
    rpc_s32_t usb_ai_chnum;      /* up to 2-ch currently */
};

struct RPC_CONNECTION {
    rpc_s32_t        srcInstanceID;   /* Source Filter instance, returned by VP_Create */
    rpc_s32_t        srcPinID;        /* Source Pin */
    rpc_s32_t        desInstanceID;   /* Destination Filter instance, returned by VP_Create */
    rpc_s32_t        desPinID;        /* Destination Pin */
    rpc_s32_t        mediaType;
};

struct RPC_SEND_LONG {
    rpc_s32_t        instanceID;
    rpc_s32_t        pinID;
    rpc_s32_t        data;
};

/* These structures should be passed by reference, are defined in rpc_common.h
struct RPC_RINGBUFF_INFO {
    u_long      pWrPtr;
    u_long      pRdPtr;
    u_long      beginPhysicalAddress;
    long        bufferSize;
    long				RBUF_ID ;
};
*/
/*
struct RPC_RINGBUFFER {
    long        instanceID;
    long        pinID;
    long        readPtrIndex;

    long 	pRINGBUFF_HEADER;
};
*/
