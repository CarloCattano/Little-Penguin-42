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

static ssize_t misc_write(struct file *filp, const char __user *buf, size_t len,
                          loff_t *f_pos)
{
	char user_msg[16];  // 15 chars + null terminator

	if (len == 0 || len >= sizeof(user_msg))
		return -EINVAL;

	// Copy full length up to buffer size - 1
	size_t copy_len = min(len, sizeof(user_msg) - 1);

	if (copy_from_user(user_msg, buf, copy_len))
		return -EFAULT;

	user_msg[copy_len] = '\0';  // Ensure null termination

	// Remove trailing newline if present
	if (copy_len > 0 && user_msg[copy_len - 1] == '\n') {
		user_msg[--copy_len] = '\0';
	}

	// Ensure exact match in length and content
	if (copy_len != message_length || strcmp(user_msg, message) != 0)
		return -EINVAL;

	printk(KERN_INFO "User wrote the correct login, returning count %zu\n", len);
	return len;
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
