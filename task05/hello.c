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

int my_module_init(struct usb_interface *intf, const struct usb_device_id *id)
{
	int rtn = 0;

	printk(KERN_DEBUG "Hello World!\n");
	printk(KERN_DEBUG "detect device insert(%d:%d %d %d).\n",id->idVendor,id->idProduct,id->bcdDevice_hi,id->bcdDevice_lo);
	printk(KERN_DEBUG "device class %d, subclass %d, protocol %d\n",id->bDeviceClass,id->bDeviceSubClass,id->bDeviceProtocol);
	printk(KERN_DEBUG "interface class %d, subclass %d, protocol %d\n",id->bInterfaceClass,id->bInterfaceSubClass,id->bInterfaceProtocol);
	return rtn;
}

void my_module_exit(struct usb_interface *intf)
{
	printk(KERN_DEBUG "module exit\n");
	printk(KERN_DEBUG "detect device disconnect\n");
}

static struct usb_driver my_usb_driver = {
	.name =		"myUsbDriver",
	.probe =	my_module_init,
	.disconnect =	my_module_exit,
	.id_table =	my_usb_table,
};

module_usb_driver(my_usb_driver);
/*
module_init(my_module_init);
module_exit(my_module_exit);
*/
MODULE_LICENSE("GPL");
