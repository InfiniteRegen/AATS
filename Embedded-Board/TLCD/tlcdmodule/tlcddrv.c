#include "peripheral.h"

#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/platform_device.h>

/************************************************************************/
//
//  Text LCD driver region ( Text LCD )
//
/************************************************************************/
volatile unsigned short *tlcd_base;

int tlcd_open(struct inode *inode, struct file *pfile)
{
	printk("[tlcd_open] - success.\n");
	if (request_mem_region(TEXT_LED_BASE_ADDRESS, ADDRESS_MAP_SIZE, "CNLED") == NULL)
		return -EBUSY;

	tlcd_base = ioremap(TEXT_LED_BASE_ADDRESS, ADDRESS_MAP_SIZE);
	if (tlcd_base == NULL) {
		release_mem_region(TEXT_LED_BASE_ADDRESS, ADDRESS_MAP_SIZE);
		return -ENOMEM;
	}

	return 0;
}

ssize_t tlcd_read(struct file *pfile, char *buf, size_t count, loff_t *filePos)
{
	unsigned short wdata;
	unsigned int ret;

	wdata = readw(tlcd_base);
	ret = copy_to_user(buf, &wdata, 2);

	return 2;
}

ssize_t tlcd_write(struct file *pfile, const char *buf,size_t count, loff_t *filePos)
{
	unsigned short wdata;
	unsigned int ret;

	ret = copy_from_user(&wdata, buf, 2);
	writew(wdata, tlcd_base);

	return count;
}

int tlcd_release(struct inode *inode, struct file *pfile)
{

	if (tlcd_base != NULL) {
		iounmap(tlcd_base);
		tlcd_base = NULL;
		release_mem_region(TEXT_LED_BASE_ADDRESS, ADDRESS_MAP_SIZE);
	}

	return 0;
}

struct file_operations tlcd_fops = {
	.owner		= THIS_MODULE,
	.open		= tlcd_open,
	.write		= tlcd_write,
	.read		= tlcd_read,
	.release	= tlcd_release
};

struct miscdevice tlcd_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEV_NAME_TEXT_LCD,
    .fops = &tlcd_fops,
};

static int __init tlcd_init(void)
{
    int error;

    error = misc_register(&tlcd_device);
    if (error) {
        pr_err("can't misc_register :(\n");
        return error;
    }

    return 0;
}

static void __exit tlcd_exit(void)
{
    misc_deregister(&tlcd_device);
}

module_init(tlcd_init)
module_exit(tlcd_exit)

MODULE_DESCRIPTION("text lcd  Driver");
MODULE_AUTHOR("cndi");
MODULE_LICENSE("GPL");
