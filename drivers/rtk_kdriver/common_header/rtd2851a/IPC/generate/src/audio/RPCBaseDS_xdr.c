/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <RPCBaseDS.h>
#include <rpc_common.h>

bool_t
xdr_HRESULT (XDR *xdrs, HRESULT *objp)
{
	 if (!xdr_rpc_s32_t (xdrs, objp))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_RPCRES_LONG (XDR *xdrs, RPCRES_LONG *objp)
{
	 if (!xdr_HRESULT (xdrs, &objp->result))
		 return FALSE;
	 if (!xdr_rpc_s32_t (xdrs, &objp->data))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_USB_DEVICE_CONFIG_INFO (XDR *xdrs, USB_DEVICE_CONFIG_INFO *objp)
{
	 if (!xdr_HRESULT (xdrs, &objp->ret))
		 return FALSE;
	 if (!xdr_rpc_s32_t (xdrs, &objp->usb_ai_samplerate))
		 return FALSE;
	 if (!xdr_rpc_s32_t (xdrs, &objp->usb_ai_format))
		 return FALSE;
	 if (!xdr_rpc_s32_t (xdrs, &objp->usb_ai_chnum))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_RPC_CONNECTION (XDR *xdrs, RPC_CONNECTION *objp)
{
	 if (!xdr_rpc_s32_t (xdrs, &objp->srcInstanceID))
		 return FALSE;
	 if (!xdr_rpc_s32_t (xdrs, &objp->srcPinID))
		 return FALSE;
	 if (!xdr_rpc_s32_t (xdrs, &objp->desInstanceID))
		 return FALSE;
	 if (!xdr_rpc_s32_t (xdrs, &objp->desPinID))
		 return FALSE;
	 if (!xdr_rpc_s32_t (xdrs, &objp->mediaType))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_RPC_SEND_LONG (XDR *xdrs, RPC_SEND_LONG *objp)
{
	 if (!xdr_rpc_s32_t (xdrs, &objp->instanceID))
		 return FALSE;
	 if (!xdr_rpc_s32_t (xdrs, &objp->pinID))
		 return FALSE;
	 if (!xdr_rpc_s32_t (xdrs, &objp->data))
		 return FALSE;
	return TRUE;
}
