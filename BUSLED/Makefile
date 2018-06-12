obj-m := busdrv.o
KDIR :=/CNDI_CD/source/BSP/kernel/kernel/
PWD := $(shell pwd)

export CROSS_COMPILE=arm-none-linux-gnueabi-

ali:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm *.o *.ko *.mod.c .*.cmd modules.order Module.symvers
