#ifndef __UAPI_LINUX_USB_CDC_H
#define __UAPI_LINUX_USB_CDC_H

#define le16 unsigned short
#define le32 UINT32
#define __le16 unsigned short

//#define u8 UINT8
//#define u16 UINT16
//#define u32 UINT32


#define USB_DIR_OUT			0		/* to device */
#define USB_DIR_IN			0x80		/* to host */

/*
 * USB types, the second of three bRequestType fields
 */
#define USB_TYPE_MASK			(0x03 << 5)
#define USB_TYPE_STANDARD		(0x00 << 5)
#define USB_TYPE_CLASS			(0x01 << 5)
#define USB_TYPE_VENDOR			(0x02 << 5)
#define USB_TYPE_RESERVED		(0x03 << 5)

/*
 * USB recipients, the third of three bRequestType fields
 */
#define USB_RECIP_MASK			0x1f
#define USB_RECIP_DEVICE		0x00
#define USB_RECIP_INTERFACE		0x01
#define USB_RECIP_ENDPOINT		0x02
#define USB_RECIP_OTHER			0x03
/* From Wireless USB 1.0 */
#define USB_RECIP_PORT			0x04
#define USB_RECIP_RPIPE		0x05

/*
 * Standard requests, for the bRequest field of a SETUP packet.
 *
 * These are qualified by the bRequestType field, so that for example
 * TYPE_CLASS or TYPE_VENDOR specific feature flags could be retrieved
 * by a GET_STATUS request.
 */
#define USB_REQ_GET_STATUS		0x00
#define USB_REQ_CLEAR_FEATURE		0x01
#define USB_REQ_SET_FEATURE		0x03
#define USB_REQ_SET_ADDRESS		0x05
#define USB_REQ_GET_DESCRIPTOR		0x06
#define USB_REQ_SET_DESCRIPTOR		0x07
#define USB_REQ_GET_CONFIGURATION	0x08
#define USB_REQ_SET_CONFIGURATION	0x09
#define USB_REQ_GET_INTERFACE		0x0A
#define USB_REQ_SET_INTERFACE		0x0B
#define USB_REQ_SYNCH_FRAME		0x0C
#define USB_REQ_SET_SEL			0x30
#define USB_REQ_SET_ISOCH_DELAY		0x31

#define USB_REQ_SET_ENCRYPTION		0x0D	/* Wireless USB */
#define USB_REQ_GET_ENCRYPTION		0x0E
#define USB_REQ_RPIPE_ABORT		0x0E
#define USB_REQ_SET_HANDSHAKE		0x0F
#define USB_REQ_RPIPE_RESET		0x0F
#define USB_REQ_GET_HANDSHAKE		0x10
#define USB_REQ_SET_CONNECTION		0x11
#define USB_REQ_SET_SECURITY_DATA	0x12
#define USB_REQ_GET_SECURITY_DATA	0x13
#define USB_REQ_SET_WUSB_DATA		0x14
#define USB_REQ_LOOPBACK_DATA_WRITE	0x15
#define USB_REQ_LOOPBACK_DATA_READ	0x16
#define USB_REQ_SET_INTERFACE_DS	0x17

/* The Link Power Management (LPM) ECN defines USB_REQ_TEST_AND_SET command,
 * used by hubs to put ports into a new L1 suspend state, except that it
 * forgot to define its number ...
 */

/*
 * USB feature flags are written using USB_REQ_{CLEAR,SET}_FEATURE, and
 * are read as a bit array returned by USB_REQ_GET_STATUS.  (So there
 * are at most sixteen features of each type.)  Hubs may also support a
 * new USB_REQ_TEST_AND_SET_FEATURE to put ports into L1 suspend.
 */
#define USB_DEVICE_SELF_POWERED		0	/* (read only) */
#define USB_DEVICE_REMOTE_WAKEUP	1	/* dev may initiate wakeup */
#define USB_DEVICE_TEST_MODE		2	/* (wired high speed only) */
#define USB_DEVICE_BATTERY		2	/* (wireless) */
#define USB_DEVICE_B_HNP_ENABLE		3	/* (otg) dev may initiate HNP */
#define USB_DEVICE_WUSB_DEVICE		3	/* (wireless)*/
#define USB_DEVICE_A_HNP_SUPPORT	4	/* (otg) RH port supports HNP */
#define USB_DEVICE_A_ALT_HNP_SUPPORT	5	/* (otg) other RH port does */
#define USB_DEVICE_DEBUG_MODE		6	/* (special devices only) */

/*
 * Test Mode Selectors
 * See USB 2.0 spec Table 9-7
 */
#define	TEST_J		1
#define	TEST_K		2
#define	TEST_SE0_NAK	3
#define	TEST_PACKET	4
#define	TEST_FORCE_EN	5

/*
 * New Feature Selectors as added by USB 3.0
 * See USB 3.0 spec Table 9-7
 */
#define USB_DEVICE_U1_ENABLE	48	/* dev may initiate U1 transition */
#define USB_DEVICE_U2_ENABLE	49	/* dev may initiate U2 transition */
#define USB_DEVICE_LTM_ENABLE	50	/* dev may send LTM */
#define USB_INTRF_FUNC_SUSPEND	0	/* function suspend */

#define USB_INTRF_FUNC_SUSPEND_LOW_POWER_SUSPEND_STATE 0x0100

#define USB_INTR_FUNC_SUSPEND_OPT_MASK	0xFF00
/*
 * Suspend Options, Table 9-8 USB 3.0 spec
 */
#define USB_INTRF_FUNC_SUSPEND_LP	(1 << (8 + 0))
#define USB_INTRF_FUNC_SUSPEND_RW	(1 << (8 + 1))

/*
 * Interface status, Figure 9-5 USB 3.0 spec
 */
#define USB_INTRF_STAT_FUNC_RW_CAP     1
#define USB_INTRF_STAT_FUNC_RW         2

#define USB_ENDPOINT_HALT		0	/* IN/OUT will STALL */

/* Bit array elements as returned by the USB_REQ_GET_STATUS request. */
#define USB_DEV_STAT_U1_ENABLED		2	/* transition into U1 state */
#define USB_DEV_STAT_U2_ENABLED		3	/* transition into U2 state */
#define USB_DEV_STAT_LTM_ENABLED	4	/* Latency tolerance messages */

