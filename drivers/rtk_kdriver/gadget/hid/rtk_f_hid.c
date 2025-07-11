#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hid.h>
#include <linux/idr.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include <linux/poll.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/usb/g_hid.h>
#include <linux/atomic.h>
#include <linux/jiffies.h>
#include <linux/timer.h>
#include <mach/rtk_platform.h>
#include "rtk_hidg_dev_info.h"
#include "u_f.h"
#include "u_hid.h"
#include "rtk_composite_platform.h"

#define HIDG_MINORS	8

static int major, minors;
static struct class *hidg_class;
static DEFINE_MUTEX(hidg_func_inst_cnt_lock); /* protects access to g_alloc_func_inst_cnt */
static int g_alloc_func_inst_cnt = 0;
static int rtk_ghid_setup(struct usb_gadget *g, int count);
static void rtk_ghid_cleanup(void);
static struct f_hidg *g_hidg_array[HIDG_MINORS];
/*-------------------------------------------------------------------------*/
/*                            HID gadget struct                            */

struct f_hidg_req_list {
	struct usb_request	*req;
	unsigned int		pos;
	struct list_head 	list;
};

struct f_hidg {
	/* configuration */
	char nid[MAX_NID_SIZE];
	unsigned char			bInterfaceSubClass;
	unsigned char			bInterfaceProtocol;
	unsigned char			protocol;
	unsigned short			report_desc_length;
	char				*report_desc;
	unsigned short			report_length;

	/* recv report */
	struct list_head		completed_out_req;
	spinlock_t			read_spinlock;
	wait_queue_head_t		read_queue;
	unsigned int			qlen;

	/* send report */
	spinlock_t			write_spinlock;
	bool				write_pending;
	wait_queue_head_t		write_queue;
	struct usb_request		*req;

	int				minor;
	struct cdev			cdev;
	struct usb_function		func;

	struct usb_ep			*in_ep;
};

static inline struct f_hidg *func_to_hidg(struct usb_function *f)
{
	return container_of(f, struct f_hidg, func);
}

/*-------------------------------------------------------------------------*/
/*                           Static descriptors                            */

static struct usb_interface_descriptor hidg_interface_desc = {
	.bLength		= sizeof hidg_interface_desc,
	.bDescriptorType	= USB_DT_INTERFACE,
	/* .bInterfaceNumber	= DYNAMIC */
	.bAlternateSetting	= 0,
	.bNumEndpoints		= 1,
	.bInterfaceClass	= USB_CLASS_HID,
	/* .bInterfaceSubClass	= DYNAMIC */
	/* .bInterfaceProtocol	= DYNAMIC */
	/* .iInterface		= DYNAMIC */
};

static struct hid_descriptor hidg_desc = {
	.bLength			= sizeof hidg_desc,
	.bDescriptorType		= HID_DT_HID,
	.bcdHID				= 0x0101,
	.bCountryCode			= 0x00,
	.bNumDescriptors		= 0x1,
	/*.desc[0].bDescriptorType	= DYNAMIC */
	/*.desc[0].wDescriptorLenght	= DYNAMIC */
};

/* Super-Speed Support */

static struct usb_endpoint_descriptor hidg_ss_in_ep_desc = {
	.bLength		= USB_DT_ENDPOINT_SIZE,
	.bDescriptorType	= USB_DT_ENDPOINT,
	.bEndpointAddress	= USB_DIR_IN,
	.bmAttributes		= USB_ENDPOINT_XFER_INT,
	/*.wMaxPacketSize	= DYNAMIC */
	.bInterval		= 1, /* FIXME: Add this field in the
				      * HID gadget configuration?
				      * (struct hidg_func_descriptor)
				      */
};

static struct usb_ss_ep_comp_descriptor hidg_ss_in_comp_desc = {
	.bLength                = sizeof(hidg_ss_in_comp_desc),
	.bDescriptorType        = USB_DT_SS_ENDPOINT_COMP,

	/* .bMaxBurst           = 0, */
	/* .bmAttributes        = 0, */
	/* .wBytesPerInterval   = DYNAMIC */
};

static struct usb_descriptor_header *hidg_ss_descriptors[] = {
	(struct usb_descriptor_header *)&hidg_interface_desc,
	(struct usb_descriptor_header *)&hidg_desc,
	(struct usb_descriptor_header *)&hidg_ss_in_ep_desc,
	(struct usb_descriptor_header *)&hidg_ss_in_comp_desc,
	NULL,
};

/* High-Speed Support */

static struct usb_endpoint_descriptor hidg_hs_in_ep_desc = {
	.bLength		= USB_DT_ENDPOINT_SIZE,
	.bDescriptorType	= USB_DT_ENDPOINT,
	.bEndpointAddress	= USB_DIR_IN,
	.bmAttributes		= USB_ENDPOINT_XFER_INT,
	/*.wMaxPacketSize	= DYNAMIC */
	.bInterval		= 1, /* FIXME: Add this field in the
				      * HID gadget configuration?
				      * (struct hidg_func_descriptor)
				      */
};

