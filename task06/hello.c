#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

char magic_number[] = "53719\n";

ssize_t misc_char_read(struct file *file, char __user *buf,
	size_t count, loff_t *ppos)
{
	int len;

	pr_debug("<myMISC> %s\n",__func__);
	/* 
	 * we only support reading the whole string at once.
	 */
	len = strlen(magic_number);
	if(count < len)
		return -EINVAL;

	/*
	 * if file position is non-zero, then assume the string has
	 * been read and indicate there is no more data to read.
	 */
	if(*ppos != 0)
		return 0;

	if(copy_to_user(buf, magic_number, len))
		return -EINVAL;
	/* tell the user how much data we wrote. */
	*ppos = len;
	return len;
}

ssize_t misc_char_write(struct file *file, const char __user *buf,
	size_t size, loff_t *ppos)
{
	int rtn = 0;
	char *data;

	pr_debug("<myMISC> %s\n",__func__);
	data = kmalloc(size, GFP_KERNEL);
	if(!data) {
		pr_debug("<myMISC> malloc failure\n");
		goto malloc_err;
	}

	rtn = copy_from_user(data, buf, size);
	pr_debug("<myMISC> data from user %s\n",data);
	kfree(data);
malloc_err:
	return rtn;
}

int misc_char_open(struct inode *inode, struct file *filp)
{
	int rtn = 0;

	pr_debug("<myMISC> %s\n",__func__);
	return rtn;
}

int misc_char_release(struct inode *inode, struct file *filp)
{
	int rtn = 0;

	pr_debug("<myMISC> %s\n",__func__);
	return rtn;
}

static struct file_operations sample_char_fops = {
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
	
	pr_debug("<myMISC> module init\n");
	rtn = misc_register(&misc_dev);
	if (rtn)
		pr_debug("unable to register misc dev\n");
	return rtn;
}

void __exit my_module_exit(void)
{
	pr_debug("<myMISC> module exit\n");
	misc_deregister(&misc_dev);
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wenhung <fyonyung@gmail.com");