/**
 * struct usb_ctrlrequest - SETUP data for a USB device control request
 * @bRequestType: matches the USB bmRequestType field
 * @bRequest: matches the USB bRequest field
 * @wValue: matches the USB wValue field (le16 byte order)
 * @wIndex: matches the USB wIndex field (le16 byte order)
 * @wLength: matches the USB wLength field (le16 byte order)
 *
 * This structure is used to send control requests to a USB device.  It matches
 * the different fields of the USB 2.0 Spec section 9.3, table 9-2.  See the
 * USB spec for a fuller description of the different fields, and what they are
 * used for.
 *
 * Note that the driver for any interface can issue control requests.
 * For most devices, interfaces don't coordinate with each other, so
 * such requests may be made at any time.
 */
#pragma pack(push,1)
struct usb_ctrlrequest {
	unsigned char bRequestType;
	unsigned char bRequest;
	unsigned short wValue;
	unsigned short wIndex;
	unsigned short wLength;
};
#pragma pack(pop)
/*-------------------------------------------------------------------------*/

/*
 * Descriptor types ... USB 2.0 spec table 9.5
 */
#define USB_DT_DEVICE			0x01
#define USB_DT_CONFIG			0x02
#define USB_DT_STRING			0x03
#define USB_DT_INTERFACE		0x04
#define USB_DT_ENDPOINT			0x05
#define USB_DT_DEVICE_QUALIFIER		0x06
#define USB_DT_OTHER_SPEED_CONFIG	0x07
#define USB_DT_INTERFACE_POWER		0x08
/* these are from a minor usb 2.0 revision (ECN) */
#define USB_DT_OTG			0x09
#define USB_DT_DEBUG			0x0a
#define USB_DT_INTERFACE_ASSOCIATION	0x0b
/* these are from the Wireless USB spec */
#define USB_DT_SECURITY			0x0c
#define USB_DT_KEY			0x0d
#define USB_DT_ENCRYPTION_TYPE		0x0e
#define USB_DT_BOS			0x0f
#define USB_DT_DEVICE_CAPABILITY	0x10
#define USB_DT_WIRELESS_ENDPOINT_COMP	0x11
#define USB_DT_WIRE_ADAPTER		0x21
#define USB_DT_RPIPE			0x22
#define USB_DT_CS_RADIO_CONTROL		0x23
/* From the T10 UAS specification */
#define USB_DT_PIPE_USAGE		0x24
/* From the USB 3.0 spec */
#define	USB_DT_SS_ENDPOINT_COMP		0x30

/* Conventional codes for class-specific descriptors.  The convention is
 * defined in the USB "Common Class" Spec (3.11).  Individual class specs
 * are authoritative for their usage, not the "common class" writeup.
 */
#define USB_DT_CS_DEVICE		(USB_TYPE_CLASS | USB_DT_DEVICE)
#define USB_DT_CS_CONFIG		(USB_TYPE_CLASS | USB_DT_CONFIG)
#define USB_DT_CS_STRING		(USB_TYPE_CLASS | USB_DT_STRING)
#define USB_DT_CS_INTERFACE		(USB_TYPE_CLASS | USB_DT_INTERFACE)
#define USB_DT_CS_ENDPOINT		(USB_TYPE_CLASS | USB_DT_ENDPOINT)

/* All standard descriptors have these 2 fields at the beginning */
#pragma pack(push,1)
struct usb_descriptor_header {
	u8  bLength;
	u8  bDescriptorType;
};
#pragma pack(pop)

/*-------------------------------------------------------------------------*/

/* USB_DT_DEVICE: Device descriptor */
#pragma pack(push,1)
struct usb_device_descriptor {
	u8  bLength;
	u8  bDescriptorType;

	le16 bcdUSB;
	u8  bDeviceClass;
	u8  bDeviceSubClass;
	u8  bDeviceProtocol;
	u8  bMaxPacketSize0;
	le16 idVendor;
	le16 idProduct;
	le16 bcdDevice;
	u8  iManufacturer;
	u8  iProduct;
	u8  iSerialNumber;
	u8  bNumConfigurations;
};
#pragma pack(pop)

#define USB_DT_DEVICE_SIZE		18


/*
 * Device and/or Interface Class codes
 * as found in bDeviceClass or bInterfaceClass
 * and defined by www.usb.org documents
 */
#define USB_CLASS_PER_INTERFACE		0	/* for DeviceClass */
#define USB_CLASS_AUDIO			1
#define USB_CLASS_COMM			2
#define USB_CLASS_HID			3
#define USB_CLASS_PHYSICAL		5
#define USB_CLASS_STILL_IMAGE		6
#define USB_CLASS_PRINTER		7
#define USB_CLASS_MASS_STORAGE		8
#define USB_CLASS_HUB			9
#define USB_CLASS_CDC_DATA		0x0a
#define USB_CLASS_CSCID			0x0b	/* chip+ smart card */
#define USB_CLASS_CONTENT_SEC		0x0d	/* content security */
#define USB_CLASS_VIDEO			0x0e
#define USB_CLASS_WIRELESS_CONTROLLER	0xe0
#define USB_CLASS_MISC			0xef
#define USB_CLASS_APP_SPEC		0xfe
#define USB_CLASS_VENDOR_SPEC		0xff

#define USB_SUBCLASS_VENDOR_SPEC	0xff

/*-------------------------------------------------------------------------*/

/* USB_DT_CONFIG: Configuration descriptor information.
 *
 * USB_DT_OTHER_SPEED_CONFIG is the same descriptor, except that the
 * descriptor type is different.  Highspeed-capable devices can look
 * different depending on what speed they're currently running.  Only
 * devices with a USB_DT_DEVICE_QUALIFIER have any OTHER_SPEED_CONFIG
 * descriptors.
 */
#pragma pack(push,1)
struct usb_config_descriptor {
	u8  bLength;
	u8  bDescriptorType;

	le16 wTotalLength;
	u8  bNumInterfaces;
	u8  bConfigurationValue;
	u8  iConfiguration;
	u8  bmAttributes;
	u8  bMaxPower;
};
#pragma pack(pop)

#define USB_DT_CONFIG_SIZE		9

/* from config descriptor bmAttributes */
#define USB_CONFIG_ATT_ONE		(1 << 7)	/* must be set */
#define USB_CONFIG_ATT_SELFPOWER	(1 << 6)	/* self powered */
#define USB_CONFIG_ATT_WAKEUP		(1 << 5)	/* can wakeup */
#define USB_CONFIG_ATT_BATTERY		(1 << 4)	/* battery powered */

/*-------------------------------------------------------------------------*/

/* USB_DT_STRING: String descriptor */
#pragma pack(push,1)
struct usb_string_descriptor {
	u8  bLength;
	u8  bDescriptorType;

	le16 wData[1];		/* UTF-16LE encoded */
};
#pragma pack(pop)

