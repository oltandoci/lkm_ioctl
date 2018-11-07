
/*!
 * \file 	ioctl_cmd.c
 * \brief 	ioctl functions for R/W
 * \author 	O.D
 * \version 0.1
 * \date 	September 4th, 2018
 *
 * O.D - September 4th, 2018
 * File creation.
 * Define RW funtions for ioctl.
 *
 */
 
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/uaccess.h>

#include "ioctl_cmd.h"

static int my_open(struct inode *inode_p, struct file *file_p);
static int my_release(struct inode *inode_p, struct file *file_p);
static long ioctl_handle(struct file *file_p, unsigned int cmd, unsigned long arg);

static dev_t dev;
static struct cdev c_dev;
static struct class *c1;
static int v1 = 1, v2 = 2;

static const struct file_operations ioctl_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .unlocked_ioctl = ioctl_handle
};

/*!
 * \fn 		ioctl_init
 * \brief 	Module init
 * \param 	void
 * \return 	0 if no error, otherwise -1.
 * \author 	O.D
 */
int ioctl_init(void)
{
	int ret;
	struct device *dev_ret;
	
    /*request the kernel for FIRST_MINOR devices*/  
    if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, MOD_ALIAS)) < 0) {
    	printk(KERN_INFO "issues calling alloc_chrdev_region\n");
        return ret;
    }
    
    /*create /sys/class/MOD_ALIAS entry*/
    if (IS_ERR(c1 = class_create(THIS_MODULE, MOD_ALIAS))) {
    	printk(KERN_INFO "issues calling class_create\n");
    	unregister_chrdev_region(dev, MINOR_CNT);
    	return PTR_ERR(c1);
    }
    
    /*init cdev in order to use user ioctl commands*/
    cdev_init(&c_dev, &ioctl_fops);
    c_dev.owner = THIS_MODULE; /*cf LDD 3rd ed, l.56*/
    
    /*once the cdev is set up, tell the kernel about it*/
    if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0) {
    	printk(KERN_INFO "issues calling cdev_add\n");
    	unregister_chrdev_region(dev, MINOR_CNT);
    	return ret;
    }
    
    /*create /sys/class/MOD_ALIAS/test_command entry and /dev/test_command entries*/
    if (IS_ERR(dev_ret = device_create(c1, NULL, dev, NULL, "test_command"))) {
    	printk(KERN_INFO "issues calling device_create\n");
    	class_destroy(c1);
    	cdev_del(&c_dev);
    	unregister_chrdev_region(dev, MINOR_CNT);
    	return PTR_ERR(dev_ret);
    }
    
    return 0;
}

/*!
 * \fn 		ioctl_exit
 * \brief 	Module exit
 * \param 	void
 * \return 	void
 * \author 	O.D
 */
void ioctl_exit(void)
{
	device_destroy(c1, dev);
    class_destroy(c1);
	cdev_del(&c_dev);
	unregister_chrdev_region(dev, MINOR_CNT);
}

/*!
 * \fn 		my_open
 * \brief 	Called from open
 * \param 	struct inode *, struct file *
 * \return 	0
 * \author 	O.D
 */
int my_open(struct inode *inode_p, struct file *file_p)
{
    return 0;
} 

/*!
 * \fn 		my_release
 * \brief 	Called from release
 * \param 	struct inode *, struct file *
 * \return 	0
 * \author 	O.D
 */
int my_release(struct inode *inode_p, struct file *file_p)
{
    return 0;
} 

/*!
 * \fn 		ioctl_handle
 * \brief 	Called from unlocked_ioctl
 * \param 	struct file *,  unsigned int, unsigned long
 * \return 	0 if OK, error value otherwise
 * \author 	O.D
 */
long ioctl_handle(struct file *file_p, unsigned int cmd, unsigned long arg)
{
	mod_arg_t st;
	
	switch (cmd) {
	
		case MOD_GET_VAR:
			st.status = v1;
			st.action = v2;
			if (copy_to_user((mod_arg_t *)arg, &st, sizeof(mod_arg_t)))
				return -EACCES;
			break;
			
		case MOD_CLR_VAR:
			v1 = v2 = 0;
			break;
			
		case MOD_SET_VAR:
			if (copy_from_user(&st, (mod_arg_t *)arg, sizeof(mod_arg_t)))
				return -EACCES;
			v1 = st.status;
			v2 = st.action;
			break;
			
		default:
			return -EINVAL;
	}
	
	return 0;
} 
