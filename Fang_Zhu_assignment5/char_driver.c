
#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>    /* Needed for the macros */
#include <linux/cdev.h>    /* Needed for cdev struct */
#include <linux/fs.h>     /* Needed for file_operations */
#include <linux/slab.h>    /* Needed for kmalloc, kzalloc etc. */
#include <linux/errno.h>   /* Needed for error checking */
#include <linux/semaphore.h>	/* Sync primitives */
#include <linux/device.h>	/* device class */
#include <linux/uaccess.h>	/* copy_*_user */

#define DRIVER_NAME "asp"
#define BUFFER_SIZE 100
#define DEFAULT_MAJOR 500 
#define DEFAULT_MINOR 0
#define DEFAULT_NUM_DEVICES  3
#define DEFAULT_RAMDISK_SIZE 16*PAGE_SIZE

static int _major = DEFAULT_MAJOR;
static int _minor = DEFAULT_MINOR;
static int _num_devs = DEFAULT_NUM_DEVICES;
static long _ramdisk_size = DEFAULT_RAMDISK_SIZE;

#define  MODULE_NAME     "asp_mycdev"
#define  MODULE_CLASS_NAME  "mycdrv_class"
#define  MODULE_NODE_NAME   "mycdrv"
#define  MAX_NODE_NAME_SIZE  10
/*Define module name*/

module_param(_major, int, S_IRUGO);
module_param(_minor, int, S_IRUGO);
module_param(_num_devs, int, S_IRUGO);
module_param(_ramdisk_size, long, S_IRUGO);
/*Load module parameters*/

static struct class *asp_mycdev_class = NULL;
static struct asp_mycdev *mycdev_devices = NULL;
static int lastSuccessfulRamdisk = -1;
static int lastSuccessfulCdev = -1;
static int lastSuccessfulNode = -1;
/* Other global variables */

#define CDRV_IOC_MAGIC  0x37
/*IOCTLs: define magic address*/

#define ASP_CLEAR_BUF  _IO(CDRV_IOC_MAGIC, 0)
/* clear the ramdisk and sets the file position at the beginning */

#define ASP_IOCTL_MAXNR  0
/* Maximum number of IOCTL defs implemented in this driver */

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Fang Zhu"); 
MODULE_DESCRIPTION("Assignment 5");
MODULE_VERSION("0.1"); 
/*Module infomation*/

struct asp_mycdev 
{
	struct cdev cdev;
	char *ramdisk;			  /* device */
	struct device *ptr;/* device node in sysfs */
	unsigned int access_key;  /* used by sculluid and scullpriv */
	struct semaphore sem;     /* mutual exclusion semaphore     */
	int devNo;				  /* device number */
	size_t size; 	          /* device size */
};
/*Define a device structure*/

static int mycdev_init_module(void);
static void mycdev_cleanup_module(void);
static int asp_mycdev_open(struct inode *, struct file *);
static int asp_mycdev_release(struct inode *, struct file *);
static ssize_t asp_mycdev_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t asp_mycdev_write(struct file *, const char __user *, size_t, loff_t *);
static loff_t asp_mycdev_lseek(struct file *, loff_t, int);
static long asp_mycdev_ioctl(struct file *, unsigned int, unsigned long);
/* Function declarations */

static struct file_operations asp_mycdev_fops =
{	
   .owner = THIS_MODULE,
   .open = asp_mycdev_open,
   .read = asp_mycdev_read,
   .write = asp_mycdev_write,
   .release = asp_mycdev_release,
   .llseek = asp_mycdev_lseek,
   .unlocked_ioctl  = asp_mycdev_ioctl,
};
/*Define fileops*/


static int asp_mycdev_open(struct inode *i_ptr, struct file *filp)
{
	struct asp_mycdev *dev; /* device information */

	dev = container_of(i_ptr->i_cdev, struct asp_mycdev, cdev);
	filp->private_data = dev; /* for other methods */
	printk(KERN_INFO "%s: device %s%d opened [Major: %d, Minor: %d]\n",MODULE_NAME, "/dev/"MODULE_NODE_NAME, dev->devNo, imajor(i_ptr), iminor(i_ptr));
	return 0;          /* success */
}
/*asp_mycdev_open*/

static int asp_mycdev_release(struct inode *i_ptr, struct file *filp)
{
	struct asp_mycdev *dev = filp->private_data;
	printk(KERN_INFO "%s: device %s%d closed\n",MODULE_NAME, "/dev/"MODULE_NODE_NAME, dev->devNo);
	return 0;
}
/*asp_mycdev_release*/