/* note that "string" zero is special, it holds language codes that
 * the device supports, not Unicode characters.
 */

/*-------------------------------------------------------------------------*/

/* USB_DT_INTERFACE: Interface descriptor */
#pragma pack(push,1)
struct usb_interface_descriptor {
	u8  bLength;
	u8  bDescriptorType;

	u8  bInterfaceNumber;
	u8  bAlternateSetting;
	u8  bNumEndpoints;
	u8  bInterfaceClass;
	u8  bInterfaceSubClass;
	u8  bInterfaceProtocol;
	u8  iInterface;
};
#pragma pack(pop)

#define USB_DT_INTERFACE_SIZE		9

/*-------------------------------------------------------------------------*/

/* USB_DT_ENDPOINT: Endpoint descriptor */
#pragma pack(push,1)
struct usb_endpoint_descriptor {
	u8  bLength;
	u8  bDescriptorType;

	u8  bEndpointAddress;
	u8  bmAttributes;
	le16 wMaxPacketSize;
	u8  bInterval;

	/* NOTE:  these two are _only_ in audio endpoints. */
	/* use USB_DT_ENDPOINT*_SIZE in bLength, not sizeof. */
	u8  bRefresh;
	u8  bSynchAddress;
};
#pragma pack(pop)

#define USB_DT_ENDPOINT_SIZE		7
#define USB_DT_ENDPOINT_AUDIO_SIZE	9	/* Audio extension */


/*
 * Endpoints
 */
#define USB_ENDPOINT_NUMBER_MASK	0x0f	/* in bEndpointAddress */
#define USB_ENDPOINT_DIR_MASK		0x80

#define USB_ENDPOINT_XFERTYPE_MASK	0x03	/* in bmAttributes */
#define USB_ENDPOINT_XFER_CONTROL	0
#define USB_ENDPOINT_XFER_ISOC		1
#define USB_ENDPOINT_XFER_BULK		2
#define USB_ENDPOINT_XFER_INT		3
#define USB_ENDPOINT_MAX_ADJUSTABLE	0x80

/* The USB 3.0 spec redefines bits 5:4 of bmAttributes as interrupt ep type. */
#define USB_ENDPOINT_INTRTYPE		0x30
#define USB_ENDPOINT_INTR_PERIODIC	(0 << 4)
#define USB_ENDPOINT_INTR_NOTIFICATION	(1 << 4)

#define USB_ENDPOINT_SYNCTYPE		0x0c
#define USB_ENDPOINT_SYNC_NONE		(0 << 2)
#define USB_ENDPOINT_SYNC_ASYNC		(1 << 2)
#define USB_ENDPOINT_SYNC_ADAPTIVE	(2 << 2)
#define USB_ENDPOINT_SYNC_SYNC		(3 << 2)

#define USB_ENDPOINT_USAGE_MASK		0x30
#define USB_ENDPOINT_USAGE_DATA		0x00
#define USB_ENDPOINT_USAGE_FEEDBACK	0x10
#define USB_ENDPOINT_USAGE_IMPLICIT_FB	0x20	/* Implicit feedback Data endpoint */


/* USB_DT_SS_ENDPOINT_COMP: SuperSpeed Endpoint Companion descriptor */
#pragma pack(push,1)
struct usb_ss_ep_comp_descriptor {
	u8  bLength;
	u8  bDescriptorType;

	u8  bMaxBurst;
	u8  bmAttributes;
	le16 wBytesPerInterval;
};
#pragma pack(pop)

#define USB_DT_SS_EP_COMP_SIZE		6


/* Bits 1:0 of bmAttributes if this is an isoc endpoint */
#define USB_SS_MULT(p)			(1 + ((p) & 0x3))

/*-------------------------------------------------------------------------*/

/* USB_DT_DEVICE_QUALIFIER: Device Qualifier descriptor */
#pragma pack(push,1)
struct usb_qualifier_descriptor {
	u8  bLength;
	u8  bDescriptorType;

	le16 bcdUSB;
	u8  bDeviceClass;
	u8  bDeviceSubClass;
	u8  bDeviceProtocol;
	u8  bMaxPacketSize0;
	u8  bNumConfigurations;
	u8  bRESERVED;
};
#pragma pack(pop)

/*-------------------------------------------------------------------------*/

/* USB_DT_OTG (from OTG 1.0a supplement) */
#pragma pack(push,1)
struct usb_otg_descriptor {
	u8  bLength;
	u8  bDescriptorType;

	u8  bmAttributes;	/* support for HNP, SRP, etc */
};
#pragma pack(pop)

/* USB_DT_OTG (from OTG 2.0 supplement) */
#pragma pack(push,1)
struct usb_otg20_descriptor {
	u8  bLength;
	u8  bDescriptorType;

	u8  bmAttributes;	/* support for HNP, SRP and ADP, etc */
	le16 bcdOTG;		/* OTG and EH supplement release number
				 * in binary-coded decimal(i.e. 2.0 is 0200H)
				 */
};
#pragma pack(pop)

/* from usb_otg_descriptor.bmAttributes */
#define USB_OTG_SRP		(1 << 0)
#define USB_OTG_HNP		(1 << 1)	/* swap host/device roles */
#define USB_OTG_ADP		(1 << 2)	/* support ADP */

/*-------------------------------------------------------------------------*/

/* USB_DT_DEBUG:  for special highspeed devices, replacing serial console */
#pragma pack(push,1)
struct usb_debug_descriptor {
	u8  bLength;
	u8  bDescriptorType;

	/* bulk endpoints with 8 byte maxpacket */
	u8  bDebugInEndpoint;
	u8  bDebugOutEndpoint;
};
#pragma pack(pop)
/*-------------------------------------------------------------------------*/

/* USB_DT_INTERFACE_ASSOCIATION: groups interfaces */
#pragma pack(push,1)
struct usb_interface_assoc_descriptor {
	u8  bLength;
	u8  bDescriptorType;

	u8  bFirstInterface;
	u8  bInterfaceCount;
	u8  bFunctionClass;
	u8  bFunctionSubClass;
	u8  bFunctionProtocol;
	u8  iFunction;
};
#pragma pack(pop)


/*-------------------------------------------------------------------------*/

/* USB_DT_SECURITY:  group of wireless security descriptors, including
 * encryption types available for setting up a CC/association.
 */
#pragma pack(push,1)
struct usb_security_descriptor {
	u8  bLength;
	u8  bDescriptorType;

	le16 wTotalLength;
	u8  bNumEncryptionTypes;
};
#pragma pack(pop)

/*-------------------------------------------------------------------------*/

/* USB_DT_KEY:  used with {GET,SET}_SECURITY_DATA; only public keys
 * may be retrieved.
 */
