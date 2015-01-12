#include <linux/kernel.h>
#include <linux/module.h>

#define LOGLEVEL KERN_DEBUG

int __init my_module_init(void)
{
	int rtn = 0;
	printk(LOGLEVEL "module init\n");
	return rtn;
}

void __exit my_module_exit(void)
{
	printk(LOGLEVEL "module exit\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
