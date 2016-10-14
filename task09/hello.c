#define pr_fmt(fmt) "<"KBUILD_MODNAME"> %s : " fmt, __func__

#include <linux/kobject.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/printk.h>
#include <linux/jiffies.h>
#include <linux/mutex.h>
#include <linux/kfifo.h>

char magic_number[] = "c157e58488d1";
extern u64 jiffies_64;
DEFINE_MUTEX(myLock);
STRUCT_KFIFO_REC_1(PAGE_SIZE) myData;

static ssize_t id_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	return sprintf(buf, "%s\n", magic_number);
}

static ssize_t id_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t size)
{
	int rtn = 0;

	/* compare input string to magic_number */
	rtn = strncmp(magic_number, buf, strlen(magic_number) - 1);
	if (rtn == 0)
		/* return how much we written */
		rtn = size;
	else
		rtn = -EINVAL;

	return rtn;

}

static ssize_t jiffies_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	return sprintf(buf, "%llu\n", jiffies_64);
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t size)
{
	int rtn;

	if (mutex_lock_interruptible(&myLock))
		return -ERESTARTSYS;
	rtn = kfifo_in(&myData, buf, size);
	mutex_unlock(&myLock);

	return rtn;

}

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	int rtn;

	if (mutex_lock_interruptible(&myLock))
		return -ERESTARTSYS;
	rtn = kfifo_out(&myData, buf, kfifo_len(&myData));
	mutex_unlock(&myLock);
	return rtn;

}

static struct kobj_attribute id_attribute = __ATTR_RW(id);
static struct kobj_attribute jiffies_attribute = __ATTR_RO(jiffies);
static struct kobj_attribute foo_attribute = __ATTR_RW(foo);

static struct attribute *attrs[] = {
	&id_attribute.attr,
	&jiffies_attribute.attr,
	&foo_attribute.attr,
	NULL,
};
static struct attribute_group attr_group = {
	.attrs = attrs,
};
static struct kobject *kobj;

int __init my_module_init(void)
{
	int rtn;

	/* create eudyptula direction */
	kobj = kobject_create_and_add("eudyptula", kernel_kobj);
	if (!kobj)
		return -ENOMEM;

	/* create files */
	rtn = sysfs_create_group(kobj, &attr_group);
	if (rtn)
		kobject_put(kobj);
	
	INIT_KFIFO(myData);
	/* success */
	rtn = 0;
	return rtn;
}
module_init(my_module_init);

void __exit my_module_exit(void)
{
	kobject_put(kobj);
}
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wenhungyang <fyonyung@gmail.com>");