#pragma pack(push,1)
struct usb_key_descriptor {
	u8  bLength;
	u8  bDescriptorType;

	u8  tTKID[3];
	u8  bReserved;
	u8  bKeyData[0];
};
#pragma pack(pop)

/*-------------------------------------------------------------------------*/

/* USB_DT_ENCRYPTION_TYPE:  bundled in DT_SECURITY groups */
#pragma pack(push,1)
struct usb_encryption_descriptor {
	u8  bLength;
	u8  bDescriptorType;

	u8  bEncryptionType;
#define	USB_ENC_TYPE_UNSECURE		0
#define	USB_ENC_TYPE_WIRED		1	/* non-wireless mode */
#define	USB_ENC_TYPE_CCM_1		2	/* aes128/cbc session */
#define	USB_ENC_TYPE_RSA_1		3	/* rsa3072/sha1 auth */
	u8  bEncryptionValue;		/* use in SET_ENCRYPTION */
	u8  bAuthKeyIndex;
};
#pragma pack(pop)


/*-------------------------------------------------------------------------*/

/* USB_DT_BOS:  group of device-level capabilities */
#pragma pack(push,1)
struct usb_bos_descriptor {
	u8  bLength;
	u8  bDescriptorType;

	le16 wTotalLength;
	u8  bNumDeviceCaps;
};
#pragma pack(pop)

#define USB_DT_BOS_SIZE		5
/*-------------------------------------------------------------------------*/

/* USB_DT_DEVICE_CAPABILITY:  grouped with BOS */
#pragma pack(push,1)
struct usb_dev_cap_header {
	u8  bLength;
	u8  bDescriptorType;
	u8  bDevCapabilityType;
};
#pragma pack(pop)

#define	USB_CAP_TYPE_WIRELESS_USB	1

#pragma pack(push,1)
struct usb_wireless_cap_descriptor {	/* Ultra Wide Band */
	u8  bLength;
	u8  bDescriptorType;
	u8  bDevCapabilityType;

	u8  bmAttributes;
#define	USB_WIRELESS_P2P_DRD		(1 << 1)
#define	USB_WIRELESS_BEACON_MASK	(3 << 2)
#define	USB_WIRELESS_BEACON_SELF	(1 << 2)
#define	USB_WIRELESS_BEACON_DIRECTED	(2 << 2)
#define	USB_WIRELESS_BEACON_NONE	(3 << 2)
	le16 wPHYRates;	/* bit rates, Mbps */
#define	USB_WIRELESS_PHY_53		(1 << 0)	/* always set */
#define	USB_WIRELESS_PHY_80		(1 << 1)
#define	USB_WIRELESS_PHY_107		(1 << 2)	/* always set */
#define	USB_WIRELESS_PHY_160		(1 << 3)
#define	USB_WIRELESS_PHY_200		(1 << 4)	/* always set */
#define	USB_WIRELESS_PHY_320		(1 << 5)
#define	USB_WIRELESS_PHY_400		(1 << 6)
#define	USB_WIRELESS_PHY_480		(1 << 7)
	u8  bmTFITXPowerInfo;	/* TFI power levels */
	u8  bmFFITXPowerInfo;	/* FFI power levels */
	le16 bmBandGroup;
	u8  bReserved;
};
#pragma pack(pop)

/* USB 2.0 Extension descriptor */
#define	USB_CAP_TYPE_EXT		2

#pragma pack(push,1)
struct usb_ext_cap_descriptor {		/* Link Power Management */
	u8  bLength;
	u8  bDescriptorType;
	u8  bDevCapabilityType;
	le32 bmAttributes;
#define USB_LPM_SUPPORT			(1 << 1)	/* supports LPM */
#define USB_BESL_SUPPORT		(1 << 2)	/* supports BESL */
#define USB_BESL_BASELINE_VALID		(1 << 3)	/* Baseline BESL valid*/
#define USB_BESL_DEEP_VALID		(1 << 4)	/* Deep BESL valid */
#define USB_GET_BESL_BASELINE(p)	(((p) & (0xf << 8)) >> 8)
#define USB_GET_BESL_DEEP(p)		(((p) & (0xf << 12)) >> 12)
};
#pragma pack(pop)

#define USB_DT_USB_EXT_CAP_SIZE	7

/*
 * SuperSpeed USB Capability descriptor: Defines the set of SuperSpeed USB
 * specific device level capabilities
 */
#define		USB_SS_CAP_TYPE		3
#pragma pack(push,1)
struct usb_ss_cap_descriptor {		/* Link Power Management */
	u8  bLength;
	u8  bDescriptorType;
	u8  bDevCapabilityType;
	u8  bmAttributes;
#define USB_LTM_SUPPORT			(1 << 1) /* supports LTM */
	le16 wSpeedSupported;
#define USB_LOW_SPEED_OPERATION		(1)	 /* Low speed operation */
#define USB_FULL_SPEED_OPERATION	(1 << 1) /* Full speed operation */
#define USB_HIGH_SPEED_OPERATION	(1 << 2) /* High speed operation */
#define USB_5GBPS_OPERATION		(1 << 3) /* Operation at 5Gbps */
	u8  bFunctionalitySupport;
	u8  bU1devExitLat;
	le16 bU2DevExitLat;
};
#pragma pack(pop)

#define USB_DT_USB_SS_CAP_SIZE	10

/*
 * Container ID Capability descriptor: Defines the instance unique ID used to
 * identify the instance across all operating modes
 */
#define	CONTAINER_ID_TYPE	4
#pragma pack(push,1)
struct usb_ss_container_id_descriptor {
	u8  bLength;
	u8  bDescriptorType;
	u8  bDevCapabilityType;
	u8  bReserved;
	u8  ContainerID[16]; /* 128-bit number */
};
#pragma pack(pop)

#define USB_DT_USB_SS_CONTN_ID_SIZE	20

/*
 * SuperSpeed Plus USB Capability descriptor: Defines the set of
 * SuperSpeed Plus USB specific device level capabilities
 */
