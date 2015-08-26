#define pr_fmt(fmt) "<"KBUILD_MODNAME"> %s : " fmt, __func__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/printk.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include <linux/mutex.h>
#include <linux/kfifo.h>

char magic_number[] = "c157e58488d1\n";
struct dentry *my_debugfs_root;
extern u64 jiffies_64;
DEFINE_MUTEX(myLock);
STRUCT_KFIFO_REC_1(PAGE_SIZE) myData;
/*
 * On success, the number of bytes read is returned
 * (zero indicates end  of  file),
 * and the file position is advanced by this number.
 */
ssize_t my_id_read(struct file *file, char __user *buf,
	size_t count, loff_t *ppos)
{
	return simple_read_from_buffer(buf, count, ppos, magic_number,
		strlen(magic_number));
}

/* On success, the number of bytes written is returned
 * (zero indicates nothing was  written).
 * On error, -1 is returned, and errno is set appropriately.
 */
ssize_t my_id_write(struct file *file, const char __user *buf,
	size_t size, loff_t *ppos)
{
	int rtn = 0;
	char *data;

	data = kzalloc(size, GFP_KERNEL);
	if (!data) {
		rtn = -ENOMEM;
		goto malloc_err;
	}

	rtn = simple_write_to_buffer(data, size, ppos, buf, size);
	if (rtn < 0)
		goto err;

	/* compare input string to magic_number */
	rtn = strncmp(magic_number, data, strlen(magic_number) - 1);
	if (rtn == 0)
		/* return how much we written */
		rtn = size;
	else
		rtn = -EINVAL;

err:
	kfree(data);
malloc_err:
	return rtn;
}

static const struct file_operations id_fops = {
	.owner = THIS_MODULE,
	.read = my_id_read,
	.write = my_id_write,
};

ssize_t foo_read(struct file *file, char __user *buf,
	size_t count, loff_t *ppos)
{
	int rtn;
	unsigned int copied;

	if (mutex_lock_interruptible(&myLock))
		return -ERESTARTSYS;
	rtn = kfifo_to_user(&myData, buf, count, &copied);
	mutex_unlock(&myLock);
	return rtn ? rtn : copied;
}

ssize_t foo_write(struct file *file, const char __user *buf,
	size_t size, loff_t *ppos)
{
	int rtn;
	unsigned int copied;

	if (mutex_lock_interruptible(&myLock))
		return -ERESTARTSYS;
	rtn = kfifo_from_user(&myData, buf, size, &copied);
	mutex_unlock(&myLock);
	return rtn ? rtn : copied;
}

static const struct file_operations foo_fops = {
	.owner = THIS_MODULE,
	.read = foo_read,
	.write = foo_write,
};

int __init my_module_init(void)
{
	int rtn;
	struct dentry *d_id, *d_jiffies, *d_foo;

	/* create eudyptula direction */
	my_debugfs_root = debugfs_create_dir("eudyptula", NULL);
	if (!my_debugfs_root) {
		rtn = -ENOENT;
		goto fail;
	}

	/* create id file */
	d_id = debugfs_create_file("id", S_IRUGO | S_IWUGO,
			my_debugfs_root, NULL, &id_fops);
	if (!d_id) {
		pr_debug("create file id fail\n");
		rtn = -ENOENT;
		goto fail;
	}

	/* create jiffies */
	d_jiffies = debugfs_create_u64("jiffies", S_IRUGO,
			my_debugfs_root, &jiffies_64);
	if (!d_jiffies) {
		pr_debug("create file jiffies fail\n");
		rtn = -ENOENT;
		goto fail;
	}

	/* create foo */
	d_foo = debugfs_create_file("foo", S_IWUSR | S_IRUGO,
			my_debugfs_root, NULL, &foo_fops);
	if (!d_foo) {
		pr_debug("create file foo fail\n");
		rtn = -ENOENT;
		goto fail;
	}
	INIT_KFIFO(myData);
	/* success */
	rtn = 0;
	goto success;
fail:
	debugfs_remove_recursive(my_debugfs_root);
	my_debugfs_root = NULL;
success:
	return rtn;
}
module_init(my_module_init);

void __exit my_module_exit(void)
{
	pr_debug("module_exit\n");
	debugfs_remove_recursive(my_debugfs_root);
}
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wenhung <fyonyung@gmail.com");