static struct usb_descriptor_header *hidg_hs_descriptors[] = {
	(struct usb_descriptor_header *)&hidg_interface_desc,
	(struct usb_descriptor_header *)&hidg_desc,
	(struct usb_descriptor_header *)&hidg_hs_in_ep_desc,
	NULL,
};

/* Full-Speed Support */

static struct usb_endpoint_descriptor hidg_fs_in_ep_desc = {
	.bLength		= USB_DT_ENDPOINT_SIZE,
	.bDescriptorType	= USB_DT_ENDPOINT,
	.bEndpointAddress	= USB_DIR_IN,
	.bmAttributes		= USB_ENDPOINT_XFER_INT,
	/*.wMaxPacketSize	= DYNAMIC */
	.bInterval		= 1, /* FIXME: Add this field in the
				       * HID gadget configuration?
				       * (struct hidg_func_descriptor)
				       */
};

static struct usb_descriptor_header *hidg_fs_descriptors[] = {
	(struct usb_descriptor_header *)&hidg_interface_desc,
	(struct usb_descriptor_header *)&hidg_desc,
	(struct usb_descriptor_header *)&hidg_fs_in_ep_desc,
	NULL,
};

/*-------------------------------------------------------------------------*/
/*                                 Strings                                 */

#define CT_FUNC_HID_IDX	0

static struct usb_string ct_func_string_defs[] = {
	[CT_FUNC_HID_IDX].s	= "RTK HID Interface",
	{},			/* end of list */
};

static struct usb_gadget_strings ct_func_string_table = {
	.language	= 0x0409,	/* en-US */
	.strings	= ct_func_string_defs,
};

static struct usb_gadget_strings *ct_func_strings[] = {
	&ct_func_string_table,
	NULL,
};
static void f_hidg_req_complete(struct usb_ep *ep, struct usb_request *req);

/******************************Remote wakeup************************************************/
#include <linux/workqueue.h>
#include "../../../usb/dwc2/core.h"
static bool g_in_suspend_sate = false;
static unsigned int g_usb_wake_up_count = 0;
static volatile bool g_in_usb_wake_state = false;

static inline struct dwc2_hsotg *to_hsotg(struct usb_gadget *gadget)
{
	return container_of(gadget, struct dwc2_hsotg, gadget);
}

static void do_usb_remote_wake_bounce(struct work_struct *wake_up_work)
{
	g_in_usb_wake_state = false;	
}
DECLARE_DELAYED_WORK(g_wakeup_bounce_work_struct, do_usb_remote_wake_bounce);

static void do_remote_wakeup(struct usb_gadget *gadget)
{
	if(gadget) {
		struct dwc2_hsotg *dwc2 = to_hsotg(gadget);
		if(dwc2 && dwc2->lx_state == DWC2_L2) {			
			if(dwc2_readl(dwc2, DSTS) & 0x1) {
				u32 dctl;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
				u32 pcgctl;
				/* Clear the Gate hclk. */
				pcgctl = dwc2_readl(dwc2, PCGCTL);
				pcgctl &= ~PCGCTL_GATEHCLK;
				dwc2_writel(dwc2, pcgctl, PCGCTL);
				udelay(5);

				/* Phy Clock bit. */
				pcgctl = dwc2_readl(dwc2, PCGCTL);
				pcgctl &= ~PCGCTL_STOPPCLK;
				dwc2_writel(dwc2, pcgctl, PCGCTL);
				udelay(5);
#endif				

				/* Set Remote Wakeup Signaling */
				dctl = dwc2_readl(dwc2, DCTL);
				dctl |= DCTL_RMTWKUPSIG;
				dwc2_writel(dwc2, dctl, DCTL);
				mdelay(10);
				dctl = dwc2_readl(dwc2, DCTL);
				dctl &= (~DCTL_RMTWKUPSIG);
				dwc2_writel(dwc2, dctl, DCTL);
				RTK_COMPOSITE_INFO("hid send remote wake done\n");
				mdelay(10);
				usb_gadget_disconnect(gadget);				
				usb_gadget_connect(gadget);
			}
			dwc2->lx_state = DWC2_L0;
			dwc2->bus_suspended = false;
			g_in_usb_wake_state = true;
			schedule_delayed_work(&g_wakeup_bounce_work_struct, 10 * HZ);			
		}
	}
}

