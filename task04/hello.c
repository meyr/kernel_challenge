#include <linux/kernel.h>
#include <linux/module.h>

static int __init my_module_init(void)
{
	int rtn = 0;

	pr_debug("Hello World!\n");
	return rtn;
}

static void __exit my_module_exit(void)
{
	pr_debug("module exit\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