#define	USB_SSP_CAP_TYPE	0xa
#pragma pack(push,1)
struct usb_ssp_cap_descriptor {
	u8  bLength;
	u8  bDescriptorType;
	u8  bDevCapabilityType;
	u8  bReserved;
	le32 bmAttributes;
#define USB_SSP_SUBLINK_SPEED_ATTRIBS	(0x1f << 0) /* sublink speed entries */
#define USB_SSP_SUBLINK_SPEED_IDS	(0xf << 5)  /* speed ID entries */
	u16  wFunctionalitySupport;
#define USB_SSP_MIN_SUBLINK_SPEED_ATTRIBUTE_ID	(0xf)
#define USB_SSP_MIN_RX_LANE_COUNT		(0xf << 8)
#define USB_SSP_MIN_TX_LANE_COUNT		(0xf << 12)
	le16 wReserved;
	le32 bmSublinkSpeedAttr[1]; /* list of sublink speed attrib entries */
#define USB_SSP_SUBLINK_SPEED_SSID	(0xf)		/* sublink speed ID */
#define USB_SSP_SUBLINK_SPEED_LSE	(0x3 << 4)	/* Lanespeed exponent */
#define USB_SSP_SUBLINK_SPEED_ST	(0x3 << 6)	/* Sublink type */
#define USB_SSP_SUBLINK_SPEED_RSVD	(0x3f << 8)	/* Reserved */
#define USB_SSP_SUBLINK_SPEED_LP	(0x3 << 14)	/* Link protocol */
#define USB_SSP_SUBLINK_SPEED_LSM	(0xff << 16)	/* Lanespeed mantissa */
};
#pragma pack(pop)


/*-------------------------------------------------------------------------*/

/* USB_DT_WIRELESS_ENDPOINT_COMP:  companion descriptor associated with
 * each endpoint descriptor for a wireless device
 */
#pragma pack(push,1)
struct usb_wireless_ep_comp_descriptor {
	u8  bLength;
	u8  bDescriptorType;

	u8  bMaxBurst;
	u8  bMaxSequence;
	le16 wMaxStreamDelay;
	le16 wOverTheAirPacketSize;
	u8  bOverTheAirInterval;
	u8  bmCompAttributes;
#define USB_ENDPOINT_SWITCH_MASK	0x03	/* in bmCompAttributes */
#define USB_ENDPOINT_SWITCH_NO		0
#define USB_ENDPOINT_SWITCH_SWITCH	1
#define USB_ENDPOINT_SWITCH_SCALE	2
};
#pragma pack(pop)

/*-------------------------------------------------------------------------*/

/* USB_REQ_SET_HANDSHAKE is a four-way handshake used between a wireless
 * host and a device for connection set up, mutual authentication, and
 * exchanging short lived session keys.  The handshake depends on a CC.
 */
#pragma pack(push,1)
struct usb_handshake {
	u8 bMessageNumber;
	u8 bStatus;
	u8 tTKID[3];
	u8 bReserved;
	u8 CDID[16];
	u8 nonce[16];
	u8 MIC[8];
};
#pragma pack(pop)

/*-------------------------------------------------------------------------*/

/* USB_REQ_SET_CONNECTION modifies or revokes a connection context (CC).
 * A CC may also be set up using non-wireless secure channels (including
 * wired USB!), and some devices may support CCs with multiple hosts.
 */
#pragma pack(push,1)
struct usb_connection_context {
	u8 CHID[16];		/* persistent host id */
	u8 CDID[16];		/* device id (unique w/in host context) */
	u8 CK[16];		/* connection key */
};
#pragma pack(pop)

/*-------------------------------------------------------------------------*/

/* USB 2.0 defines three speeds, here's how Linux identifies them */

enum usb_device_speed {
	USB_SPEED_UNKNOWN = 0,			/* enumerating */
	USB_SPEED_LOW, USB_SPEED_FULL,		/* usb 1.1 */
	USB_SPEED_HIGH,				/* usb 2.0 */
	USB_SPEED_WIRELESS,			/* wireless (usb 2.5) */
	USB_SPEED_SUPER,			/* usb 3.0 */
	USB_SPEED_SUPER_PLUS,			/* usb 3.1 */
};


enum usb_device_state {
	/* NOTATTACHED isn't in the USB spec, and this state acts
	 * the same as ATTACHED ... but it's clearer this way.
	 */
	USB_STATE_NOTATTACHED = 0,

	/* chapter 9 and authentication (wireless) device states */
	USB_STATE_ATTACHED,
	USB_STATE_POWERED,			/* wired */
	USB_STATE_RECONNECTING,			/* auth */
	USB_STATE_UNAUTHENTICATED,		/* auth */
	USB_STATE_DEFAULT,			/* limited function */
	USB_STATE_ADDRESS,
	USB_STATE_CONFIGURED,			/* most functions */

	USB_STATE_SUSPENDED

	/* NOTE:  there are actually four different SUSPENDED
	 * states, returning to POWERED, DEFAULT, ADDRESS, or
	 * CONFIGURED respectively when SOF tokens flow again.
	 * At this level there's no difference between L1 and L2
	 * suspend states.  (L2 being original USB 1.1 suspend.)
	 */
};

enum usb3_link_state {
	USB3_LPM_U0 = 0,
	USB3_LPM_U1,
	USB3_LPM_U2,
	USB3_LPM_U3
};

/*
 * A U1 timeout of 0x0 means the parent hub will reject any transitions to U1.
 * 0xff means the parent hub will accept transitions to U1, but will not
 * initiate a transition.
 *
 * A U1 timeout of 0x1 to 0x7F also causes the hub to initiate a transition to
 * U1 after that many microseconds.  Timeouts of 0x80 to 0xFE are reserved
 * values.
 *
 * A U2 timeout of 0x0 means the parent hub will reject any transitions to U2.
 * 0xff means the parent hub will accept transitions to U2, but will not
 * initiate a transition.
 *
 * A U2 timeout of 0x1 to 0xFE also causes the hub to initiate a transition to
 * U2 after N*256 microseconds.  Therefore a U2 timeout value of 0x1 means a U2
 * idle timer of 256 microseconds, 0x2 means 512 microseconds, 0xFE means
 * 65.024ms.
 */
#define USB3_LPM_DISABLED		0x0
#define USB3_LPM_U1_MAX_TIMEOUT		0x7F
#define USB3_LPM_U2_MAX_TIMEOUT		0xFE
#define USB3_LPM_DEVICE_INITIATED	0xFF

#pragma pack(push,1)
struct usb_set_sel_req {
	u8	u1_sel;
	u8	u1_pel;
	le16	u2_sel;
	le16	u2_pel;
};
#pragma pack(pop)

/*
 * The Set System Exit Latency control transfer provides one byte each for
 * U1 SEL and U1 PEL, so the max exit latency is 0xFF.  U2 SEL and U2 PEL each
 * are two bytes long.
 */
#define USB3_LPM_MAX_U1_SEL_PEL		0xFF
#define USB3_LPM_MAX_U2_SEL_PEL		0xFFFF

/*-------------------------------------------------------------------------*/

