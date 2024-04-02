/* WRITE PROJECT STUFF LIKE NAMES */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <asm/param.h>
#include <linux/jiffies.h>

#define BUFFER_SIZE 128
#define PROC_NAME "seconds"

/**
 * Function prototypes
 */
static ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);	

/* We needed to change this for it to work:
 * file_operations => proc_ops
 * .read => .proc_read
 */
static const struct proc_ops proc_ops= {
	// .owner = THIS_MODULE,
        .proc_read = proc_read,
};

struct first_jiffies  {
	long long amnt;
} first;

/* This function is called when the module is loaded. */
static int proc_init(void)
{

        // creates the /proc/hello entry
        // the following function call is a wrapper for
        // proc_create_data() passing NULL as the last argument
        proc_create(PROC_NAME, 0, NULL, &proc_ops);
        first.amnt = get_jiffies_64();

        printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

	return 0;
}

/* This function is called when the module is removed. */
static void proc_exit(void) {

        // removes the /proc/hello entry
        remove_proc_entry(PROC_NAME, NULL);
        printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
        int rv = 0;
        char buffer[BUFFER_SIZE];
        static int completed = 0;
        long long seconds = 0;

        if (completed) {
                completed = 0;
                return 0;
        }

        completed = 1;
        seconds = (get_jiffies_64() - first.amnt) / HZ;
	
        rv = sprintf(buffer, "%llu\n", seconds);

        // copies the contents of buffer to userspace usr_buf
        copy_to_user(usr_buf, buffer, rv);

        return rv;
}


/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Don't Forget your Jiffies");
MODULE_AUTHOR("AAA");
