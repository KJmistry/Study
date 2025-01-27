#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define PROC_FILENAME "myprocfile"
#define BUFFER_SIZE   128

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kshitij mistry");
MODULE_DESCRIPTION("A Simple Linux Kernel Module with Proc FS");

static struct proc_dir_entry *proc_file;
static char                   proc_data[BUFFER_SIZE] = "Hello from Proc FS!\n";

// Function to read from the proc file
static ssize_t proc_read(struct file *file, char __user *user_buf, size_t count, loff_t *pos)
{
    printk(KERN_INFO "Reading from myprocfile!\n");
    return simple_read_from_buffer(user_buf, count, pos, proc_data, strlen(proc_data));
}

// Function to write to the proc file
static ssize_t proc_write(struct file *file, const char __user *user_buf, size_t count, loff_t *pos)
{
    if (count > BUFFER_SIZE - 1)
    {
        return -EINVAL;
    }

    if (copy_from_user(proc_data, user_buf, count))
    {
        return -EFAULT;
    }

    printk(KERN_INFO "Writing to myprocfile!\n");

    proc_data[count] = '\0';  // Null-terminate the string
    return count;
}

// File operations for the proc file
static const struct proc_ops proc_fops = {
    .proc_read = proc_read,
    .proc_write = proc_write,
};

static int __init mymodule_init(void)
{
    printk(KERN_INFO "My Module Loaded!\n");

    // Create /proc/myprocfile
    proc_file = proc_create(PROC_FILENAME, 0666, NULL, &proc_fops);
    if (!proc_file)
    {
        printk(KERN_ERR "Failed to create /proc/%s\n", PROC_FILENAME);
        return -ENOMEM;
    }
    printk(KERN_INFO "/proc/%s created successfully\n", PROC_FILENAME);
    return 0;
}

static void __exit mymodule_exit(void)
{
    remove_proc_entry(PROC_FILENAME, NULL);
    printk(KERN_INFO "/proc/%s removed\n", PROC_FILENAME);
    printk(KERN_INFO "My Module Unloaded!\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);