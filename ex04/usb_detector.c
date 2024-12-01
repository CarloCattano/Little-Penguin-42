#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb.h>

int __init usb_dry_init(void)
{
	printk(KERN_INFO "USB detector initializing\n");
	return 0;
}

void __exit usb_dry_exit(void)
{
	printk(KERN_INFO "USB detector exiting\n");
}

module_init(usb_dry_init);
module_exit(usb_dry_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Carlo Cattano");
MODULE_DESCRIPTION("A USB keyboard detection module that prints messages when "
		   "connected or disconnected.");
