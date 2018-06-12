#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/platform_device.h>

MODULE_DESCRIPTION("CountingDown");
MODULE_AUTHOR("AATS");
MODULE_LICENSE("AATS");

#define DOT_SW_ADDRESS		(0x06000000 + 0x1000)
#define ADDRESS_MAP_SIZE	0X1000

volatile unsigned short *dotsw_base;

ssize_t dot_write(struct file *pfile, const char *buf, size_t count, loff_t *filePos)
{
	unsigned short wData[2];
	unsigned int ret;

	ret = copy_from_user((unsigned char*)wData, buf, 4);

	if (wData[0] != 0 && wData[1] != 0) {
		writew(wData[0], dotsw_base);
		writew(wData[1], dotsw_base+1);
	}

	return 2;
}

int dot_release(struct inode *inode, struct file *pfile)
{
	if (dotsw_base != NULL) {
		iounmap(dotsw_base);
		dotsw_base = NULL;
		release_mem_region(DOT_SW_ADDRESS, ADDRESS_MAP_SIZE);
	}

	return 0;
}

int dot_open(struct inode *inode, struct file *pfile)
{
	if (check_mem_region(DOT_SW_ADDRESS, ADDRESS_MAP_SIZE)) {
		printk("dotswdrv : memory already in use\n");
		return -EBUSY;
	}

	if (request_mem_region(DOT_SW_ADDRESS, ADDRESS_MAP_SIZE, "DOTSW") == NULL) {
		printk("dotswdrv : fail to allocate mem region\n");
		return -EBUSY;
	}

	dotsw_base = ioremap(DOT_SW_ADDRESS, ADDRESS_MAP_SIZE);

	if (dotsw_base == NULL) {
		printk("dotswdrv : fail to io mapping\n");
		release_mem_region(DOT_SW_ADDRESS, ADDRESS_MAP_SIZE);
		return -ENOMEM;
	}

	return 0;
}

struct file_operations dot_fops = {
	.owner = THIS_MODULE,
	.open = dot_open,
	.write = dot_write,
	.release = dot_release
};

struct miscdevice dot_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "dotsw",
	.fops = &dot_fops,

};

static int __init dot_init(void)
{
	int res;

	res = misc_register(&dot_device);

	if(res) {
		printk("fail to register the device\n");
		return res;
	}

	return 0;
}

static void __exit dot_exit(void)
{
	misc_deregister(&dot_device);
}

module_init(dot_init)
module_exit(dot_exit)
