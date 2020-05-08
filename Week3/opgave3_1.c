#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h> 	     /* file stuff */
#include <linux/kernel.h>    /* printk() */
#include <linux/errno.h>     /* error codes */
MODULE_LICENSE("Dual BSD/GPL");

const int major = 500;
const int minor = 0;
const int amount = 0;
static const char device_name[] = "driver-van-max";


static struct cdev* device;


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
        .read = dev_read,
        .write = dev_write,
        .open = dev_open,
        .release = dev_release,
};

static int dev_init(void){
    printk(KERN_ALERT "init device\n");

    int result = 0;
    dev_t dev_num;

    dev_num = MKDEV(major, minor);


    result = register_chrdev_region( dev_num, amount, device_name );

    if (result != 0)
    {
        printk(KERN_ALERT "init failed!\n");
    }

    device = cdev_alloc();

    cdev_init(&device->cdev, &fileOps);
    device->ops = &fileOps;

    int err = cdev_add(&device, dev_num, 1){
        if(err){
            printK(KERN_ALERT "Error %d adding chdev", err);
        }
    }

    cdev_add


    return result;
    }
static void dev_exit(void){
    printk(KERN_ALERT "Goodbye, world\n");
    dev_t dev_num;

    dev_num = MKDEV(major, minor);
    unregister_chrdev_region(dev_num, amount);
    }
    
    
module_init(dev_init);
module_exit(dev_exit);