#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>
#include <linux/slab.h>
#include <linux/mutex.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Carlo Cattano");
MODULE_DESCRIPTION("debugfs module with id, jiffies, and foo");

#define BUF_SIZE PAGE_SIZE

const char login[] = "ccattano";
static const int login_len = sizeof(login) - 1;

// Foo buffer and locking
static char foo_buf[BUF_SIZE];
static size_t foo_len = 0;
static DEFINE_MUTEX(foo_mutex);

// Debugfs directory and file entries
static struct dentry *debugfs_dir;
static struct dentry *id_file, *jiffies_file, *foo_file;

// ===== ID FILE (read/write by anyone, validation as before) =====

static ssize_t id_read(struct file *f, char __user *buf, size_t len, loff_t *off) {
    return simple_read_from_buffer(buf, len, off, login, login_len);
}

//static const size_t login_len = sizeof(login) - 1; 

static ssize_t id_write(struct file *f, const char __user *buf, size_t len, loff_t *off) {
	char user_msg[16];
	
	if (len >= sizeof(user_msg))
		return -EINVAL;
	
	if (copy_from_user(user_msg, buf, len))
		return -EFAULT;
	
	user_msg[len] = '\0';
	
	if (len != login_len)
		return -EINVAL;
	
	if (strncmp(user_msg, login, login_len) != 0)
		return -EINVAL; 
	
	pr_info("User wrote the correct login, returning count %zu\n", len);
	return len;
}
						   

static const struct file_operations id_fops = {
    .owner = THIS_MODULE,
    .read = id_read,
    .write = id_write,
};

// ===== JIFFIES FILE (read-only) =====

static ssize_t jiffies_read(struct file *f, char __user *buf, size_t len, loff_t *off) {
    char tmp[32];
    int written = snprintf(tmp, sizeof(tmp), "%lu\n", jiffies);
    return simple_read_from_buffer(buf, len, off, tmp, written);
}

static const struct file_operations jiffies_fops = {
    .owner = THIS_MODULE,
    .read = jiffies_read,
};

// ===== FOO FILE (root-write, all-read) =====

static ssize_t foo_read(struct file *f, char __user *buf, size_t len, loff_t *off) {
    ssize_t ret;

    if (mutex_lock_interruptible(&foo_mutex))
        return -ERESTARTSYS;

    ret = simple_read_from_buffer(buf, len, off, foo_buf, foo_len);

    mutex_unlock(&foo_mutex);
    return ret;
}

static ssize_t foo_write(struct file *f, const char __user *buf, size_t len, loff_t *off) {
    if (!capable(CAP_SYS_ADMIN)) return -EACCES;
    if (len > BUF_SIZE) return -EINVAL;

    if (mutex_lock_interruptible(&foo_mutex)) return -ERESTARTSYS;

    foo_len = len;
    if (copy_from_user(foo_buf, buf, len)) {
        mutex_unlock(&foo_mutex);
        return -EFAULT;
    }

    mutex_unlock(&foo_mutex);
    return len;
}

static const struct file_operations foo_fops = {
    .owner = THIS_MODULE,
    .read = foo_read,
    .write = foo_write,
};

// ===== MODULE INIT & EXIT =====

static int __init fortytwo_init(void) {
    debugfs_dir = debugfs_create_dir("fortytwo", NULL);
    if (!debugfs_dir) return -ENOMEM;

    id_file = debugfs_create_file("id", 0666, debugfs_dir, NULL, &id_fops);
    jiffies_file = debugfs_create_file("jiffies", 0444, debugfs_dir, NULL, &jiffies_fops);
    foo_file = debugfs_create_file("foo", 0644, debugfs_dir, NULL, &foo_fops);

    if (!id_file || !jiffies_file || !foo_file) {
        debugfs_remove_recursive(debugfs_dir);
        return -ENOMEM;
    }

    pr_info("fortytwo debugfs module loaded.\n");
    return 0;
}

static void __exit fortytwo_exit(void) {
    debugfs_remove_recursive(debugfs_dir);
    pr_info("fortytwo debugfs module unloaded.\n");
}

module_init(fortytwo_init);
module_exit(fortytwo_exit);

