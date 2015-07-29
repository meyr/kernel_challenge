#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

char magic_number[] = "c157e58488d1\n";

#define my_debug(fmt, args...) \
	pr_debug("<myMISC> %s "fmt, __func__, ## args)

/*
 * On success, the number of bytes read is returned
 * (zero indicates end  of  file),
 * and the file position is advanced by this number.
 */
ssize_t misc_char_read(struct file *file, char __user *buf,
	size_t count, loff_t *ppos)
{
	int rtn;
	int len;
	my_debug("count %d ,ppos %d\n", (int)count, (int)*ppos);
	/* check ppos input argument*/
	/* ppos must great than 0 */
	if (*ppos < 0) {
		rtn = -EINVAL;
		goto err;
	}

	/* read out the data*/
	len = strlen(magic_number);
	if (*ppos >= len) {
		rtn = 0;
		goto finish;
	}
	/* check count input parameter */
	/* count must great than 0 */
	if (count <= 0) {
		rtn = 0;
		goto err;
	}
	/* read byte must small and equal remain byte */
	if (count >= len - *ppos)
		count = len - *ppos;

	/* Returns number of bytes that __could not be copied.__
	 * On success, this will be zero.
	 */
	rtn = copy_to_user(buf, magic_number + *ppos, count);

	if (rtn == count) {
		/* can't read all "count" byte data */
		rtn = -EFAULT;
		goto err;
	} else
		/* return how much we read */
		rtn = count - rtn;

	/* move ppos position */
	*ppos += rtn;
finish:
err:
	my_debug("return %d\n", rtn);
	return rtn;
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

	my_debug("count %d , ppos %d\n", (int)size, (int)*ppos);
	data = kmalloc(size, GFP_KERNEL);
	if (!data) {
		my_debug("malloc failure\n");
		goto malloc_err;
	}
	memset(data, '\0', size);
	/* check input argument */
	/* we only support write from file start */
	if (*ppos != 0) {
		rtn = -EINVAL;
		goto err;
	}
	/* Returns number of bytes that __could not be copied.__
	 * On success, this will be zero.
	 */
	rtn = copy_from_user(data, buf, size);
	if (rtn != 0) {
		my_debug("copy_from_user fail\n");
		rtn = -EFAULT;
		goto err;
	}

	/* compare input string to magic_number */
	rtn = strncmp(magic_number, data, strlen(magic_number) - 1);
	my_debug("strncmp result %d\n", rtn);
	if (rtn == 0)
		/* return how much we written */
		rtn = size;
	else
		rtn = -EINVAL;

err:
	kfree(data);
malloc_err:
	my_debug("return %d\n", rtn);
	return rtn;
}

int misc_char_open(struct inode *inode, struct file *filp)
{
	int rtn = 0;

	my_debug("\n");
	return rtn;
}

int misc_char_release(struct inode *inode, struct file *filp)
{
	int rtn = 0;

	my_debug("\n");
	return rtn;
}

static const struct file_operations sample_char_fops = {
	.owner = THIS_MODULE,
	.read = misc_char_read,
	.write = misc_char_write,
	.open = misc_char_open,
	.release = misc_char_release,
};

static struct miscdevice misc_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "eudyptula",
	.fops = &sample_char_fops,
	.mode = S_IRWXUGO,
};

int __init my_module_init(void)
{
	int rtn = 0;

	my_debug("module init\n");
	rtn = misc_register(&misc_dev);
	if (rtn)
		my_debug("unable to register misc dev\n");
	return rtn;
}

void __exit my_module_exit(void)
{
	my_debug("module exit\n");
	misc_deregister(&misc_dev);
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wenhung <fyonyung@gmail.com");

