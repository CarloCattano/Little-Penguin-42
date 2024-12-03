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

char str[PAGE_SIZE];

static int __init myfd_init(void)
{
	int retval = misc_register(&myfd_device);
	return retval == 0 ? 0 : retval;
}

static void __exit myfd_cleanup(void)
{
	misc_deregister(&myfd_device);
}

ssize_t myfd_read(struct file *fp, char __user *user, size_t size, loff_t *offs)
{
	size_t t, i;
	size_t len = strlen(str);
	for (t = len - 1, i = 0; t < len; t--, i++) {
		user[i] = str[t];
	}
	user[len] = '\0';
	return simple_read_from_buffer(user, size, offs, user, len);
}

ssize_t myfd_write(struct file *fp, const char __user *user, size_t size,
		   loff_t *offs)
{
	ssize_t res =
		simple_write_to_buffer(str, sizeof(str) - 1, offs, user, size);
	if (res < 0)
		return res;
	str[res] = '\0';
	return res;
}

module_init(myfd_init);
module_exit(myfd_cleanup);
