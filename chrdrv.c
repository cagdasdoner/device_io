#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/poll.h>
#include <linux/wait.h>

#define DEVICE_NAME "chrdrv"
#define CLASS_NAME  "char"

static int majorNo;
static char   message[256] = {0}; 
static struct class* charClass  = NULL;
static struct device* charDevice = NULL;
wait_queue_head_t waitqueue;

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static unsigned int device_poll(struct file *filp, poll_table *wait);

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release,
	.poll = device_poll
};

MODULE_LICENSE("GPL");

static int __init chrdrv_init(void)
{
	if(!(majorNo = register_chrdev(0, DEVICE_NAME, &fops))) {
		return majorNo;
	}

	printk(KERN_INFO "chrdrv attach as %d.\n", majorNo);

	/* Attach descriptor under /dev instead of mknod. */
	charClass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(charClass)){
		unregister_chrdev(majorNo, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class.\n");
		return PTR_ERR(charClass);
	}
	charDevice = device_create(charClass, NULL, MKDEV(majorNo, 0), NULL, DEVICE_NAME);
	if (IS_ERR(charDevice)){
		class_destroy(charClass);
		unregister_chrdev(majorNo, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create the device.\n");
		return PTR_ERR(charDevice);
	}
	init_waitqueue_head(&waitqueue);

	return 0;
}

static void __exit chrdrv_exit(void)
{
	device_destroy(charClass, MKDEV(majorNo, 0));
	class_unregister(charClass);
	class_destroy(charClass);
	unregister_chrdev(majorNo, DEVICE_NAME);
	printk(KERN_INFO "chrdrv detach.\n");
}

static int device_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "chrdrv open.\n");

	return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "chrdrv close.\n");
	
	return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t * offset)
{
	int ret = 0;
	printk(KERN_INFO "chrdrv read.\n");
	if(strlen(message)) {
		/* TODO: Compare user space buff size with stored kernels. */
		if (copy_to_user(buffer, message, strlen(message)) != 0) {
			printk(KERN_ALERT "chrdrv failed to copy.\n");
			return -EFAULT;
		}
		ret = strlen(message);
		/* Clear driver's message buffer after a successfull read. */
		memset(message, 0, strlen(message));
	}

	return ret;
}

/* TODO: Re-organize return values of the calls. */
static ssize_t device_write(struct file *filp, const char *buffer, size_t len, loff_t * off)
{
	printk(KERN_INFO "chrdrv write. size : %ld\n", len);
	if (copy_from_user(message, buffer, len) != 0) {
		printk(KERN_ALERT "chrdrv failed to copy.\n");
		return -EFAULT;
	}
	/* Notifying waitqueue upon new data arrival */
	wake_up_interruptible(&waitqueue);

	return len;
}

static unsigned int device_poll(struct file *filp, poll_table *wait)
{
	unsigned int ret = 0;
	printk(KERN_INFO "chrdrv poll\n");
    poll_wait(filp, &waitqueue, wait);
	/* Wait if driver's message buffer is empty. */
    if (strlen(message)) {
		ret |= POLLIN | POLLRDNORM;
	}

	return ret;		
}

module_init(chrdrv_init);
module_exit(chrdrv_exit);

