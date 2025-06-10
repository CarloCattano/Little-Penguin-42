#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h> // Required for copy_from_user

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Louis Solofrizzo <louis@ne02ptzero.me>");
MODULE_DESCRIPTION("Useless module");

static ssize_t myfd_read(struct file *fp, char __user *user, size_t size,
			 loff_t *offs);
static ssize_t myfd_write(struct file *fp, const char __user *user, size_t size,
			  loff_t *offs);

static struct file_operations myfd_fops = {
	.owner = THIS_MODULE,
	.read = myfd_read,
	.write = myfd_write,
};

static struct miscdevice myfd_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "reverse",
	.fops = &myfd_fops,
};

static char str[PAGE_SIZE];
static char *tmp;

static int __init myfd_init(void)
{
	int retval = misc_register(&myfd_device);
	return retval == 0 ? 0 : retval;
}

static void __exit myfd_cleanup(void)
{
	misc_deregister(&myfd_device);
	printk(KERN_INFO "Cleaning up module.\n");
}

ssize_t myfd_read(struct file *fp, char __user *user, size_t size, loff_t *offs)
{
	size_t t, i;

	tmp = kmalloc(PAGE_SIZE, GFP_KERNEL);
	// PAGE_SIZE is the size of a memory page in the kernel
	// GFP_KERNEL is a flag that indicates the type of memory allocation

	if (!tmp)
		return -ENOMEM; // Not enough memory

	for (t = strlen(str), i = 0; t > 0; t--, i++)
		tmp[i] = str[t - 1];

	tmp[i] = '\0';

	if (copy_to_user(user, tmp, i)) {
		kfree(tmp);
		return -EFAULT; // Bad address
	}

	kfree(tmp);
	return i;
}

static ssize_t myfd_write(struct file *fp, const char __user *user, size_t size,
			  loff_t *offs)
{
	ssize_t res;

	if (size >= PAGE_SIZE)
		return -EINVAL;

	res = simple_write_to_buffer(str, PAGE_SIZE - 1, offs, user, size);

	if (res < 0)
		return res;

	str[res] = '\0';
	return res;
}

module_init(myfd_init);
module_exit(myfd_cleanup);
