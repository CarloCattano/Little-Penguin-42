#include <linux/module.h>
#include <linux/usb.h>
#include <linux/hid.h>

const struct usb_device_id usb_drv_id_table[] = {
	{ USB_INTERFACE_INFO(
		USB_INTERFACE_CLASS_HID, USB_INTERFACE_SUBCLASS_BOOT,
		USB_INTERFACE_PROTOCOL_KEYBOARD) }, // Matches any USB keyboard
	{},
};

int usb_drv_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	printk(KERN_INFO
	       "MYUSB DRIVER: USB Detector: Probing device (Vendor: %04x, Product: %04x)\n",
	       id->idVendor, id->idProduct);
	return 0;
}

void usb_drv_disconnect(struct usb_interface *intf)
{
	printk(KERN_INFO "MYUSB DRIVER: Disconnecting device\n");
}

struct usb_driver usb_drv = { .name = "Test USB driver",
			      .probe = usb_drv_probe,
			      .disconnect = usb_drv_disconnect,
			      .id_table = usb_drv_id_table };

int __init usb_dry_init(void)
{
	printk(KERN_INFO "MYUSB DRIVER: USB detector initializing\n");
	usb_register(&usb_drv);
	return 0;
}

void __exit usb_dry_exit(void)
{
	printk(KERN_INFO "MYUSB DRIVER: USB detector exiting\n");
	usb_deregister(&usb_drv);
}

module_init(usb_dry_init);
module_exit(usb_dry_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Carlo Cattano");
MODULE_DESCRIPTION("A USB keyboard detection module that prints messages when "
		   "connected or disconnected.");
