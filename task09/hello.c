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

static ssize_t jiffies_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	return sprintf(buf, "%llu\n", jiffies_64);
}

static struct kobj_attribute id_attribute =
	__ATTR(id, 0666, id_show, NULL);
static struct kobj_attribute jiffies_attribute =
	__ATTR(jiffies, 0444, jiffies_show, NULL);
//static struct kobj_attribute foo_attribute =
//	__ATTR(bar, 0644, foo_show, foo_store);

static struct attribute *attrs[] = {
	&id_attribute.attr,
	&jiffies_attribute.attr,
//	&foo_attribute.attr,
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
MODULE_AUTHOR("wenhung <fyonyung@gmail.com>");

