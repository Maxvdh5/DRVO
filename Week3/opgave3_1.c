#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");


struct file_operations fileOps = {
        .read = dev_read,
        .write = dev_write,
        .open = dev_open,
        .release = dev_release,
        };


ssize_t dev_read(struct file *filp, char *buffer, size_t len, loff_t *offset){
    printk(KERN_ALERT "Read %ld?\n",len);
    return 1;
}

ssize_t dev_write(struct file *filp, char *buffer, size_t len, loff_t *offset){
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

static int dev_init(void){
    printk(KERN_ALERT "init device\n");

//    cdev = cdev_alloc();

    return 0;
    }
static void dev_exit(void){
    printk(KERN_ALERT "Goodbye, world\n");
    }
    
    
module_init(dev_init);
module_exit(dev_exit);