#include <linux/dcache.h>
#include <linux/fs.h>
#include <linux/fs_struct.h>
#include <linux/module.h>
#include <linux/mount.h>
#include <linux/namei.h>
#include <linux/nsproxy.h>
#include <linux/path.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Carlo Cattano");
MODULE_DESCRIPTION("/proc/mymounts: list mount points");

static int show_mount_info(struct vfsmount *mnt, void *arg) {
  struct seq_file *m = arg;
  struct path p;
  char *buf, *path;

  buf = (char *)__get_free_page(GFP_KERNEL);
  if (!buf)
    return -ENOMEM;

  p.mnt = mnt;
  p.dentry = mnt->mnt_root;

  path = d_path(&p, buf, PAGE_SIZE);
  if (!IS_ERR(path)) {
    seq_printf(m, "%s\n", path);
  }

  free_page((unsigned long)buf);
  return 0;
}

static int mymounts_show(struct seq_file *m, void *v) {
  struct path root_path;
  get_fs_root(current->fs, &root_path); // upadate +6.x

  iterate_mounts(show_mount_info, m, root_path.mnt);
  return 0;
}

static int mymounts_open(struct inode *inode, struct file *file) {
  return single_open(file, mymounts_show, NULL);
}

static const struct proc_ops mymounts_fops = {
    .proc_open = mymounts_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init mymounts_init(void) {
  proc_create("mymounts", 0, NULL, &mymounts_fops);
  pr_info("mymounts loaded\n");
  return 0;
}

static void __exit mymounts_exit(void) {
  remove_proc_entry("mymounts", NULL);
  pr_info("mymounts unloaded\n");
}

module_init(mymounts_init);
module_exit(mymounts_exit);