static void do_usb_remote_wake(struct work_struct *wake_up_work)
{
	struct f_hidg *hidg  = NULL;
	const char *gadget_name = NULL;
	
	hidg = g_hidg_array[0];
	if (!hidg || !hidg->in_ep || !hidg->in_ep->enabled 
		|| !hidg->func.config->cdev || !hidg->func.config->cdev->gadget)
		return;
	gadget_name = hidg->func.config->cdev->gadget->name;
	if(g_in_suspend_sate && gadget_name && strstr(gadget_name, "dwc2")) {
		do_remote_wakeup(hidg->func.config->cdev->gadget);
	}		
}
DECLARE_WORK(g_wakeup_work_struct, do_usb_remote_wake);

void f_hidg_do_usb_wake(void)
{
	if(get_product_type() == PRODUCT_TYPE_DIAS) {
		if(g_in_suspend_sate) {
			if(g_usb_wake_up_count == 0) {
				schedule_work(&g_wakeup_work_struct);
				g_usb_wake_up_count++;
			}
		}
	}
}

bool f_hidg_in_usb_remote_wake(void)
{
	return  g_in_usb_wake_state;
}

ssize_t f_hidg_send_data(int id, const unsigned char *buffer, unsigned int count, unsigned int f_flags)
{
	struct f_hidg *hidg  = NULL;
	struct usb_request *req;
	unsigned long flags;
	ssize_t status = -ENOMEM;

	if(id < 0 || id >= HIDG_MINORS)
		return -EFAULT;

	hidg = g_hidg_array[id];

	if (!hidg || !hidg->in_ep || !hidg->in_ep->enabled)
		return -EFAULT;

	spin_lock_irqsave(&hidg->write_spinlock, flags);

#define WRITE_COND (!hidg->write_pending)
try_again:
	/* write queue */
	if(in_interrupt()) {
		if (!WRITE_COND) {
			spin_unlock_irqrestore(&hidg->write_spinlock, flags);
			return -ERESTARTSYS;
		}
	} else {
		while (!WRITE_COND) {
			spin_unlock_irqrestore(&hidg->write_spinlock, flags);
			if (f_flags & O_NONBLOCK)
				return -EAGAIN;

			if (wait_event_interruptible_exclusive(
					hidg->write_queue, WRITE_COND))
				return -ERESTARTSYS;

			spin_lock_irqsave(&hidg->write_spinlock, flags);
		}
	}
	hidg->write_pending = 1;
	req = hidg->req;
	count  = min_t(unsigned, count, hidg->report_length);

	spin_unlock_irqrestore(&hidg->write_spinlock, flags);
	
	if(!req) {
		status = -EINVAL;
		goto release_write_pending;
	}
	memcpy(req->buf, buffer, count);
        
	spin_lock_irqsave(&hidg->write_spinlock, flags);

	/* when our function has been disabled by host */
	if (!hidg->req) {
		free_ep_req(hidg->in_ep, req);
		/*
		 * TODO
		 * Should we fail with error here?
		 */
		goto try_again;
	}
	
	req->status   = 0;
	req->zero     = 0;
	req->length   = count;
	req->complete = f_hidg_req_complete;
	req->context  = hidg;

	spin_unlock_irqrestore(&hidg->write_spinlock, flags);

	status = usb_ep_queue(hidg->in_ep, req, GFP_ATOMIC);
	if (status < 0) {
		//ERROR(hidg->func.config->cdev,
		//	"usb_ep_queue error on int endpoint %zd\n", status);
		goto release_write_pending;
	} else {
		status = count;
	}

	return status;
release_write_pending:
	spin_lock_irqsave(&hidg->write_spinlock, flags);
	hidg->write_pending = 0;
	spin_unlock_irqrestore(&hidg->write_spinlock, flags);

	wake_up(&hidg->write_queue);

	return status;
}



/*-------------------------------------------------------------------------*/
/*                              Char Device                                */

static ssize_t f_hidg_read(struct file *file, char __user *buffer,
			size_t count, loff_t *ptr)
{
	return 0;
}

static void f_hidg_req_complete(struct usb_ep *ep, struct usb_request *req)
{
	struct f_hidg *hidg = (struct f_hidg *)ep->driver_data;
	unsigned long flags;

	if (req->status != 0) {
		ERROR(hidg->func.config->cdev,
			"End Point Request ERROR: %d\n", req->status);
	}

	spin_lock_irqsave(&hidg->write_spinlock, flags);
	hidg->write_pending = 0;
	spin_unlock_irqrestore(&hidg->write_spinlock, flags);
	wake_up(&hidg->write_queue);
}