/*
 * As per USB compliance update, a device that is actively drawing
 * more than 100mA from USB must report itself as bus-powered in
 * the GetStatus(DEVICE) call.
 * http://compliance.usb.org/index.asp?UpdateFile=Electrical&Format=Standard#34
 */
#define USB_SELF_POWER_VBUS_MAX_DRAW		100

















#define USB_CDC_SUBCLASS_ACM			0x02
#define USB_CDC_SUBCLASS_ETHERNET		0x06
#define USB_CDC_SUBCLASS_WHCM			0x08
#define USB_CDC_SUBCLASS_DMM			0x09
#define USB_CDC_SUBCLASS_MDLM			0x0a
#define USB_CDC_SUBCLASS_OBEX			0x0b
#define USB_CDC_SUBCLASS_EEM			0x0c
#define USB_CDC_SUBCLASS_NCM			0x0d
#define USB_CDC_SUBCLASS_MBIM			0x0e

#define USB_CDC_PROTO_NONE			0

#define USB_CDC_ACM_PROTO_AT_V25TER		1
#define USB_CDC_ACM_PROTO_AT_PCCA101		2
#define USB_CDC_ACM_PROTO_AT_PCCA101_WAKE	3
#define USB_CDC_ACM_PROTO_AT_GSM		4
#define USB_CDC_ACM_PROTO_AT_3G			5
#define USB_CDC_ACM_PROTO_AT_CDMA		6
#define USB_CDC_ACM_PROTO_VENDOR		0xff

#define USB_CDC_PROTO_EEM			7

#define USB_CDC_NCM_PROTO_NTB			1
#define USB_CDC_MBIM_PROTO_NTB			2

/*-------------------------------------------------------------------------*/

/*
 * Class-Specific descriptors ... there are a couple dozen of them
 */

#define USB_CDC_HEADER_TYPE		0x00	/* header_desc */
#define USB_CDC_CALL_MANAGEMENT_TYPE	0x01	/* call_mgmt_descriptor */
#define USB_CDC_ACM_TYPE		0x02	/* acm_descriptor */
#define USB_CDC_UNION_TYPE		0x06	/* union_desc */
#define USB_CDC_COUNTRY_TYPE		0x07
#define USB_CDC_NETWORK_TERMINAL_TYPE	0x0a	/* network_terminal_desc */
#define USB_CDC_ETHERNET_TYPE		0x0f	/* ether_desc */
#define USB_CDC_WHCM_TYPE		0x11
#define USB_CDC_MDLM_TYPE		0x12	/* mdlm_desc */
#define USB_CDC_MDLM_DETAIL_TYPE	0x13	/* mdlm_detail_desc */
#define USB_CDC_DMM_TYPE		0x14
#define USB_CDC_OBEX_TYPE		0x15
#define USB_CDC_NCM_TYPE		0x1a
#define USB_CDC_MBIM_TYPE		0x1b
#define USB_CDC_MBIM_EXTENDED_TYPE	0x1c

/* "Header Functional Descriptor" from CDC spec  5.2.3.1 */
#pragma pack(push,1)
struct usb_cdc_header_desc {
	u8	bLength;
	u8	bDescriptorType;
	u8	bDescriptorSubType;

	le16	bcdCDC;
};
#pragma pack(pop)

/* "Call Management Descriptor" from CDC spec  5.2.3.2 */
#pragma pack(push,1)
struct usb_cdc_call_mgmt_descriptor {
	u8	bLength;
	u8	bDescriptorType;
	u8	bDescriptorSubType;

	u8	bmCapabilities;
#define USB_CDC_CALL_MGMT_CAP_CALL_MGMT		0x01
#define USB_CDC_CALL_MGMT_CAP_DATA_INTF		0x02

	u8	bDataInterface;
};
#pragma pack(pop)

/* "Abstract Control Management Descriptor" from CDC spec  5.2.3.3 */
#pragma pack(push,1)
struct usb_cdc_acm_descriptor {
	u8	bLength;
	u8	bDescriptorType;
	u8	bDescriptorSubType;

	u8	bmCapabilities;
};
#pragma pack(pop)

/* capabilities from 5.2.3.3 */

#define USB_CDC_COMM_FEATURE	0x01
#define USB_CDC_CAP_LINE	0x02
#define USB_CDC_CAP_BRK		0x04
#define USB_CDC_CAP_NOTIFY	0x08

/* "Union Functional Descriptor" from CDC spec 5.2.3.8 */
#pragma pack(push,1)
struct usb_cdc_union_desc {
	u8	bLength;
	u8	bDescriptorType;
	u8	bDescriptorSubType;

	u8	bMasterInterface0;
	u8	bSlaveInterface0;
	/* ... and there could be other slave interfaces */
};
#pragma pack(pop)

/* "Country Selection Functional Descriptor" from CDC spec 5.2.3.9 */
#pragma pack(push,1)
struct usb_cdc_country_functional_desc {
	u8	bLength;
	u8	bDescriptorType;
	u8	bDescriptorSubType;

	u8	iCountryCodeRelDate;
	le16	wCountyCode0;
	/* ... and there can be a lot of country codes */
};
#pragma pack(pop)

/* "Network Channel Terminal Functional Descriptor" from CDC spec 5.2.3.11 */
#pragma pack(push,1)
struct usb_cdc_network_terminal_desc {
	u8	bLength;
	u8	bDescriptorType;
	u8	bDescriptorSubType;

	u8	bEntityId;
	u8	iName;
	u8	bChannelIndex;
	u8	bPhysicalInterface;
};
#pragma pack(pop)

/* "Ethernet Networking Functional Descriptor" from CDC spec 5.2.3.16 */
#pragma pack(push,1)
struct usb_cdc_ether_desc {
	u8	bLength;
	u8	bDescriptorType;
	u8	bDescriptorSubType;

	u8	iMACAddress;
	le32	bmEthernetStatistics;
	le16	wMaxSegmentSize;
	le16	wNumberMCFilters;
	u8	bNumberPowerFilters;
};
#pragma pack(pop)

/* "Telephone Control Model Functional Descriptor" from CDC WMC spec 6.3..3 */
#pragma pack(push,1)
struct usb_cdc_dmm_desc {
	u8	bFunctionLength;
	u8	bDescriptorType;
	u8	bDescriptorSubtype;
	u16	bcdVersion;
	le16	wMaxCommand;
};
#pragma pack(pop)

/* "MDLM Functional Descriptor" from CDC WMC spec 6.7.2.3 */
#pragma pack(push,1)
struct usb_cdc_mdlm_desc {
	u8	bLength;
	u8	bDescriptorType;
	u8	bDescriptorSubType;

