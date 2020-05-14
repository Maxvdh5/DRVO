#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h> 	     /* file stuff */
#include <linux/kernel.h>    /* printk() */
#include <linux/errno.h>     /* error codes */
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <linux/io.h>
#include <linux/irqdomain.h>
#include <linux/interrupt.h>
#include <linux/of.h>
#include <linux/of_address.h>

MODULE_LICENSE("Dual BSD/GPL");

#define IRQLINE 1

int major;
const int minor = 0;
const int amount = 0;
static const char device_name[] = "driver-van-max";
static struct class *cl;


static struct cdev device;
static dev_t dev_num;

int FirstInt;
module_param(FirstInt,int,0660);

static int SecondInt;
static char *SecondIntName;
module_param_named(SecondIntName,SecondInt,int,0660);

// memory buffer stuff
char* char_buff;
static const int MAX_SIZE = 256;
int buff_size = 0;


// interrupt stuff
static devfs_handle_t handle;


ssize_t dev_read(struct file *filp, char *buffer, size_t len, loff_t *ppos){
    ssize_t bytes;

//    max bytes never < buff_size
    if (buff_size < len){
        bytes = buff_size;
    } else {
        bytes = len;
    }

//    check if there is data available
    if(bytes == 0) {
        return 0;
    }

//    copy requested bytes to user
    int result;
    result = copy_to_user(buffer, char_buff+*ppos,bytes);

    if(result < 0){
        printk(KERN_ALERT "could not copy bytes to user\n");
        return -1;
    } else {
        printk("copy succeeded! copied %d bytes\n", bytes);
        buff_size -= bytes;
        return bytes;
    }
}



ssize_t dev_write(struct file *filp, const char *buffer, size_t len, loff_t *ppos){

//  reset the memory buffers
    memset(char_buff, 0, MAX_SIZE);

    printk("bytes received:%d\n",len);

    if(len > MAX_SIZE){
        printk("Message was to long\n");
        len = MAX_SIZE-1;
    }

    int result;
    result = copy_from_user(char_buff,buffer,len);

    if (result < 0){
        printk(KERN_ALERT "error while reading from user space\n");
    }

    printk("content written to buffer[%s]\n", char_buff);

    buff_size = len;

    return len;
}

int dev_open(struct inode *inode, struct file *filp) {
    printk(KERN_ALERT "Opened device\n");

    return 0;
}

int dev_release(struct inode *inode, struct file *filp) {
    printk(KERN_ALERT "Opened device\n");

    return 0;
}


// this function is used in user space to change the file pointer ppos
//
static loff_t dev_lseek(struct file *filp, loff_t offset, int orig){
    loff_t new_pos = 0;

    printk(KERN_INFO "using lseek to change ppos\n");
    switch (orig) {
        case 0 :        // SEEK_SET
            new_pos = offset;
            break;
        case 1 :        // SEEK_CUR
            new_pos = filp->f_pos + offset;
            break;
        case 2 :        // SEEK_END
            new_pos = buff_size - offset;
            break;
    }
    if (new_pos > buff_size)
        new_pos = buff_size;
    if (new_pos < 0)
        new_pos = 0;
    filp->f_pos = new_pos;
    printk(KERN_INFO "the new pos: %d\n", filp->f_pos);
    return new_pos;
}

long dev_ioctl (struct file *filp, unsigned int cmd, unsigned long arg) {
    printk(KERN_INFO "using \n");
    switch (cmd) {
        case HELLO_WORLD :
            printk(KERN_INFO "Hello World from ioctl!\n");
            break;
    }
    return 0;
}

int interrupt_count = 0;
static void irq_handler(int irq, void* dev_id, struct pt_regs *regs)
{
    interrupt_count++;
    printk(KERN_INFO "Interrupt %d\n", interrupt_count);
}

struct file_operations fileOps = {
        .owner = THIS_MODULE,
        .read = dev_read,
        .write = dev_write,
        .open = dev_open,
        .release = dev_release,
        .llseek = dev_lseek,
        .unlocked_ioctl = dev_ioctl,
};

static void dev_exit(void){
    printk(KERN_ALERT "Goodbye, world\n");
    cdev_del(&device);
    device_destroy(cl, dev_num);
    class_destroy(cl);
    unregister_chrdev_region(dev_num, amount);
}

static int dev_init(void){
    printk(KERN_ALERT "init device\n");
    printk(KERN_ALERT "Parameter1=%d Parameter2=%d\n", FirstInt, SecondInt);

//    allocate room for the driver
    if (alloc_chrdev_region(&dev_num, minor, amount, device_name ) < 0)
    {
        printk(KERN_ALERT "init failed!\n");
        return -1;
    }

//    create a class for the device driver
    if ((cl = class_create(THIS_MODULE, device_name)) == NULL)
    {
        printk(KERN_ALERT "cannot create class\n");
        unregister_chrdev_region(dev_num, 1);
        return -1;
    }
//     create a driver device
    if (device_create(cl, NULL, dev_num, NULL, "Character-driver-device") == NULL)
    {
        printk(KERN_ALERT "cannot create device\n");
        class_destroy(cl);
        unregister_chrdev_region(dev_num, 1);
        return -1;
    }

    major = MAJOR(dev_num);
    printk(KERN_ALERT "major number: %d!\n", major);

//    init and add character device
    cdev_init(&device, &fileOps);
    if (cdev_add(&device, dev_num, 1) < 0)
    {
        printk(KERN_ALERT "Error adding chdev");
        dev_exit(); // goto dev_exit to cleanup all the created memory
        return -1;
    }

//    hook interrupt to function

    if (request_irq(IRQLINE, kbd_irq_handler, IRQ_SHARED,
                    device_name, (void*) &handle) < 0) {
        printk(KERN_CRIT "%s: interrupt line busy\n",device_name);
        dev_exit(); // goto dev_exit to cleanup all the created memory
        return -1
    }

//    allocate memory for buffers
    char_buff = kmalloc(MAX_SIZE, GFP_KERNEL); // GFP_KERNEL --> Allocate normal kernel ram. May sleep.

//    check if allocation was successful
    if(!char_buff){
        printk(KERN_ALERT "could not allocate room for memory buffers");
        dev_exit(); // goto dev_exit to cleanup all the created memory
        return -1;
    }

//    reset the buffers
    memset(char_buff, 0, MAX_SIZE);

    return 0;
}

module_init(dev_init);
module_exit(dev_exit);