static ssize_t f_hidg_write(struct file *file, const char __user *buffer,
			    size_t count, loff_t *offp)
{
	struct f_hidg *hidg  = file->private_data;
	struct usb_request *req;
	unsigned long flags;
	ssize_t status = -ENOMEM;

	if (!access_ok(buffer, count))
		return -EFAULT;

	spin_lock_irqsave(&hidg->write_spinlock, flags);

#define WRITE_COND (!hidg->write_pending)
try_again:
	/* write queue */
	while (!WRITE_COND) {
		spin_unlock_irqrestore(&hidg->write_spinlock, flags);
		if (file->f_flags & O_NONBLOCK)
			return -EAGAIN;

		if (wait_event_interruptible_exclusive(
				hidg->write_queue, WRITE_COND))
			return -ERESTARTSYS;

		spin_lock_irqsave(&hidg->write_spinlock, flags);
	}

	hidg->write_pending = 1;
	req = hidg->req;
	count  = min_t(unsigned, count, hidg->report_length);

	spin_unlock_irqrestore(&hidg->write_spinlock, flags);

	if(!req) {
		status = -EINVAL;
		goto release_write_pending;
	}
	status = copy_from_user(req->buf, buffer, count);
	if (status != 0) {
		ERROR(hidg->func.config->cdev,
			"copy_from_user error\n");
		status = -EINVAL;
		goto release_write_pending;
	}

	spin_lock_irqsave(&hidg->write_spinlock, flags);

	/* when our function has been disabled by host */
	if (!hidg->req) {
		free_ep_req(hidg->in_ep, req);
		/*
		 * TODO
		 * Should we fail with error here?
		 */
		goto try_again;
	}
	
	req->status   = 0;
	req->zero     = 0;
	req->length   = count;
	req->complete = f_hidg_req_complete;
	req->context  = hidg;

	spin_unlock_irqrestore(&hidg->write_spinlock, flags);

	status = usb_ep_queue(hidg->in_ep, req, GFP_ATOMIC);
	if (status < 0) {
		ERROR(hidg->func.config->cdev,
			"usb_ep_queue error on int endpoint %zd\n", status);
		goto release_write_pending;
	} else {
		status = count;
	}

	return status;
release_write_pending:
	spin_lock_irqsave(&hidg->write_spinlock, flags);
	hidg->write_pending = 0;
	spin_unlock_irqrestore(&hidg->write_spinlock, flags);

	wake_up(&hidg->write_queue);

	return status;
}

static __poll_t f_hidg_poll(struct file *file, poll_table *wait)
{
	struct f_hidg	*hidg  = file->private_data;
	__poll_t	ret = 0;

	poll_wait(file, &hidg->read_queue, wait);
	poll_wait(file, &hidg->write_queue, wait);

	if (WRITE_COND)
		ret |= EPOLLOUT | EPOLLWRNORM;

	return ret;
}

#undef WRITE_COND
#undef READ_COND

static int f_hidg_release(struct inode *inode, struct file *fd)
{
	fd->private_data = NULL;
	return 0;
}

static int f_hidg_open(struct inode *inode, struct file *fd)
{
	struct f_hidg *hidg =
		container_of(inode->i_cdev, struct f_hidg, cdev);

	fd->private_data = hidg;

	return 0;
}

/*-------------------------------------------------------------------------*/
/*                                usb_function                             */

static inline struct usb_request *hidg_alloc_ep_req(struct usb_ep *ep,
						    unsigned length)
{
	struct usb_request      *req;

	req = usb_ep_alloc_request(ep, GFP_ATOMIC);
	if (req) {
		req->length = usb_endpoint_dir_out(ep->desc) ?
			usb_ep_align(ep, length) : length;
		req->buf = kmalloc(req->length, GFP_ATOMIC);
		if (!req->buf) {
			usb_ep_free_request(ep, req);
			req = NULL;
		}
	}
	return req;
}

