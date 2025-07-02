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
#include "rtk_hidg_dev_info.h"
#include "u_f.h"
#include "u_hid.h"

#define HIDG_MINORS	8

static int major, minors;
static struct class *hidg_ex_class;
static DEFINE_MUTEX(hidg_ex_func_inst_cnt_lock); /* protects access to g_alloc_func_inst_cnt */
static int g_alloc_func_inst_cnt = 0;
static int rtk_ghid_setup(struct usb_gadget *g, int count);
static void rtk_ghid_cleanup(void);
static struct f_hidg_ex *g_hidg_ex_array[HIDG_MINORS];

/*-------------------------------------------------------------------------*/
/*                            HID gadget struct                            */

struct f_hidg_ex_req_list {
	struct usb_request	*req;
	unsigned int		pos;
	struct list_head 	list;
};

struct f_hidg_ex {
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

static inline struct f_hidg_ex *func_to_hidg_ex(struct usb_function *f)
{
	return container_of(f, struct f_hidg_ex, func);
}

/*-------------------------------------------------------------------------*/
/*                           Static descriptors                            */

static struct usb_interface_descriptor hidg_ex_interface_desc = {
	.bLength		= sizeof hidg_ex_interface_desc,
	.bDescriptorType	= USB_DT_INTERFACE,
	/* .bInterfaceNumber	= DYNAMIC */
	.bAlternateSetting	= 0,
	.bNumEndpoints		= 1,
	.bInterfaceClass	= USB_CLASS_HID,
	/* .bInterfaceSubClass	= DYNAMIC */
	/* .bInterfaceProtocol	= DYNAMIC */
	/* .iInterface		= DYNAMIC */
};

static struct hid_descriptor hidg_ex_desc = {
	.bLength			= sizeof hidg_ex_desc,
	.bDescriptorType		= HID_DT_HID,
	.bcdHID				= 0x0101,
	.bCountryCode			= 0x00,
	.bNumDescriptors		= 0x1,
	/*.desc[0].bDescriptorType	= DYNAMIC */
	/*.desc[0].wDescriptorLenght	= DYNAMIC */
};

/* Super-Speed Support */

static struct usb_endpoint_descriptor hidg_ex_ss_in_ep_desc = {
	.bLength		= USB_DT_ENDPOINT_SIZE,
	.bDescriptorType	= USB_DT_ENDPOINT,
	.bEndpointAddress	= USB_DIR_IN,
	.bmAttributes		= USB_ENDPOINT_XFER_INT,
	/*.wMaxPacketSize	= DYNAMIC */
	.bInterval		= 1, /* FIXME: Add this field in the
				      * HID gadget configuration?
				      * (struct hidg_ex_func_descriptor)
				      */
};

static struct usb_ss_ep_comp_descriptor hidg_ex_ss_in_comp_desc = {
	.bLength                = sizeof(hidg_ex_ss_in_comp_desc),
	.bDescriptorType        = USB_DT_SS_ENDPOINT_COMP,

