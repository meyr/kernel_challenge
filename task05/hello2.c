#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/hid.h>

/* Define these values to match your devices */
#define USB_SKEL_VENDOR_ID	0xFFF0
#define USB_SKEL_PRODUCT_ID	0xFFF0

/* table of devices that work with this driver */
static const struct usb_device_id my_usb_table[] = {
	/* special vendor id , product id device */
	{ USB_DEVICE(USB_SKEL_VENDOR_ID, USB_SKEL_PRODUCT_ID) },
	/* any USB keyboard */
	{ USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID,
				USB_INTERFACE_SUBCLASS_BOOT,
				USB_INTERFACE_PROTOCOL_KEYBOARD) },
	/* any USB Mouse */
	{ USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID,
				USB_INTERFACE_SUBCLASS_BOOT,
				USB_INTERFACE_PROTOCOL_MOUSE) },
	{ }					/* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, my_usb_table);

static const struct file_operations skel_fops = {
	.owner =	THIS_MODULE,
};

/*
 * usb class driver info in order to get a minor number from the usb core,
 * and to have the device registered with devfs and the driver core
 */
/* Get a minor range for your devices from the usb maintainer */
#define USB_SKEL_MINOR_BASE	192

static struct usb_class_driver skel_class = {
	.name = "myUSB%d",
	.fops = &skel_fops,
	.minor_base = USB_SKEL_MINOR_BASE,
};

static int my_probe(struct usb_interface *interface,
			const struct usb_device_id *id)
{
	int rtn = 0;

	pr_debug("<myUSB> %s\n", __func__);
	rtn = usb_register_dev(interface, &skel_class);
	if (rtn) {
		pr_err("Not able to get a minor for thie device");
		goto error;
	}
error:
	return rtn;
}

static void my_disconnect(struct usb_interface *interface)
{
	pr_debug("<myUSB> %s\n", __func__);
}

static struct usb_driver my_usb_driver = {
	.name = "myUsbDriver",
	.id_table = my_usb_table,
	.probe = my_probe,
	.disconnect = my_disconnect,
};

int __init my_module_init(void)
{
	int rtn = 0;

	pr_debug("<myUSB> module init\n");
	rtn = usb_register(&my_usb_driver);
	if (rtn)
		pr_warn("<myUSB> usb_register failed. error number %d", rtn);
	return rtn;
}

void __exit my_module_exit(void)
{
	pr_debug("<myUSB> module exit\n");
	usb_deregister(&my_usb_driver);
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