static int hidg_setup(struct usb_function *f,
		const struct usb_ctrlrequest *ctrl)
{
	struct f_hidg			*hidg = func_to_hidg(f);
	struct usb_composite_dev	*cdev = f->config->cdev;
	struct usb_request		*req  = cdev->req;
	int status = 0;
	__u16 value, length;

	value	= __le16_to_cpu(ctrl->wValue);
	length	= __le16_to_cpu(ctrl->wLength);

	VDBG(cdev,
	     "%s crtl_request : bRequestType:0x%x bRequest:0x%x Value:0x%x\n",
	     __func__, ctrl->bRequestType, ctrl->bRequest, value);

	switch ((ctrl->bRequestType << 8) | ctrl->bRequest) {
	case ((USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8
		  | HID_REQ_GET_REPORT):
		VDBG(cdev, "get_report\n");

		/* send an empty report */
		length = min_t(unsigned, length, hidg->report_length);
		memset(req->buf, 0x0, length);

		goto respond;
		break;

	case ((USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8
		  | HID_REQ_GET_PROTOCOL):
		VDBG(cdev, "get_protocol\n");
		length = min_t(unsigned int, length, 1);
		((u8 *) req->buf)[0] = hidg->protocol;
		goto respond;
		break;

	case ((USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8
		  | HID_REQ_SET_REPORT):
		VDBG(cdev, "set_report | wLength=%d\n", ctrl->wLength);
		goto stall;
		break;

	case ((USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8
		  | HID_REQ_SET_PROTOCOL):
		VDBG(cdev, "set_protocol\n");
		if (value > HID_REPORT_PROTOCOL)
			goto stall;
		length = 0;
		/*
		 * We assume that programs implementing the Boot protocol
		 * are also compatible with the Report Protocol
		 */
		if (hidg->bInterfaceSubClass == USB_INTERFACE_SUBCLASS_BOOT) {
			hidg->protocol = value;
			goto respond;
		}
		goto stall;
		break;

	case ((USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_INTERFACE) << 8
		  | USB_REQ_GET_DESCRIPTOR):
		switch (value >> 8) {
		case HID_DT_HID:
		{
			struct hid_descriptor hidg_desc_copy = hidg_desc;

			VDBG(cdev, "USB_REQ_GET_DESCRIPTOR: HID\n");
			hidg_desc_copy.desc[0].bDescriptorType = HID_DT_REPORT;
			hidg_desc_copy.desc[0].wDescriptorLength =
				cpu_to_le16(hidg->report_desc_length);

			length = min_t(unsigned short, length,
						   hidg_desc_copy.bLength);
			memcpy(req->buf, &hidg_desc_copy, length);
			goto respond;
			break;
		}
		case HID_DT_REPORT:
			VDBG(cdev, "USB_REQ_GET_DESCRIPTOR: REPORT\n");
			length = min_t(unsigned short, length,
						   hidg->report_desc_length);
			memcpy(req->buf, hidg->report_desc, length);
			goto respond;
			break;

		default:
			VDBG(cdev, "Unknown descriptor request 0x%x\n",
				 value >> 8);
			goto stall;
			break;
		}
		break;

	default:
		VDBG(cdev, "Unknown request 0x%x\n",
			 ctrl->bRequest);
		goto stall;
		break;
	}

stall:
	return -EOPNOTSUPP;

respond:
	req->zero = 0;
	req->length = length;
	status = usb_ep_queue(cdev->gadget->ep0, req, GFP_ATOMIC);
	if (status < 0)
		ERROR(cdev, "usb_ep_queue error on ep0 %d\n", value);
	return status;
}

static void hidg_disable(struct usb_function *f)
{
	struct f_hidg *hidg = func_to_hidg(f);
	unsigned long flags;
	g_in_suspend_sate = false;		
	g_usb_wake_up_count = 0;
	usb_ep_disable(hidg->in_ep);

	spin_lock_irqsave(&hidg->write_spinlock, flags);
	if (!hidg->write_pending) {
		free_ep_req(hidg->in_ep, hidg->req);
		hidg->write_pending = 1;
	}

	hidg->req = NULL;
	spin_unlock_irqrestore(&hidg->write_spinlock, flags);
}

static int hidg_set_alt(struct usb_function *f, unsigned intf, unsigned alt)
{
	struct usb_composite_dev		*cdev = f->config->cdev;
	struct f_hidg				*hidg = func_to_hidg(f);
	struct usb_request			*req_in = NULL;
	unsigned long				flags;
	int status = 0;

	VDBG(cdev, "hidg_set_alt intf:%d alt:%d\n", intf, alt);
	g_in_suspend_sate = false;	
	g_usb_wake_up_count = 0;
	if (hidg->in_ep != NULL) {
		/* restart endpoint */
		usb_ep_disable(hidg->in_ep);

		status = config_ep_by_speed(f->config->cdev->gadget, f,
					    hidg->in_ep);
		if (status) {
			ERROR(cdev, "config_ep_by_speed FAILED!\n");
			goto fail;
		}
		status = usb_ep_enable(hidg->in_ep);
		if (status < 0) {
			ERROR(cdev, "Enable IN endpoint FAILED!\n");
			goto fail;
		}
		hidg->in_ep->driver_data = hidg;

		req_in = hidg_alloc_ep_req(hidg->in_ep, hidg->report_length);
		if (!req_in) {
			status = -ENOMEM;
			goto disable_ep_in;
		}
	}


	if (hidg->in_ep != NULL) {
		spin_lock_irqsave(&hidg->write_spinlock, flags);
		hidg->req = req_in;
		hidg->write_pending = 0;
		spin_unlock_irqrestore(&hidg->write_spinlock, flags);

		wake_up(&hidg->write_queue);
	}
	return 0;
disable_ep_in:
	if (hidg->in_ep)
		usb_ep_disable(hidg->in_ep);

fail:
	return status;
}

static const struct file_operations f_hidg_fops = {
	.owner		= THIS_MODULE,
	.open		= f_hidg_open,
	.release	= f_hidg_release,
	.write		= f_hidg_write,
	.read		= f_hidg_read,
	.poll		= f_hidg_poll,
	.llseek		= noop_llseek,
};

static int hidg_bind(struct usb_configuration *c, struct usb_function *f)
{
	struct usb_ep		*ep;
	struct f_hidg		*hidg = func_to_hidg(f);
	struct usb_string	*us;
	struct device		*device;
	int			status;
	dev_t			dev;

	/* maybe allocate device-global string IDs, and patch descriptors */
	us = usb_gstrings_attach(c->cdev, ct_func_strings,
				 ARRAY_SIZE(ct_func_string_defs));
	if (IS_ERR(us))
		return PTR_ERR(us);
	hidg_interface_desc.iInterface = us[CT_FUNC_HID_IDX].id;

	/* allocate instance-specific interface IDs, and patch descriptors */
	status = usb_interface_id(c, f);
	if (status < 0)
		goto fail;
	hidg_interface_desc.bInterfaceNumber = status;

	/* allocate instance-specific endpoints */
	status = -ENODEV;
	ep = usb_ep_autoconfig(c->cdev->gadget, &hidg_fs_in_ep_desc);
	if (!ep)
		goto fail;
	hidg->in_ep = ep;

	/* set descriptor dynamic values */
	hidg_interface_desc.bInterfaceSubClass = hidg->bInterfaceSubClass;
	hidg_interface_desc.bInterfaceProtocol = hidg->bInterfaceProtocol;
	hidg->protocol = HID_REPORT_PROTOCOL;
	hidg_ss_in_ep_desc.wMaxPacketSize = cpu_to_le16(hidg->report_length);
	hidg_ss_in_comp_desc.wBytesPerInterval =
				cpu_to_le16(hidg->report_length);
	hidg_hs_in_ep_desc.wMaxPacketSize = cpu_to_le16(hidg->report_length);
	hidg_fs_in_ep_desc.wMaxPacketSize = cpu_to_le16(hidg->report_length);
        
	/*
	 * We can use hidg_desc struct here but we should not relay
	 * that its content won't change after returning from this function.
	 */
	hidg_desc.desc[0].bDescriptorType = HID_DT_REPORT;
	hidg_desc.desc[0].wDescriptorLength =
		cpu_to_le16(hidg->report_desc_length);

	hidg_hs_in_ep_desc.bEndpointAddress =
		hidg_fs_in_ep_desc.bEndpointAddress;

	hidg_ss_in_ep_desc.bEndpointAddress =
		hidg_fs_in_ep_desc.bEndpointAddress;

	status = usb_assign_descriptors(f, hidg_fs_descriptors,
			hidg_hs_descriptors, hidg_ss_descriptors, NULL);
	if (status)
		goto fail;

	spin_lock_init(&hidg->write_spinlock);
	hidg->write_pending = 1;
	hidg->req = NULL;
	spin_lock_init(&hidg->read_spinlock);
	init_waitqueue_head(&hidg->write_queue);
	init_waitqueue_head(&hidg->read_queue);
	INIT_LIST_HEAD(&hidg->completed_out_req);

	/* create char device */
	cdev_init(&hidg->cdev, &f_hidg_fops);
	dev = MKDEV(major, hidg->minor);
	status = cdev_add(&hidg->cdev, dev, 1);
	if (status)
		goto fail_free_descs;

	device = device_create(hidg_class, NULL, dev, NULL, hidg->nid);
	if (IS_ERR(device)) {
		status = PTR_ERR(device);
		goto del;
	}

	g_hidg_array[hidg->minor] = hidg;

	return 0;
del:
	cdev_del(&hidg->cdev);
fail_free_descs:
	usb_free_all_descriptors(f);
fail:
	ERROR(f->config->cdev, "hidg_bind FAILED\n");
	if (hidg->req != NULL)
		free_ep_req(hidg->in_ep, hidg->req);

	return status;
}


static inline struct f_hid_opts *to_f_hid_opts(struct config_item *item)
{
	return container_of(to_config_group(item), struct f_hid_opts,
			    func_inst.group);
}

static void hid_attr_release(struct config_item *item)
{
	struct f_hid_opts *opts = to_f_hid_opts(item);

	usb_put_function_instance(&opts->func_inst);
}

static struct configfs_item_operations hidg_item_ops = {
	.release	= hid_attr_release,
};

#define F_HID_OPT(name, prec, limit)					\
static ssize_t f_hid_opts_##name##_show(struct config_item *item, char *page)\
{									\
	struct f_hid_opts *opts = to_f_hid_opts(item);			\
	int result;							\
									\
	mutex_lock(&opts->lock);					\
	result = sprintf(page, "%d\n", opts->name);			\
	mutex_unlock(&opts->lock);					\
									\
	return result;							\
}									\
									\
static ssize_t f_hid_opts_##name##_store(struct config_item *item,	\
					 const char *page, size_t len)	\
{									\
	struct f_hid_opts *opts = to_f_hid_opts(item);			\
	int ret;							\
	u##prec num;							\
									\
	mutex_lock(&opts->lock);					\
	if (opts->refcnt) {						\
		ret = -EBUSY;						\
		goto end;						\
	}								\
									\
	ret = kstrtou##prec(page, 0, &num);				\
	if (ret)							\
		goto end;						\
									\
	if (num > limit) {						\
		ret = -EINVAL;						\
		goto end;						\
	}								\
	opts->name = num;						\
	ret = len;							\
									\
end:									\
	mutex_unlock(&opts->lock);					\
	return ret;							\
}									\
									\
CONFIGFS_ATTR(f_hid_opts_, name)

F_HID_OPT(subclass, 8, 255);
F_HID_OPT(protocol, 8, 255);
F_HID_OPT(report_length, 16, 65535);

static ssize_t f_hid_opts_report_desc_show(struct config_item *item, char *page)
{
	struct f_hid_opts *opts = to_f_hid_opts(item);
	int result;

	mutex_lock(&opts->lock);
	result = opts->report_desc_length;
	memcpy(page, opts->report_desc, opts->report_desc_length);
	mutex_unlock(&opts->lock);

	return result;
}

static ssize_t f_hid_opts_report_desc_store(struct config_item *item,
					    const char *page, size_t len)
{
	struct f_hid_opts *opts = to_f_hid_opts(item);
	int ret = -EBUSY;
	char *d;

	mutex_lock(&opts->lock);

	if (opts->refcnt)
		goto end;
	if (len > PAGE_SIZE) {
		ret = -ENOSPC;
		goto end;
	}
	d = kmemdup(page, len, GFP_KERNEL);
	if (!d) {
		ret = -ENOMEM;
		goto end;
	}
	kfree(opts->report_desc);
	opts->report_desc = d;
	opts->report_desc_length = len;
	opts->report_desc_alloc = true;
	ret = len;
end:
	mutex_unlock(&opts->lock);
	return ret;
}

CONFIGFS_ATTR(f_hid_opts_, report_desc);

static ssize_t f_hid_opts_dev_show(struct config_item *item, char *page)
{
	struct f_hid_opts *opts = to_f_hid_opts(item);

	return sprintf(page, "%d:%d\n", major, opts->minor);
}

CONFIGFS_ATTR_RO(f_hid_opts_, dev);

static struct configfs_attribute *hid_attrs[] = {
	&f_hid_opts_attr_subclass,
	&f_hid_opts_attr_protocol,
	&f_hid_opts_attr_report_length,
	&f_hid_opts_attr_report_desc,
	&f_hid_opts_attr_dev,
	NULL,
};

static const struct config_item_type hid_func_type = {
	.ct_item_ops	= &hidg_item_ops,
	.ct_attrs	= hid_attrs,
	.ct_owner	= THIS_MODULE,
};


static void hidg_free_inst(struct usb_function_instance *f)
{
	struct f_hid_opts *opts;

	opts = container_of(f, struct f_hid_opts, func_inst);

	mutex_lock(&hidg_func_inst_cnt_lock);

	g_alloc_func_inst_cnt--;
	if (g_alloc_func_inst_cnt == 0)
		rtk_ghid_cleanup();

	mutex_unlock(&hidg_func_inst_cnt_lock);

	if (opts->report_desc_alloc)
		kfree(opts->report_desc);

	kfree(opts);
}

static int hidg_set_inst_name(struct usb_function_instance *f,
			      const char *name)
{
	struct f_hid_opts *opts;
	struct rtk_hidg_report_desc_data * report_desc_data= NULL;
	
	opts = container_of(f, struct f_hid_opts, func_inst);

	if(!name)
		return -EINVAL;
	report_desc_data = rtk_hidg_get_hid_func_node_by_name(name);
	if(!report_desc_data)
		return -EINVAL;
	opts->subclass = report_desc_data->func_desc->subclass;
	opts->protocol =report_desc_data->func_desc->protocol;
	opts->report_length = report_desc_data->func_desc->report_length;
	opts->report_desc_length = report_desc_data->func_desc->report_desc_length;
	opts->report_desc = report_desc_data->func_desc->report_desc;
	snprintf(opts->nid, MAX_NID_SIZE, "%s", report_desc_data->nid);
	opts->minor = report_desc_data->id;
	return 0;
}

static struct usb_function_instance *hidg_alloc_inst(void)
{
	struct f_hid_opts *opts;
	struct usb_function_instance *ret;
	int status = 0;

	opts = kzalloc(sizeof(*opts), GFP_KERNEL);
	if (!opts)
		return ERR_PTR(-ENOMEM);
	mutex_init(&opts->lock);
	opts->func_inst.free_func_inst = hidg_free_inst;
	opts->func_inst.set_inst_name = hidg_set_inst_name;
	ret = &opts->func_inst;

	mutex_lock(&hidg_func_inst_cnt_lock);

	if (g_alloc_func_inst_cnt == 0) {	
		status = rtk_ghid_setup(NULL, HIDG_MINORS);
		if (status)  {
			ret = ERR_PTR(status);
			kfree(opts);
			goto unlock;
		}
	}
	g_alloc_func_inst_cnt++;
	
	config_group_init_type_name(&opts->func_inst.group, "", &hid_func_type);

unlock:
	mutex_unlock(&hidg_func_inst_cnt_lock);
	return ret;
}

static void hidg_free(struct usb_function *f)
{
	struct f_hidg *hidg;
	struct f_hid_opts *opts;

	hidg = func_to_hidg(f);
	opts = container_of(f->fi, struct f_hid_opts, func_inst);
	kfree(hidg->report_desc);
	kfree(hidg);
	mutex_lock(&opts->lock);
	--opts->refcnt;
	mutex_unlock(&opts->lock);
}

static void hidg_unbind(struct usb_configuration *c, struct usb_function *f)
{
	struct f_hidg *hidg = func_to_hidg(f);
	g_hidg_array[hidg->minor] = NULL;

	device_destroy(hidg_class, MKDEV(major, hidg->minor));
	cdev_del(&hidg->cdev);

	usb_free_all_descriptors(f);
}

void hidg_suspend(struct usb_function *func)
{
	struct f_hidg *hidg = func_to_hidg(func);
	if(hidg->minor == 0) {
		RTK_COMPOSITE_INFO("%s, %d\n", __FUNCTION__, __LINE__);
		g_in_suspend_sate = true;
		g_usb_wake_up_count = 0;
		
	}
}
void hidg_resume(struct usb_function *func)
{
	struct f_hidg *hidg = func_to_hidg(func);
	if(hidg->minor == 0) {
		RTK_COMPOSITE_INFO("%s, %d\n", __FUNCTION__, __LINE__);
		g_in_suspend_sate = false;
	}
}

static struct usb_function *hidg_alloc(struct usb_function_instance *fi)
{
	struct f_hidg *hidg;
	struct f_hid_opts *opts;

	/* allocate and initialize one new instance */
	hidg = kzalloc(sizeof(*hidg), GFP_KERNEL);
	if (!hidg)
		return ERR_PTR(-ENOMEM);

	opts = container_of(fi, struct f_hid_opts, func_inst);

	mutex_lock(&opts->lock);
	++opts->refcnt;

	snprintf(hidg->nid, MAX_NID_SIZE, "%s", opts->nid);
	hidg->minor = opts->minor;
	hidg->bInterfaceSubClass = opts->subclass;
	hidg->bInterfaceProtocol = opts->protocol;
	hidg->report_length = opts->report_length;
	hidg->report_desc_length = opts->report_desc_length;
	if (opts->report_desc) {
		hidg->report_desc = kmemdup(opts->report_desc,
					    opts->report_desc_length,
					    GFP_KERNEL);
		if (!hidg->report_desc) {
			kfree(hidg);
			mutex_unlock(&opts->lock);
			return ERR_PTR(-ENOMEM);
		}
	}

	mutex_unlock(&opts->lock);

	hidg->func.name    = "hid";
	hidg->func.bind    = hidg_bind;
	hidg->func.unbind  = hidg_unbind;
	hidg->func.set_alt = hidg_set_alt;
	hidg->func.disable = hidg_disable;
	hidg->func.suspend = hidg_suspend;
	hidg->func.resume = hidg_resume;
	hidg->func.setup   = hidg_setup;
	hidg->func.free_func = hidg_free;

	/* this could me made configurable at some point */
	hidg->qlen	   = 4;

	return &hidg->func;
}

//DECLARE_USB_FUNCTION_INIT(rtk_hid, hidg_alloc_inst, hidg_alloc);

static struct usb_function_driver rtk_hid_usb_func = {
	.name = "rtk_hid",
	.mod  = THIS_MODULE,
	.alloc_inst = hidg_alloc_inst,
	.alloc_func = hidg_alloc,	
};

int __init f_hidg_init(void)
{
	return usb_function_register(&rtk_hid_usb_func);
}

void f_hidg_uninit(void)
{
	usb_function_unregister(&rtk_hid_usb_func);
}

static int rtk_ghid_setup(struct usb_gadget *g, int count)
{
	int status;
	dev_t dev;

	hidg_class = class_create(THIS_MODULE, "rtk_hidg");
	if (IS_ERR(hidg_class)) {
		status = PTR_ERR(hidg_class);
		hidg_class = NULL;
		return status;
	}

	status = alloc_chrdev_region(&dev, 0, count, "rtk_hidg");
	if (status) {
		class_destroy(hidg_class);
		hidg_class = NULL;
		return status;
	}

	major = MAJOR(dev);
	minors = count;

	return 0;
}

static void rtk_ghid_cleanup(void)
{
	if (major) {
		unregister_chrdev_region(MKDEV(major, 0), minors);
		major = minors = 0;
	}

	class_destroy(hidg_class);
	hidg_class = NULL;
}