static ssize_t asp_mycdev_read(struct file *filp, char __user *buf, size_t count,loff_t *f_offset)
{
	struct asp_mycdev *dev = filp->private_data; 
	ssize_t retval = 0;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;
	if (*f_offset >= dev->size)
		goto out;
	if (*f_offset + count > dev->size)
	{
		count = dev->size - *f_offset;
		printk(KERN_WARNING "%s: device %s%d: Attempt to READ beyond the device size!\n", MODULE_NAME, "/dev/"MODULE_NODE_NAME, dev->devNo);
		count = dev->size - *f_offset;
	}

	retval = count - copy_to_user(buf, (dev->ramdisk + *f_offset), count);
	*f_offset += retval;

out:
	up(&dev->sem);
	return retval;
}
/*asp_mycdev_read*/

static ssize_t asp_mycdev_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_offset)
{
	struct asp_mycdev *dev = filp->private_data;
	ssize_t retval = -ENOMEM; /* value used in "goto out" statements */

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;
	if((count + *f_offset) > dev->size) { /* write beyond our device size */
		printk(KERN_WARNING "%s: device %s%d: Attempt to WRITE beyond the device size! Returning!\n",MODULE_NAME, "/dev/"MODULE_NODE_NAME, dev->devNo);
		goto out;
	}

	/* copy to user and update the offset in the device */
	retval = count - copy_from_user((dev->ramdisk + *f_offset), buf, count);
	*f_offset += retval;

	printk(KERN_DEBUG "%s: device %s%d: bytes written: %d, current position: %d\n", MODULE_NAME, "/dev/"MODULE_NODE_NAME, dev->devNo, (int)retval, (int)*f_offset);
out:
	up(&dev->sem);
	return retval;
}
/*asp_mycdev_write*/

loff_t asp_mycdev_lseek(struct file *filp, loff_t f_offset, int action)
{
	loff_t new_offset;
	struct asp_mycdev *dev = filp->private_data;

	/* ENTER Critical Section */
	if(down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	switch (action)
	{
		case SEEK_SET:
			new_offset = f_offset;
			break;

		case SEEK_CUR:
			new_offset = filp->f_pos + f_offset;
			break;

		case SEEK_END:
			new_offset = dev->size + f_offset;
			break;

		default:
			new_offset = -EINVAL;
			goto out;
	}
	/* validity checks (lower boundary) */
	new_offset = (new_offset < 0)? 0: new_offset;

	printk(KERN_DEBUG "%s: device %s%d: Current offset: %ld, Requested offset: %ld\n", MODULE_NAME, "/dev/"MODULE_NODE_NAME, dev->devNo, (long) filp->f_pos, (long) new_offset);

	/* if the new_offset is beyond the current size of ramdisk,
	reallocate ramdisk to hold double the current size
	and fill the remaining region with all zeros */
	if(new_offset > dev->size)
	{
		char *new_ramdisk = NULL;
		int pages = -1;
		size_t old_ramdiskSize = -1;
		size_t new_ramdiskSize = -1;

		/* find the new ramdisk size which is multiple of PAGE_SIZE */
		pages = new_offset / PAGE_SIZE;		// Assert (pages >= 1)
		pages = (new_offset % PAGE_SIZE > 0)? pages+1 : pages;
		new_ramdiskSize = pages * PAGE_SIZE;

		/* reallocate ramdisk */
		new_ramdisk = krealloc(dev->ramdisk, new_ramdiskSize, GFP_KERNEL);
		if(new_ramdisk != NULL)
		{
			/* save old ramdiskSize, we will need it to update the expanded memory */
			old_ramdiskSize = dev->size;
			/* realloc succeeded, zero out the extra memory */
			dev->ramdisk = new_ramdisk;
			dev->size = new_ramdiskSize;
			memset(dev->ramdisk + old_ramdiskSize, 0, new_ramdiskSize - old_ramdiskSize);

			printk(KERN_DEBUG "%s: device %s%d: Ramdisk resized! ""old_ramdiskSize: %d, new_ramdiskSize: %d, zerod out memory: %d\n", MODULE_NAME, "/dev/"MODULE_NODE_NAME, dev->devNo, (int) old_ramdiskSize, (int) new_ramdiskSize, (int) (new_ramdiskSize - old_ramdiskSize));
		}
		else {
			/* realloc failed, old ramdisk handle is still valid */
			printk(KERN_DEBUG "%s: device %s%d: Failed to reallocate ramdisk!\n", MODULE_NAME, "/dev/"MODULE_NODE_NAME, dev->devNo);
			new_offset = -ENOMEM;
			goto out;
		}
	}
	/* update the current seek */
	filp->f_pos = new_offset;

	printk(KERN_DEBUG "%s: device %s%d: Seeking to position: %ld\n", MODULE_NAME, "/dev/"MODULE_NODE_NAME, dev->devNo, (long) new_offset);
out:
	up(&dev->sem);
	return new_offset;
}
/*asp_mycdev_lseek*/

long asp_mycdev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	long retval = -1;
	struct asp_mycdev *mycdev = NULL;

	/* Extracts type and number bitfields;
	don't decode wrong commands; return -ENOTTY (Inappropriate IOCTL)  */
	if(_IOC_TYPE(cmd) != CDRV_IOC_MAGIC)
		return -ENOTTY;
	if(_IOC_NR(cmd) > ASP_IOCTL_MAXNR)
		return -ENOTTY;

	/* If everything is fine, extract the command and perform action */
	mycdev = filp->private_data;

	/* Enter Critical Section */
	if(down_interruptible(&mycdev->sem))
		return -ERESTARTSYS;

	switch (cmd)
	{
		/* clear the ramdisk & seek to start of the file */
		case ASP_CLEAR_BUF:
			memset(mycdev->ramdisk, 0, mycdev->size);
			filp->f_pos = 0;
			retval = 1;
			break;

		/* the control is unlikely to come here after MAXNR check above */
		default:
			retval = -ENOTTY;
	}

	/* Exit Critical Section */
	up(&mycdev->sem);

	/* Just to debug */
	if(retval == 1){
		printk(KERN_DEBUG "%s: device %s%d: Successful Reset!\n", MODULE_NAME, "/dev/"MODULE_NODE_NAME, mycdev->devNo);
	}
	return retval;
}
/*asp_mycdev_ioctl*/

