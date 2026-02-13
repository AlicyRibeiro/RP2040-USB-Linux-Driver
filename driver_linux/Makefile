KDIR ?= /lib/modules/$(shell uname -r)/build
obj-m += pico_usb_driver.o
PWD := $(shell pwd)

export ARCH=x86_64
export CROSS_COMPILE=

all:
	$(MAKE) -C $(KDIR) M=$(PWD) CC=gcc modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f *.o *.ko *.mod.c *.symvers *.order *~ .*.cmd .tmp_versions/
