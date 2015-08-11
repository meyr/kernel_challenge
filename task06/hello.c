#define pr_fmt(fmt) "<"KBUILD_MODNAME"> %s : " fmt, __func__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/printk.h>

char magic_number[] = "c157e58488d1\n";

/*
 * On success, the number of bytes read is returned
 * (zero indicates end  of  file),
 * and the file position is advanced by this number.
 */
ssize_t misc_char_read(struct file *file, char __user *buf,
	size_t count, loff_t *ppos)
{
	return simple_read_from_buffer(buf, count, ppos, magic_number,
		strlen(magic_number));
}

/* On success, the number of bytes written is returned
 * (zero indicates nothing was  written).
 * On error, -1 is returned, and errno is set appropriately.
 */
ssize_t misc_char_write(struct file *file, const char __user *buf,
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

static const struct file_operations sample_char_fops = {
	.owner = THIS_MODULE,
	.read = misc_char_read,
	.write = misc_char_write,
};

static struct miscdevice misc_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "eudyptula",
	.fops = &sample_char_fops,
	.mode = S_IRWXUGO,
};

int __init my_module_init(void)
{
	return misc_register(&misc_dev);
}

void __exit my_module_exit(void)
{
	misc_deregister(&misc_dev);
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wenhung <fyonyung@gmail.com");

