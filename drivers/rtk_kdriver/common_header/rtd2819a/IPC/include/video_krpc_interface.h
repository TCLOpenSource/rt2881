#ifndef __VIDEO_KRPC_INTERFACE_H__
#define __VIDEO_KRPC_INTERFACE_H__

#define RPC_VCPU_ID_0x150_SYSTEM_TO_VIDEO_GENERIC 0x150
#define RPC_VCPU_ID_0x151_SYSTEM_TO_VIDEO_VDEC 0x151

#define SUBID_VDEC_SET_NV12_BUFFERPOOL 100

typedef struct {
	unsigned int type;
	unsigned int portID;
	unsigned int payloadSize;
	unsigned int payloadAddr;
	unsigned int resultSize;
	unsigned int resultAddr;
	unsigned int param1;
	unsigned int param2;
} video_krpc_generic_hdr_t;

#endif
