#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/sysfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kshitij mistry");
MODULE_DESCRIPTION("A Simple Linux Kernel Module with SysFS Support");

static struct kobject *my_kobj;
static int             my_value = 42;

// Function to read the sysfs attribute
static ssize_t my_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", my_value);
}

// Function to write to the sysfs attribute
static ssize_t my_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    sscanf(buf, "%d", &my_value);
    return count;
}

static struct kobj_attribute my_attribute = __ATTR(my_value, 0660, my_show, my_store);

static int __init mymodule_init(void)
{
    printk(KERN_INFO "My Module Loaded!\n");

    // Create sysfs entry under /sys/kernel/
    my_kobj = kobject_create_and_add("my_kobject", kernel_kobj);
    if (!my_kobj)
    {
        return -ENOMEM;
    }

    if (sysfs_create_file(my_kobj, &my_attribute.attr))
    {
        printk(KERN_ERR "Failed to create sysfs file\n");
        kobject_put(my_kobj);
        return -ENOMEM;
    }
    printk(KERN_INFO "Sysfs entry created: /sys/kernel/my_kobject/my_value\n");
    return 0;
}

static void __exit mymodule_exit(void)
{
    sysfs_remove_file(my_kobj, &my_attribute.attr);
    kobject_put(my_kobj);
    printk(KERN_INFO "Sysfs entry removed\n");
    printk(KERN_INFO "My Module Unloaded!\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);
