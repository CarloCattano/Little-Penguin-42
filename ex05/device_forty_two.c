#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Carlo Cattano");
MODULE_DESCRIPTION("Misc char device example ex05");

static char *message = "ccattano";
static int message_length = sizeof(message);

static ssize_t misc_read(struct file *filp, char __user *buf, size_t count,
			 loff_t *f_pos)
{
	return simple_read_from_buffer(buf, count, f_pos, message,
				       message_length);
}

static ssize_t misc_write(struct file *filp, const char __user *buf,
			  size_t count, loff_t *f_pos)
{
	char user_msg[16];

	if (count >= sizeof(user_msg) || count == 0)
		return -EINVAL;

	size_t copy_len = min(count, message_length);
	if (copy_from_user(user_msg, buf, count))
		return -EFAULT;

	user_msg[copy_len] = '\0';

	if (copy_len > 0 && user_msg[copy_len - 1] == '\n') {
		user_msg[--copy_len] = '\0';
	}

	if (strlen(user_msg) != message_length)
		return -EINVAL;

	if (strncmp(user_msg, "ccattano", message_length + 1) != 0)
		return -EINVAL; // Invalid input

	printk(KERN_INFO "User wrote the correct login, returning count %zu\n",
	       count);
	return count;
}

const struct file_operations misc_fops = {
	.owner = THIS_MODULE,
	.read = misc_read,
	.write = misc_write,
};

static struct miscdevice misc_example = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "fortytwo",
	.fops = &misc_fops,
};

static int __init misc_dev_init(void)
{
	printk("Misc device initialized Minor number: %i\n",
	       misc_example.minor);
	return misc_register(&misc_example);
}

static void __exit misc_dev_exit(void)
{
	printk(KERN_INFO "Cleaning up module...\n");
	misc_deregister(&misc_example);
}

module_init(misc_dev_init);
module_exit(misc_dev_exit);
