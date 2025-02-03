#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kshitij mistry");
MODULE_DESCRIPTION("A Simple Linux Kernel Module");

static int __init mymodule_init(void)
{
    printk(KERN_INFO "My Module Loaded!\n");
    return 0;
}

static void __exit mymodule_exit(void)
{
    printk(KERN_INFO "My Module Unloaded!\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);
