#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/platform_device.h>

MODULE_DESCRIPTION("BUS-LED");
MODULE_AUTHOR("AATS");
MODULE_LICENSE("AATS CORP");

#define BUS_SW_ADDRESS		(0x06000000 + 0x5000)
#define ADDRESS_MAP_SIZE	0X1000

volatile unsigned short *busled_base;

ssize_t bus_write(struct file *pfile, const char *buf, size_t count, loff_t *filePos)
{
	unsigned short rdata = 0;
	unsigned short ret = 0;

	ret = copy_from_user((unsigned char *)&rdata, buf, 2);
	rdata &= 0xff;
	writew(rdata, busled_base);

	return 2;
}

int bus_release(struct inode *inode, struct file *pfile)
{
	if (busled_base != NULL) {
		iounmap(busled_base);
		busled_base = NULL;
		release_mem_region(BUS_SW_ADDRESS, ADDRESS_MAP_SIZE);
	}

	return 0;
}

int bus_open(struct inode *inode, struct file *pfile)
{
	if (check_mem_region(BUS_SW_ADDRESS, ADDRESS_MAP_SIZE)) {
		printk("busleddrv : memory already in use\n");
		return -EBUSY;
	}

	if (request_mem_region(BUS_SW_ADDRESS, ADDRESS_MAP_SIZE, "DIPSW") == NULL) {
		printk("busleddrv : fail to allocate mem region\n");
		return -EBUSY;
	}

	busled_base = ioremap(BUS_SW_ADDRESS, ADDRESS_MAP_SIZE);
	if (busled_base == NULL) {
		printk("busleddrv : fail to io mapping\n");
		release_mem_region(BUS_SW_ADDRESS, ADDRESS_MAP_SIZE);
		return -ENOMEM;
	}

	return 0;
}

struct file_operations bus_fops = {
	.owner = THIS_MODULE,
	.open = bus_open,
	.write = bus_write,
	.release = bus_release
};

struct miscdevice bus_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "busled",
	.fops = &bus_fops,

};

static int __init bus_init(void)
{
	int res;

	res = misc_register(&bus_device);

	if (res) {
		printk("fail to register the device\n");
		return res;
	}

	return 0;
}

static void __exit bus_exit(void)
{
	misc_deregister(&bus_device);
}

module_init(bus_init)
module_exit(bus_exit)