	/* .bMaxBurst           = 0, */
	/* .bmAttributes        = 0, */
	/* .wBytesPerInterval   = DYNAMIC */
};

static struct usb_descriptor_header *hidg_ex_ss_descriptors[] = {
	(struct usb_descriptor_header *)&hidg_ex_interface_desc,
	(struct usb_descriptor_header *)&hidg_ex_desc,
	(struct usb_descriptor_header *)&hidg_ex_ss_in_ep_desc,
	(struct usb_descriptor_header *)&hidg_ex_ss_in_comp_desc,
	NULL,
};

/* High-Speed Support */

static struct usb_endpoint_descriptor hidg_ex_hs_in_ep_desc = {
	.bLength		= USB_DT_ENDPOINT_SIZE,
	.bDescriptorType	= USB_DT_ENDPOINT,
	.bEndpointAddress	= USB_DIR_IN,
	.bmAttributes		= USB_ENDPOINT_XFER_INT,
	/*.wMaxPacketSize	= DYNAMIC */
	.bInterval		= 1, /* FIXME: Add this field in the
				      * HID gadget configuration?
				      * (struct hidg_ex_func_descriptor)
				      */
};

static struct usb_descriptor_header *hidg_ex_hs_descriptors[] = {
	(struct usb_descriptor_header *)&hidg_ex_interface_desc,
	(struct usb_descriptor_header *)&hidg_ex_desc,
	(struct usb_descriptor_header *)&hidg_ex_hs_in_ep_desc,
	NULL,
};

/* Full-Speed Support */

static struct usb_endpoint_descriptor hidg_ex_fs_in_ep_desc = {
	.bLength		= USB_DT_ENDPOINT_SIZE,
	.bDescriptorType	= USB_DT_ENDPOINT,
	.bEndpointAddress	= USB_DIR_IN,
	.bmAttributes		= USB_ENDPOINT_XFER_INT,
	/*.wMaxPacketSize	= DYNAMIC */
	.bInterval		= 1, /* FIXME: Add this field in the
				       * HID gadget configuration?
				       * (struct hidg_ex_func_descriptor)
				       */
};

static struct usb_descriptor_header *hidg_ex_fs_descriptors[] = {
	(struct usb_descriptor_header *)&hidg_ex_interface_desc,
	(struct usb_descriptor_header *)&hidg_ex_desc,
	(struct usb_descriptor_header *)&hidg_ex_fs_in_ep_desc,
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
static void f_hidg_ex_req_complete(struct usb_ep *ep, struct usb_request *req);
ssize_t f_hidg_ex_send_data(int id, const unsigned char *buffer, unsigned int count, unsigned int f_flags)
{
	struct f_hidg_ex *hidg_ex  = NULL;
	struct usb_request *req;
	unsigned long flags;
	ssize_t status = -ENOMEM;

	if(id < 0 || id >= HIDG_MINORS)
		return -EFAULT;

	hidg_ex = g_hidg_ex_array[id];

	if (!hidg_ex || !hidg_ex->in_ep || !hidg_ex->in_ep->enabled)
		return -EFAULT;

	spin_lock_irqsave(&hidg_ex->write_spinlock, flags);

#define WRITE_COND (!hidg_ex->write_pending)
try_again:
	/* write queue */
	if(in_interrupt()) {
		if (!WRITE_COND) {
			spin_unlock_irqrestore(&hidg_ex->write_spinlock, flags);
			return -ERESTARTSYS;
		}
	} else {
		while (!WRITE_COND) {
			spin_unlock_irqrestore(&hidg_ex->write_spinlock, flags);
			if (f_flags & O_NONBLOCK)
				return -EAGAIN;

			if (wait_event_interruptible_exclusive(
					hidg_ex->write_queue, WRITE_COND))
				return -ERESTARTSYS;

			spin_lock_irqsave(&hidg_ex->write_spinlock, flags);
		}
	}
	hidg_ex->write_pending = 1;
	req = hidg_ex->req;
	count  = min_t(unsigned, count, hidg_ex->report_length);

	spin_unlock_irqrestore(&hidg_ex->write_spinlock, flags);
	
	if(!req) {
		status = -EINVAL;
		goto release_write_pending;
	}
	memcpy(req->buf, buffer, count);
        
	spin_lock_irqsave(&hidg_ex->write_spinlock, flags);

	/* when our function has been disabled by host */
	if (!hidg_ex->req) {
		free_ep_req(hidg_ex->in_ep, req);
		/*
		 * TODO
		 * Should we fail with error here?
		 */
		goto try_again;
	}
	
	req->status   = 0;
	req->zero     = 0;
	req->length   = count;
	req->complete = f_hidg_ex_req_complete;
	req->context  = hidg_ex;

	spin_unlock_irqrestore(&hidg_ex->write_spinlock, flags);

	status = usb_ep_queue(hidg_ex->in_ep, req, GFP_ATOMIC);
	if (status < 0) {
		//ERROR(hidg_ex->func.config->cdev,
		//	"usb_ep_queue error on int endpoint %zd\n", status);
		goto release_write_pending;
	} else {
		status = count;
	}

	return status;
release_write_pending:
	spin_lock_irqsave(&hidg_ex->write_spinlock, flags);
	hidg_ex->write_pending = 0;
	spin_unlock_irqrestore(&hidg_ex->write_spinlock, flags);

	wake_up(&hidg_ex->write_queue);

	return status;
}



/*-------------------------------------------------------------------------*/
/*                              Char Device                                */

static ssize_t f_hidg_ex_read(struct file *file, char __user *buffer,
			size_t count, loff_t *ptr)
{
	return 0;
}

static void f_hidg_ex_req_complete(struct usb_ep *ep, struct usb_request *req)
{
	struct f_hidg_ex *hidg_ex = (struct f_hidg_ex *)ep->driver_data;
	unsigned long flags;

	if (req->status != 0) {
		ERROR(hidg_ex->func.config->cdev,
			"End Point Request ERROR: %d\n", req->status);
	}

	spin_lock_irqsave(&hidg_ex->write_spinlock, flags);
	hidg_ex->write_pending = 0;
	spin_unlock_irqrestore(&hidg_ex->write_spinlock, flags);
	wake_up(&hidg_ex->write_queue);
}

static ssize_t f_hidg_ex_write(struct file *file, const char __user *buffer,
			    size_t count, loff_t *offp)
{
	struct f_hidg_ex *hidg_ex  = file->private_data;
	struct usb_request *req;
	unsigned long flags;
	ssize_t status = -ENOMEM;

	if (!access_ok(buffer, count))
		return -EFAULT;

	spin_lock_irqsave(&hidg_ex->write_spinlock, flags);

#define WRITE_COND (!hidg_ex->write_pending)
try_again:
	/* write queue */
	while (!WRITE_COND) {
		spin_unlock_irqrestore(&hidg_ex->write_spinlock, flags);
		if (file->f_flags & O_NONBLOCK)
			return -EAGAIN;

		if (wait_event_interruptible_exclusive(
				hidg_ex->write_queue, WRITE_COND))
			return -ERESTARTSYS;

		spin_lock_irqsave(&hidg_ex->write_spinlock, flags);
	}

	hidg_ex->write_pending = 1;
	req = hidg_ex->req;
	count  = min_t(unsigned, count, hidg_ex->report_length);

	spin_unlock_irqrestore(&hidg_ex->write_spinlock, flags);

	if(!req) {
		status = -EINVAL;
		goto release_write_pending;
	}
	status = copy_from_user(req->buf, buffer, count);
	if (status != 0) {
		ERROR(hidg_ex->func.config->cdev,
			"copy_from_user error\n");
		status = -EINVAL;
		goto release_write_pending;
	}

	spin_lock_irqsave(&hidg_ex->write_spinlock, flags);

	/* when our function has been disabled by host */
	if (!hidg_ex->req) {
		free_ep_req(hidg_ex->in_ep, req);
		/*
		 * TODO
		 * Should we fail with error here?
		 */
		goto try_again;
	}
	
	req->status   = 0;
	req->zero     = 0;
	req->length   = count;
	req->complete = f_hidg_ex_req_complete;
	req->context  = hidg_ex;

	spin_unlock_irqrestore(&hidg_ex->write_spinlock, flags);

	status = usb_ep_queue(hidg_ex->in_ep, req, GFP_ATOMIC);
	if (status < 0) {
		ERROR(hidg_ex->func.config->cdev,
			"usb_ep_queue error on int endpoint %zd\n", status);
		goto release_write_pending;
	} else {
		status = count;
	}

	return status;
release_write_pending:
	spin_lock_irqsave(&hidg_ex->write_spinlock, flags);
	hidg_ex->write_pending = 0;
	spin_unlock_irqrestore(&hidg_ex->write_spinlock, flags);

	wake_up(&hidg_ex->write_queue);

	return status;
}

static __poll_t f_hidg_ex_poll(struct file *file, poll_table *wait)
{
	struct f_hidg_ex	*hidg_ex  = file->private_data;
	__poll_t	ret = 0;

	poll_wait(file, &hidg_ex->read_queue, wait);
	poll_wait(file, &hidg_ex->write_queue, wait);

	if (WRITE_COND)
		ret |= EPOLLOUT | EPOLLWRNORM;

	return ret;
}

#undef WRITE_COND
#undef READ_COND

static int f_hidg_ex_release(struct inode *inode, struct file *fd)
{
	fd->private_data = NULL;
	return 0;
}

static int f_hidg_ex_open(struct inode *inode, struct file *fd)
{
	struct f_hidg_ex *hidg_ex =
		container_of(inode->i_cdev, struct f_hidg_ex, cdev);

	fd->private_data = hidg_ex;

	return 0;
}

/*-------------------------------------------------------------------------*/
/*                                usb_function                             */

static inline struct usb_request *hidg_ex_alloc_ep_req(struct usb_ep *ep,
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

static int hidg_ex_setup(struct usb_function *f,
		const struct usb_ctrlrequest *ctrl)
{
	struct f_hidg_ex			*hidg_ex = func_to_hidg_ex(f);
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
		length = min_t(unsigned, length, hidg_ex->report_length);
		memset(req->buf, 0x0, length);

		goto respond;
		break;

	case ((USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE) << 8
		  | HID_REQ_GET_PROTOCOL):
		VDBG(cdev, "get_protocol\n");
		length = min_t(unsigned int, length, 1);
		((u8 *) req->buf)[0] = hidg_ex->protocol;
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
		if (hidg_ex->bInterfaceSubClass == USB_INTERFACE_SUBCLASS_BOOT) {
			hidg_ex->protocol = value;
			goto respond;
		}
		goto stall;
		break;

	case ((USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_INTERFACE) << 8
		  | USB_REQ_GET_DESCRIPTOR):
		switch (value >> 8) {
		case HID_DT_HID:
		{
			struct hid_descriptor hidg_ex_desc_copy = hidg_ex_desc;

			VDBG(cdev, "USB_REQ_GET_DESCRIPTOR: HID\n");
			hidg_ex_desc_copy.desc[0].bDescriptorType = HID_DT_REPORT;
			hidg_ex_desc_copy.desc[0].wDescriptorLength =
				cpu_to_le16(hidg_ex->report_desc_length);

			length = min_t(unsigned short, length,
						   hidg_ex_desc_copy.bLength);
			memcpy(req->buf, &hidg_ex_desc_copy, length);
			goto respond;
			break;
		}
		case HID_DT_REPORT:
			VDBG(cdev, "USB_REQ_GET_DESCRIPTOR: REPORT\n");
			length = min_t(unsigned short, length,
						   hidg_ex->report_desc_length);
			memcpy(req->buf, hidg_ex->report_desc, length);
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

static void hidg_ex_disable(struct usb_function *f)
{
	struct f_hidg_ex *hidg_ex = func_to_hidg_ex(f);
	unsigned long flags;

	usb_ep_disable(hidg_ex->in_ep);

	spin_lock_irqsave(&hidg_ex->write_spinlock, flags);
	if (!hidg_ex->write_pending) {
		free_ep_req(hidg_ex->in_ep, hidg_ex->req);
		hidg_ex->write_pending = 1;
	}

	hidg_ex->req = NULL;
	spin_unlock_irqrestore(&hidg_ex->write_spinlock, flags);
}

static int hidg_ex_set_alt(struct usb_function *f, unsigned intf, unsigned alt)
{
	struct usb_composite_dev		*cdev = f->config->cdev;
	struct f_hidg_ex				*hidg_ex = func_to_hidg_ex(f);
	struct usb_request			*req_in = NULL;
	unsigned long				flags;
	int status = 0;

	VDBG(cdev, "hidg_ex_set_alt intf:%d alt:%d\n", intf, alt);

	if (hidg_ex->in_ep != NULL) {
		/* restart endpoint */
		usb_ep_disable(hidg_ex->in_ep);

		status = config_ep_by_speed(f->config->cdev->gadget, f,
					    hidg_ex->in_ep);
		if (status) {
			ERROR(cdev, "config_ep_by_speed FAILED!\n");
			goto fail;
		}
		status = usb_ep_enable(hidg_ex->in_ep);
		if (status < 0) {
			ERROR(cdev, "Enable IN endpoint FAILED!\n");
			goto fail;
		}
		hidg_ex->in_ep->driver_data = hidg_ex;

		req_in = hidg_ex_alloc_ep_req(hidg_ex->in_ep, hidg_ex->report_length);
		if (!req_in) {
			status = -ENOMEM;
			goto disable_ep_in;
		}
	}


	if (hidg_ex->in_ep != NULL) {
		spin_lock_irqsave(&hidg_ex->write_spinlock, flags);
		hidg_ex->req = req_in;
		hidg_ex->write_pending = 0;
		spin_unlock_irqrestore(&hidg_ex->write_spinlock, flags);

		wake_up(&hidg_ex->write_queue);
	}
	return 0;
disable_ep_in:
	if (hidg_ex->in_ep)
		usb_ep_disable(hidg_ex->in_ep);

fail:
	return status;
}

static const struct file_operations f_hidg_ex_fops = {
	.owner		= THIS_MODULE,
	.open		= f_hidg_ex_open,
	.release	= f_hidg_ex_release,
	.write		= f_hidg_ex_write,
	.read		= f_hidg_ex_read,
	.poll		= f_hidg_ex_poll,
	.llseek		= noop_llseek,
};

static int hidg_ex_bind(struct usb_configuration *c, struct usb_function *f)
{
	struct usb_ep		*ep;
	struct f_hidg_ex		*hidg_ex = func_to_hidg_ex(f);
	struct usb_string	*us;
	struct device		*device;
	int			status;
	dev_t			dev;

	/* maybe allocate device-global string IDs, and patch descriptors */
	us = usb_gstrings_attach(c->cdev, ct_func_strings,
				 ARRAY_SIZE(ct_func_string_defs));
	if (IS_ERR(us))
		return PTR_ERR(us);
	hidg_ex_interface_desc.iInterface = us[CT_FUNC_HID_IDX].id;

	/* allocate instance-specific interface IDs, and patch descriptors */
	status = usb_interface_id(c, f);
	if (status < 0)
		goto fail;
	hidg_ex_interface_desc.bInterfaceNumber = status;

	/* allocate instance-specific endpoints */
	status = -ENODEV;
	ep = usb_ep_autoconfig(c->cdev->gadget, &hidg_ex_fs_in_ep_desc);
	if (!ep)
		goto fail;
	hidg_ex->in_ep = ep;

	/* set descriptor dynamic values */
	hidg_ex_interface_desc.bInterfaceSubClass = hidg_ex->bInterfaceSubClass;
	hidg_ex_interface_desc.bInterfaceProtocol = hidg_ex->bInterfaceProtocol;
	hidg_ex->protocol = HID_REPORT_PROTOCOL;
	hidg_ex_ss_in_ep_desc.wMaxPacketSize = cpu_to_le16(hidg_ex->report_length);
	hidg_ex_ss_in_comp_desc.wBytesPerInterval =
				cpu_to_le16(hidg_ex->report_length);
	hidg_ex_hs_in_ep_desc.wMaxPacketSize = cpu_to_le16(hidg_ex->report_length);
	hidg_ex_fs_in_ep_desc.wMaxPacketSize = cpu_to_le16(hidg_ex->report_length);
        
	/*
	 * We can use hidg_ex_desc struct here but we should not relay
	 * that its content won't change after returning from this function.
	 */
	hidg_ex_desc.desc[0].bDescriptorType = HID_DT_REPORT;
	hidg_ex_desc.desc[0].wDescriptorLength =
		cpu_to_le16(hidg_ex->report_desc_length);

	hidg_ex_hs_in_ep_desc.bEndpointAddress =
		hidg_ex_fs_in_ep_desc.bEndpointAddress;

	hidg_ex_ss_in_ep_desc.bEndpointAddress =
		hidg_ex_fs_in_ep_desc.bEndpointAddress;

	status = usb_assign_descriptors(f, hidg_ex_fs_descriptors,
			hidg_ex_hs_descriptors, hidg_ex_ss_descriptors, NULL);
	if (status)
		goto fail;

	spin_lock_init(&hidg_ex->write_spinlock);
	hidg_ex->write_pending = 1;
	hidg_ex->req = NULL;
	spin_lock_init(&hidg_ex->read_spinlock);
	init_waitqueue_head(&hidg_ex->write_queue);
	init_waitqueue_head(&hidg_ex->read_queue);
	INIT_LIST_HEAD(&hidg_ex->completed_out_req);

	/* create char device */
	cdev_init(&hidg_ex->cdev, &f_hidg_ex_fops);
	dev = MKDEV(major, hidg_ex->minor);
	status = cdev_add(&hidg_ex->cdev, dev, 1);
	if (status)
		goto fail_free_descs;

	device = device_create(hidg_ex_class, NULL, dev, NULL, hidg_ex->nid);
	if (IS_ERR(device)) {
		status = PTR_ERR(device);
		goto del;
	}

	g_hidg_ex_array[hidg_ex->minor] = hidg_ex;

	return 0;
del:
	cdev_del(&hidg_ex->cdev);
fail_free_descs:
	usb_free_all_descriptors(f);
fail:
	ERROR(f->config->cdev, "hidg_ex_bind FAILED\n");
	if (hidg_ex->req != NULL)
		free_ep_req(hidg_ex->in_ep, hidg_ex->req);

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

static struct configfs_item_operations hidg_ex_item_ops = {
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
	.ct_item_ops	= &hidg_ex_item_ops,
	.ct_attrs	= hid_attrs,
	.ct_owner	= THIS_MODULE,
};


static void hidg_ex_free_inst(struct usb_function_instance *f)
{
	struct f_hid_opts *opts;

	opts = container_of(f, struct f_hid_opts, func_inst);

	mutex_lock(&hidg_ex_func_inst_cnt_lock);

	g_alloc_func_inst_cnt--;
	if (g_alloc_func_inst_cnt == 0)
		rtk_ghid_cleanup();

	mutex_unlock(&hidg_ex_func_inst_cnt_lock);

	if (opts->report_desc_alloc)
		kfree(opts->report_desc);

	kfree(opts);
}

static int hidg_ex_set_inst_name(struct usb_function_instance *f,
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

static struct usb_function_instance *hidg_ex_alloc_inst(void)
{
	struct f_hid_opts *opts;
	struct usb_function_instance *ret;
	int status = 0;

	opts = kzalloc(sizeof(*opts), GFP_KERNEL);
	if (!opts)
		return ERR_PTR(-ENOMEM);
	mutex_init(&opts->lock);
	opts->func_inst.free_func_inst = hidg_ex_free_inst;
	opts->func_inst.set_inst_name = hidg_ex_set_inst_name;
	ret = &opts->func_inst;

	mutex_lock(&hidg_ex_func_inst_cnt_lock);

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
	mutex_unlock(&hidg_ex_func_inst_cnt_lock);
	return ret;
}

static void hidg_ex_free(struct usb_function *f)
{
	struct f_hidg_ex *hidg_ex;
	struct f_hid_opts *opts;

	hidg_ex = func_to_hidg_ex(f);
	opts = container_of(f->fi, struct f_hid_opts, func_inst);
	kfree(hidg_ex->report_desc);
	kfree(hidg_ex);
	mutex_lock(&opts->lock);
	--opts->refcnt;
	mutex_unlock(&opts->lock);
}

static void hidg_ex_unbind(struct usb_configuration *c, struct usb_function *f)
{
	struct f_hidg_ex *hidg_ex = func_to_hidg_ex(f);
	g_hidg_ex_array[hidg_ex->minor] = NULL;

	device_destroy(hidg_ex_class, MKDEV(major, hidg_ex->minor));
	cdev_del(&hidg_ex->cdev);

	usb_free_all_descriptors(f);
}

static struct usb_function *hidg_ex_alloc(struct usb_function_instance *fi)
{
	struct f_hidg_ex *hidg_ex;
	struct f_hid_opts *opts;

	/* allocate and initialize one new instance */
	hidg_ex = kzalloc(sizeof(*hidg_ex), GFP_KERNEL);
	if (!hidg_ex)
		return ERR_PTR(-ENOMEM);

	opts = container_of(fi, struct f_hid_opts, func_inst);

	mutex_lock(&opts->lock);
	++opts->refcnt;

	snprintf(hidg_ex->nid, MAX_NID_SIZE, "%s", opts->nid);
	hidg_ex->minor = opts->minor;
	hidg_ex->bInterfaceSubClass = opts->subclass;
	hidg_ex->bInterfaceProtocol = opts->protocol;
	hidg_ex->report_length = opts->report_length;
	hidg_ex->report_desc_length = opts->report_desc_length;
	if (opts->report_desc) {
		hidg_ex->report_desc = kmemdup(opts->report_desc,
					    opts->report_desc_length,
					    GFP_KERNEL);
		if (!hidg_ex->report_desc) {
			kfree(hidg_ex);
			mutex_unlock(&opts->lock);
			return ERR_PTR(-ENOMEM);
		}
	}

	mutex_unlock(&opts->lock);

	hidg_ex->func.name    = "hid_ex";
	hidg_ex->func.bind    = hidg_ex_bind;
	hidg_ex->func.unbind  = hidg_ex_unbind;
	hidg_ex->func.set_alt = hidg_ex_set_alt;
	hidg_ex->func.disable = hidg_ex_disable;
	hidg_ex->func.setup   = hidg_ex_setup;
	hidg_ex->func.free_func = hidg_ex_free;

	/* this could me made configurable at some point */
	hidg_ex->qlen	   = 4;

	return &hidg_ex->func;
}

//DECLARE_USB_FUNCTION_INIT(rtk_hid, hidg_ex_alloc_inst, hidg_ex_alloc);

static struct usb_function_driver rtk_hid_ex_usb_func = {
	.name = "rtk_hid_ex",
	.mod  = THIS_MODULE,
	.alloc_inst = hidg_ex_alloc_inst,
	.alloc_func = hidg_ex_alloc,	
};

int __init f_hidg_ex_init(void)
{
	return usb_function_register(&rtk_hid_ex_usb_func);
}

void  f_hidg_ex_uninit(void)
{
	usb_function_unregister(&rtk_hid_ex_usb_func);
}

static int rtk_ghid_setup(struct usb_gadget *g, int count)
{
	int status;
	dev_t dev;

	hidg_ex_class = class_create(THIS_MODULE, "rtk_hidg_ex");
	if (IS_ERR(hidg_ex_class)) {
		status = PTR_ERR(hidg_ex_class);
		hidg_ex_class = NULL;
		return status;
	}

	status = alloc_chrdev_region(&dev, 0, count, "rtk_hidg_ex");
	if (status) {
		class_destroy(hidg_ex_class);
		hidg_ex_class = NULL;
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

	class_destroy(hidg_ex_class);
	hidg_ex_class = NULL;
}
