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

int __init my_module_init(void)
{
	int rtn = 0;

	pr_debug("<myUSB> module init\n");
	return rtn;
}

void __exit my_module_exit(void)
{
	pr_debug("<myUSB> module exit\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