	le16	bcdVersion;
	u8	bGUID[16];
};
#pragma pack(pop)

/* "MDLM Detail Functional Descriptor" from CDC WMC spec 6.7.2.4 */
#pragma pack(push,1)
struct usb_cdc_mdlm_detail_desc {
	u8	bLength;
	u8	bDescriptorType;
	u8	bDescriptorSubType;

	/* type is associated with mdlm_desc.bGUID */
	u8	bGuidDescriptorType;
	u8	bDetailData[0];
};
#pragma pack(pop)

/* "OBEX Control Model Functional Descriptor" */
#pragma pack(push,1)
struct usb_cdc_obex_desc {
	u8	bLength;
	u8	bDescriptorType;
	u8	bDescriptorSubType;

	le16	bcdVersion;
};
#pragma pack(pop)

/* "NCM Control Model Functional Descriptor" */
#pragma pack(push,1)
struct usb_cdc_ncm_desc {
	u8	bLength;
	u8	bDescriptorType;
	u8	bDescriptorSubType;

	le16	bcdNcmVersion;
	u8	bmNetworkCapabilities;
};
#pragma pack(pop)

/* "MBIM Control Model Functional Descriptor" */
#pragma pack(push,1)
struct usb_cdc_mbim_desc {
	u8	bLength;
	u8	bDescriptorType;
	u8	bDescriptorSubType;

	le16	bcdMBIMVersion;
	le16  wMaxControlMessage;
	u8    bNumberFilters;
	u8    bMaxFilterSize;
	le16  wMaxSegmentSize;
	u8    bmNetworkCapabilities;
};
#pragma pack(pop)

/* "MBIM Extended Functional Descriptor" from CDC MBIM spec 1.0 errata-1 */
#pragma pack(push,1)
struct usb_cdc_mbim_extended_desc {
	u8	bLength;
	u8	bDescriptorType;
	u8	bDescriptorSubType;

	le16	bcdMBIMExtendedVersion;
	u8	bMaxOutstandingCommandMessages;
	le16	wMTU;
};
#pragma pack(pop)

/*-------------------------------------------------------------------------*/

/*
 * Class-Specific Control Requests (6.2)
 *
 * section 3.6.2.1 table 4 has the ACM profile, for modems.
 * section 3.8.2 table 10 has the ethernet profile.
 *
 * Microsoft's RNDIS stack for Ethernet is a vendor-specific CDC ACM variant,
 * heavily dependent on the encapsulated (proprietary) command mechanism.
 */

#define USB_CDC_SEND_ENCAPSULATED_COMMAND	0x00
#define USB_CDC_GET_ENCAPSULATED_RESPONSE	0x01
#define USB_CDC_REQ_SET_LINE_CODING		0x20
#define USB_CDC_REQ_GET_LINE_CODING		0x21
#define USB_CDC_REQ_SET_CONTROL_LINE_STATE	0x22
#define USB_CDC_REQ_SEND_BREAK			0x23
#define USB_CDC_SET_ETHERNET_MULTICAST_FILTERS	0x40
#define USB_CDC_SET_ETHERNET_PM_PATTERN_FILTER	0x41
#define USB_CDC_GET_ETHERNET_PM_PATTERN_FILTER	0x42
#define USB_CDC_SET_ETHERNET_PACKET_FILTER	0x43
#define USB_CDC_GET_ETHERNET_STATISTIC		0x44
#define USB_CDC_GET_NTB_PARAMETERS		0x80
#define USB_CDC_GET_NET_ADDRESS			0x81
#define USB_CDC_SET_NET_ADDRESS			0x82
#define USB_CDC_GET_NTB_FORMAT			0x83
#define USB_CDC_SET_NTB_FORMAT			0x84
#define USB_CDC_GET_NTB_INPUT_SIZE		0x85
#define USB_CDC_SET_NTB_INPUT_SIZE		0x86
#define USB_CDC_GET_MAX_DATAGRAM_SIZE		0x87
#define USB_CDC_SET_MAX_DATAGRAM_SIZE		0x88
#define USB_CDC_GET_CRC_MODE			0x89
#define USB_CDC_SET_CRC_MODE			0x8a

/* Line Coding Structure from CDC spec 6.2.13 */
#pragma pack(push,1)
struct usb_cdc_line_coding {
	le32	dwDTERate;
	u8	bCharFormat;
#define USB_CDC_1_STOP_BITS			0
#define USB_CDC_1_5_STOP_BITS			1
#define USB_CDC_2_STOP_BITS			2

	u8	bParityType;
#define USB_CDC_NO_PARITY			0
#define USB_CDC_ODD_PARITY			1
#define USB_CDC_EVEN_PARITY			2
#define USB_CDC_MARK_PARITY			3
#define USB_CDC_SPACE_PARITY			4

	u8	bDataBits;
};
#pragma pack(pop)

/* table 62; bits in multicast filter */
#define	USB_CDC_PACKET_TYPE_PROMISCUOUS		(1 << 0)
#define	USB_CDC_PACKET_TYPE_ALL_MULTICAST	(1 << 1) /* no filter */
#define	USB_CDC_PACKET_TYPE_DIRECTED		(1 << 2)
#define	USB_CDC_PACKET_TYPE_BROADCAST		(1 << 3)
#define	USB_CDC_PACKET_TYPE_MULTICAST		(1 << 4) /* filtered */


/*-------------------------------------------------------------------------*/

/*
 * Class-Specific Notifications (6.3) sent by interrupt transfers
 *
 * section 3.8.2 table 11 of the CDC spec lists Ethernet notifications
 * section 3.6.2.1 table 5 specifies ACM notifications, accepted by RNDIS
 * RNDIS also defines its own bit-incompatible notifications
 */

#define USB_CDC_NOTIFY_NETWORK_CONNECTION	0x00
#define USB_CDC_NOTIFY_RESPONSE_AVAILABLE	0x01
#define USB_CDC_NOTIFY_SERIAL_STATE		0x20
#define USB_CDC_NOTIFY_SPEED_CHANGE		0x2a

#pragma pack(push,1)
struct usb_cdc_notification {
	u8	bmRequestType;
	u8	bNotificationType;
	le16	wValue;
	le16	wIndex;
	le16	wLength;
};
#pragma pack(pop)

#pragma pack(push,1)
struct usb_cdc_speed_change {
	le32	DLBitRRate;	/* contains the downlink bit rate (IN pipe) */
	le32	ULBitRate;	/* contains the uplink bit rate (OUT pipe) */
};
#pragma pack(pop)

/*-------------------------------------------------------------------------*/