static int setup_cdev(struct asp_mycdev *dev, int index)
{
	int error = 0;
	int retval = 0;
	dev_t devNo = 0;

	/* Device Number */
	devNo = MKDEV(_major, _minor + index);
	/* Init cdev */
	cdev_init(&dev->cdev, &asp_mycdev_fops);
	dev->cdev.owner = THIS_MODULE,
	dev->cdev.ops = &asp_mycdev_fops;
	/* Add the device, NOTE:: This makes the device go live! */
	error = cdev_add(&dev->cdev, devNo, 1);
	/* report error */
	if(error) {
		printk(KERN_WARNING "%s: Error %d adding mycdev%d\n", MODULE_NAME, error, index);
		retval = -1;
	}
	return retval;
}
/*Initial Device*/

static int mycdev_init_module(void)
{  
	dev_t devNum = 0;
	bool ramdiskAllocFailed = false;
	bool cdevSetupFailed = false;
	bool nodeSetupFailed = false;
	int i = 0, retval = 0;

	printk(KERN_INFO "%s: Initializing Module!\n", MODULE_NAME);

	/* Allocate major and range of minor numbers to work with the driver dynamically
	 unless otherwise specified at load time */
	if(_major || _minor) {
		devNum = MKDEV(_major, _minor);
		retval = register_chrdev_region(devNum, _num_devs, MODULE_NODE_NAME);
		
	}
	else {
		retval = alloc_chrdev_region(&devNum, _minor,_num_devs, MODULE_NODE_NAME);
		_major = MAJOR(devNum);
		//printk(KERN_DEBUG "the module node name is %s\n",MODULE_NODE_NAME);
	}
	if(retval < 0){
		printk(KERN_WARNING "%s: Unable to allocate major %d\n", MODULE_NAME, _major);
		return retval;
	}
	printk(KERN_DEBUG "%s: Requested Devices - %d, Major Number:- %d, Minor Number- %d\n",\
		MODULE_NAME, _num_devs, _major, _minor);

	/* Setup the device class, needed to create device nodes in sysfs */
	asp_mycdev_class = class_create(THIS_MODULE, MODULE_CLASS_NAME);
	if(IS_ERR_OR_NULL(asp_mycdev_class)){
		printk(KERN_WARNING "%s: Failed to Init Device Class %s\n", MODULE_NAME, MODULE_CLASS_NAME);
		retval = -1;
		goto FAIL;
	}
	printk(KERN_INFO "%s: Created device class: %s\n", MODULE_NAME, MODULE_CLASS_NAME);

	/* Allocate and setup the devices here */
	mycdev_devices = kzalloc(_num_devs * sizeof(struct asp_mycdev), GFP_KERNEL);
	if(mycdev_devices == NULL){
		retval = -ENOMEM;
	}

	/* Setup the devices one by one */
	for(i = 0; i < _num_devs; i++)
	{
		char nodeName[MAX_NODE_NAME_SIZE] = { 0 };
		int cdevStatus = 0;

		/* Device number */
		mycdev_devices[i].devNo = i;
		/* Initializing semaphore */
		sema_init(&mycdev_devices[i].sem,1);

		/* Initializing ramdisk */
		mycdev_devices[i].ramdisk = kzalloc((size_t) _ramdisk_size, GFP_KERNEL);
		if(mycdev_devices[i].ramdisk == NULL){
			/* mark that we failed to allocate current device memory,
			we will clean up previously allocated devices in cleanup module */
			printk(KERN_WARNING "%s: Failed to allocate ramdisk for device %d\n", MODULE_NAME, i);
			ramdiskAllocFailed = true;
			break;	/* exit for */
		}
		lastSuccessfulRamdisk = i;
		mycdev_devices[i].size = _ramdisk_size;

		/* Create device node here */
		snprintf(nodeName, sizeof(nodeName), MODULE_NODE_NAME"%d", i);

		mycdev_devices[i].ptr = device_create(asp_mycdev_class, NULL, MKDEV(_major, _minor + i), NULL, nodeName);
		if(IS_ERR_OR_NULL(mycdev_devices[i].ptr))
		{
			/* mark that we failed to create and register current device node with sysfs,
			we will clean up previously device nodes in cleanup module */
			printk(KERN_WARNING "%s: Failed to Create Device Node %s\n", MODULE_NAME, nodeName);
			nodeSetupFailed = true;
			break;
		}
		lastSuccessfulNode = i; // mark the last successful node

		/* Setup cdev struct here */
		cdevStatus = setup_cdev(&mycdev_devices[i], i);
		if(cdevStatus < 0){
			/* mark that we failed to allocate current cdev,
			we will clean up previously allocated cdevs in cleanup module */
			printk(KERN_WARNING "%s: Failed to setup cdev for device %d\n", MODULE_NAME, i);
			cdevSetupFailed = true;
			break;
		}
		lastSuccessfulCdev = i;
	}
	/* cleanup if we failed to allocate device memory */
	if(ramdiskAllocFailed || nodeSetupFailed || cdevSetupFailed)
	{
		retval = -ENOMEM;
	}

	printk(KERN_INFO "%s: Initialization Complete!\n", MODULE_NAME);
	printk(KERN_INFO "%s: lastSuccessfulRamdisk: %d, lastSuccessfulNode: %d, lastSuccessfulCdev: %d\n", MODULE_NAME, lastSuccessfulRamdisk, lastSuccessfulNode, lastSuccessfulCdev);

	return 0;
FAIL:
	mycdev_cleanup_module();
	return retval;
}  
/*Init module*/

