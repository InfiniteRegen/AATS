#include "peripheral.h"

#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/platform_device.h>

/************************************************************************
*
*  OLED driver region
*
************************************************************************/
volatile unsigned short  *oled_base;

int oled_open(struct inode *inode, struct file *pfile)
{

	printk("[oled_open] - success.\n");
	if (request_mem_region(OLED_BASE_ADDRESS ,ADDRESS_MAP_SIZE,"CNLED") == NULL)
		return -EBUSY;

	oled_base = ioremap(OLED_BASE_ADDRESS ,ADDRESS_MAP_SIZE);
	if (oled_base == NULL) {
		release_mem_region(OLED_BASE_ADDRESS, ADDRESS_MAP_SIZE);
		return -ENOMEM;
	}

	return 0;
}

ssize_t oled_read(struct file *pfile, char *buf, size_t count, loff_t *filePos)
{
	unsigned short wdata;
	unsigned int ret;

	wdata = readw(oled_base);
	ret = copy_to_user(buf, &wdata, 2);

	return 2;
}

ssize_t oled_write(struct file *pfile, const char *buf,size_t count, loff_t *filePos)
{

	unsigned short wdata;
	unsigned int ret;

	ret = copy_from_user(&wdata, buf, 2);
	writew(wdata, oled_base);

	return count;
}

int oled_release(struct inode *inode , struct file *pfile)
{

	if (oled_base != NULL) {
		iounmap(oled_base);
		oled_base = NULL;
		release_mem_region(OLED_BASE_ADDRESS, ADDRESS_MAP_SIZE);
	}

	return 0;
}

struct file_operations oled_fops = {
	.owner		= THIS_MODULE,
	.open		= oled_open,
	.write		= oled_write,
	.read		= oled_read,
	.release	= oled_release
};


struct miscdevice oled_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEV_NAME_OLED,
    .fops = &oled_fops,
};

static int __init oled_init(void)
{
    int error;

    error = misc_register(&oled_device);
    if (error) {
        pr_err("can't misc_register :(\n");
        return error;
    }

    return 0;
}

static void __exit oled_exit(void)
{
    misc_deregister(&oled_device);
}

module_init(oled_init)
module_exit(oled_exit)

MODULE_DESCRIPTION("oled Driver");
MODULE_AUTHOR("cndi");
MODULE_LICENSE("GPL");