/*
 * Class Specific structures and constants
 *
 * CDC NCM NTB parameters structure, CDC NCM subclass 6.2.1
 *
 */
#pragma pack(push,1)
struct usb_cdc_ncm_ntb_parameters {
	le16	wLength;
	le16	bmNtbFormatsSupported;
	le32	dwNtbInMaxSize;
	le16	wNdpInDivisor;
	le16	wNdpInPayloadRemainder;
	le16	wNdpInAlignment;
	le16	wPadding1;
	le32	dwNtbOutMaxSize;
	le16	wNdpOutDivisor;
	le16	wNdpOutPayloadRemainder;
	le16	wNdpOutAlignment;
	le16	wNtbOutMaxDatagrams;
};
#pragma pack(pop)

/*
 * CDC NCM transfer headers, CDC NCM subclass 3.2
 */

#define USB_CDC_NCM_NTH16_SIGN		0x484D434E /* NCMH */
#define USB_CDC_NCM_NTH32_SIGN		0x686D636E /* ncmh */

#pragma pack(push,1)
struct usb_cdc_ncm_nth16 {
	le32	dwSignature;
	le16	wHeaderLength;
	le16	wSequence;
	le16	wBlockLength;
	le16	wNdpIndex;
};
#pragma pack(pop)

#pragma pack(push,1)
struct usb_cdc_ncm_nth32 {
	le32	dwSignature;
	le16	wHeaderLength;
	le16	wSequence;
	le32	dwBlockLength;
	le32	dwNdpIndex;
};
#pragma pack(pop)

/*
 * CDC NCM datagram pointers, CDC NCM subclass 3.3
 */

#define USB_CDC_NCM_NDP16_CRC_SIGN	0x314D434E /* NCM1 */
#define USB_CDC_NCM_NDP16_NOCRC_SIGN	0x304D434E /* NCM0 */
#define USB_CDC_NCM_NDP32_CRC_SIGN	0x316D636E /* ncm1 */
#define USB_CDC_NCM_NDP32_NOCRC_SIGN	0x306D636E /* ncm0 */

#define USB_CDC_MBIM_NDP16_IPS_SIGN     0x00535049 /* IPS<sessionID> : IPS0 for now */
#define USB_CDC_MBIM_NDP32_IPS_SIGN     0x00737069 /* ips<sessionID> : ips0 for now */
#define USB_CDC_MBIM_NDP16_DSS_SIGN     0x00535344 /* DSS<sessionID> */
#define USB_CDC_MBIM_NDP32_DSS_SIGN     0x00737364 /* dss<sessionID> */

/* 16-bit NCM Datagram Pointer Entry */
#pragma pack(push,1)
struct usb_cdc_ncm_dpe16 {
	le16	wDatagramIndex;
	le16	wDatagramLength;
};
#pragma pack(pop)

/* 16-bit NCM Datagram Pointer Table */
#pragma pack(push,1)
struct usb_cdc_ncm_ndp16 {
	le32	dwSignature;
	le16	wLength;
	le16	wNextNdpIndex;
	struct	usb_cdc_ncm_dpe16 dpe16[0];
};
#pragma pack(pop)

/* 32-bit NCM Datagram Pointer Entry */
#pragma pack(push,1)
struct usb_cdc_ncm_dpe32 {
	le32	dwDatagramIndex;
	le32	dwDatagramLength;
};
#pragma pack(pop)

/* 32-bit NCM Datagram Pointer Table */
#pragma pack(push,1)
struct usb_cdc_ncm_ndp32 {
	le32	dwSignature;
	le16	wLength;
	le16	wReserved6;
	le32	dwNextNdpIndex;
	le32	dwReserved12;
	struct	usb_cdc_ncm_dpe32 dpe32[0];
};
#pragma pack(pop)

/* CDC NCM subclass 3.2.1 and 3.2.2 */
#define USB_CDC_NCM_NDP16_INDEX_MIN			0x000C
#define USB_CDC_NCM_NDP32_INDEX_MIN			0x0010

/* CDC NCM subclass 3.3.3 Datagram Formatting */
#define USB_CDC_NCM_DATAGRAM_FORMAT_CRC			0x30
#define USB_CDC_NCM_DATAGRAM_FORMAT_NOCRC		0X31

/* CDC NCM subclass 4.2 NCM Communications Interface Protocol Code */
#define USB_CDC_NCM_PROTO_CODE_NO_ENCAP_COMMANDS	0x00
#define USB_CDC_NCM_PROTO_CODE_EXTERN_PROTO		0xFE

/* CDC NCM subclass 5.2.1 NCM Functional Descriptor, bmNetworkCapabilities */
#define USB_CDC_NCM_NCAP_ETH_FILTER			(1 << 0)
#define USB_CDC_NCM_NCAP_NET_ADDRESS			(1 << 1)
#define USB_CDC_NCM_NCAP_ENCAP_COMMAND			(1 << 2)
#define USB_CDC_NCM_NCAP_MAX_DATAGRAM_SIZE		(1 << 3)
#define USB_CDC_NCM_NCAP_CRC_MODE			(1 << 4)
#define	USB_CDC_NCM_NCAP_NTB_INPUT_SIZE			(1 << 5)

/* CDC NCM subclass Table 6-3: NTB Parameter Structure */
#define USB_CDC_NCM_NTB16_SUPPORTED			(1 << 0)
#define USB_CDC_NCM_NTB32_SUPPORTED			(1 << 1)

/* CDC NCM subclass Table 6-3: NTB Parameter Structure */
#define USB_CDC_NCM_NDP_ALIGN_MIN_SIZE			0x04
#define USB_CDC_NCM_NTB_MAX_LENGTH			0x1C

/* CDC NCM subclass 6.2.5 SetNtbFormat */
#define USB_CDC_NCM_NTB16_FORMAT			0x00
#define USB_CDC_NCM_NTB32_FORMAT			0x01

/* CDC NCM subclass 6.2.7 SetNtbInputSize */
#define USB_CDC_NCM_NTB_MIN_IN_SIZE			2048
#define USB_CDC_NCM_NTB_MIN_OUT_SIZE			2048

/* NTB Input Size Structure */
#pragma pack(push,1)
struct usb_cdc_ncm_ndp_input_size {
	le32	dwNtbInMaxSize;
	le16	wNtbInMaxDatagrams;
	le16	wReserved;
};
#pragma pack(pop)

/* CDC NCM subclass 6.2.11 SetCrcMode */
#define USB_CDC_NCM_CRC_NOT_APPENDED			0x00
#define USB_CDC_NCM_CRC_APPENDED			0x01


#endif /* __UAPI_LINUX_USB_CDC_H */