static void mycdev_cleanup_module(void)
{  
	int i = 0;

	printk(KERN_INFO "%s: Cleaning Up Module!\n", MODULE_NAME);

	/* we will free the failed struct */
	/* Cleanup devices */
	if(mycdev_devices != NULL)
	{
		/* ramdisk */
		for(i = 0; i <= lastSuccessfulRamdisk; i++)
		{
			if(mycdev_devices[i].ramdisk != NULL)
			{
				kfree(mycdev_devices[i].ramdisk); // free the space
				mycdev_devices[i].ramdisk = NULL;
			}
		}
		/* cdev */
		for(i = 0; i <= lastSuccessfulCdev; i++)
		{
			cdev_del(&mycdev_devices[i].cdev);
		}
		/* device nodes */
		for(i = 0; i <= lastSuccessfulNode; i++)
		{
			device_destroy(asp_mycdev_class, MKDEV(_major, _minor + i));
		}
		/* free up device array */
		kfree(mycdev_devices);
		mycdev_devices = NULL;

		printk(KERN_DEBUG "%s: Freed up %d devices/ramdisks.\n", MODULE_NAME, lastSuccessfulRamdisk + 1);
		printk(KERN_DEBUG "%s: Freed up %d devices/nodes.\n", MODULE_NAME, lastSuccessfulNode + 1);
		printk(KERN_DEBUG "%s: Freed up %d devices/cdevs.\n", MODULE_NAME, lastSuccessfulCdev + 1);
	}

	/* Clean up device class */
	if(!IS_ERR_OR_NULL(asp_mycdev_class)){
		class_destroy(asp_mycdev_class);
		asp_mycdev_class = NULL;
		printk(KERN_DEBUG "%s: Freed up %s device class.\n", MODULE_NAME, MODULE_CLASS_NAME);
	}

	/* Cleaning up the chrdev_region,
	this is never called if the registration failes */
	unregister_chrdev_region(MKDEV(_major, _minor), _num_devs);

	printk(KERN_INFO "%s: Cleanup Done!\n", MODULE_NAME);
}  
/*Exit module: recycled/freed at the time of unloading your
device driver module.*/

module_init(mycdev_init_module);
module_exit(mycdev_cleanup_module);
