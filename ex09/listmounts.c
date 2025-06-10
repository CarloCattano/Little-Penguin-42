#include <linux/fs.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>

#define PROC_NAME "mymounts"
#define MOUNTS_PATH "/proc/self/mounts"

static struct proc_dir_entry *proc_entry;

/**
 * mymounts_show - display mount points in /proc/mymounts
 * @v: unused (required by seq_file API)
 */
static int mymounts_show(struct seq_file *m, void *v)
{
	struct file *file;
	char *buf;
	char *line;
	char *dev_name;
	char *mount_point;
	loff_t pos = 0;
	ssize_t read_bytes;

	file = filp_open(MOUNTS_PATH, O_RDONLY, 0);
	if (IS_ERR(file)) {
		pr_err("Failed to open %s: %ld\n", MOUNTS_PATH, PTR_ERR(file));
		return PTR_ERR(file);
	}

	// Allocate buffer for reading
	buf = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (!buf) {
		filp_close(file, NULL);
		return -ENOMEM;
	}

	// Read and process the file
	while ((read_bytes = kernel_read(file, buf, PAGE_SIZE - 1, &pos)) > 0) {
		buf[read_bytes] = '\0'; // Ensure null termination
		line = buf;
		while (line) {
			char *next_line = strchr(line, '\n');
			if (next_line) {
				*next_line = '\0'; // Terminate current line
				next_line++; // Move to next line
			}

			// Parse : device mountpoint ...
			dev_name = strsep(&line, " ");
			mount_point = strsep(&line, " ");
			if (dev_name && mount_point) {
				seq_printf(m, "%s %s\n", dev_name, mount_point);
			}

			line = next_line; // Continue next line
		}
	}

	// Cleanup
	kfree(buf);
	filp_close(file, NULL);
	return 0;
}

/**
 * mymounts_open - Open callback for /proc/mymounts
 */
static int mymounts_open(struct inode *inode, struct file *file)
{
	return single_open(file, mymounts_show, NULL);
}

static const struct proc_ops mymounts_proc_ops = {
	.proc_open = mymounts_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};

/**
 * mymounts_init - Module initialization
 */
static int __init mymounts_init(void)
{
	proc_entry = proc_create(PROC_NAME, 0444, NULL, &mymounts_proc_ops);
	if (!proc_entry) {
		pr_err("Failed to create /proc/%s\n", PROC_NAME);
		return -ENOMEM;
	}
	pr_info("/proc/%s created\n", PROC_NAME);
	return 0;
}

/**
 * mymounts_exit - Module cleanup
 */
static void __exit mymounts_exit(void)
{
	if (proc_entry)
		proc_remove(proc_entry);
	pr_info("/proc/%s removed\n", PROC_NAME);
}

module_init(mymounts_init);
module_exit(mymounts_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Carlo Cattano");
MODULE_DESCRIPTION("Lists mount points in /proc/mymounts");
