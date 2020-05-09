#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h> 	     /* file stuff */
#include <linux/kernel.h>    /* printk() */
#include <linux/errno.h>     /* error codes */
MODULE_LICENSE("Dual BSD/GPL");

int major;
const int minor = 0;
const int amount = 0;
static const char device_name[] = "driver-van-max";


static struct cdev device;
static dev_t dev_num;

int FirstInt;
module_param(FirstInt,int,0660);

static int SecondInt;
static char *SecondIntName;
module_param_named(SecondIntName,SecondInt,int,0660);


ssize_t dev_read(struct file *filp, char *buffer, size_t len, loff_t *offset){
    printk(KERN_ALERT "Read %ld?\n",len);
    return 1;
}

ssize_t dev_write(struct file *filp, const char *buffer, size_t len, loff_t *offset){
    printk(KERN_ALERT "Write %ld?\n",len);
    return 1;
}

int dev_open(struct inode *inode, struct file *filp) {
    printk(KERN_ALERT "Opened device\n");

    return 0;
}

int dev_release(struct inode *inode, struct file *filp) {
    printk(KERN_ALERT "Opened device\n");

    return 0;
}

struct file_operations fileOps = {
        .owner = THIS_MODULE,
        .read = dev_read,
        .write = dev_write,
        .open = dev_open,
        .release = dev_release,
};

static int dev_init(void){
    printk(KERN_ALERT "init device\n");
    printk(KERN_ALERT "Parameter1=%d Parameter2=%d\n", FirstInt, SecondInt);

    dev_num = MKDEV(major, minor);

    if (major = alloc_chrdev_region(&dev_num, minor, amount, device_name ) < 0)
    {
        printk(KERN_ALERT "init failed!\n");
        return -1;
    }
//    if ((cl = class_create(THIS_MODULE, device_name)) == NULL)
//    {
//        printk(KERN_ALERT "cannot create class\n");
//        unregister_chrdev_region(dev_num, 1);
//        return -1;
//    }
    printk(KERN_ALERT "major number: %d!\n", major);
    cdev_init(&device, &fileOps);
    if (cdev_add(&device, dev_num, 1) < 0)
    {
        printk(KERN_ALERT "Error adding chdev");
        unregister_chrdev_region(dev_num, 1);
        return -1;
    }

    return 0;
    }
static void dev_exit(void){
    printk(KERN_ALERT "Goodbye, world\n");
    dev_t dev_num;

    dev_num = MKDEV(major, minor);
    unregister_chrdev_region(dev_num, amount);
    }
    
    
module_init(dev_init);
module_exit(dev_exit);