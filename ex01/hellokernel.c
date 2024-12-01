#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/version.h>

static int __init hellokernel_init(void)
{
        printk(KERN_INFO "Hello, World! \n");
        return 0;
}

static void __exit hellokernel_exit(void)
{
        printk(KERN_INFO "Cleaning up module... \n");
}

module_init(hellokernel_init);
module_exit(hellokernel_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Carlo Cattano");
MODULE_DESCRIPTION("A Hello World module that works across kernel versions");
