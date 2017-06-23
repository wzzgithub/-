$(warning KERNELRELEASE=$(KERNELRELEASE))
ifeq ($(KERNELRELEASE),)

KERNELDIR ?= /media/tl437x/linux-3.14.43/

#KERNELDIR ?= /lib/modules/$(shell uname -r)/build  
PWD := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules


clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module* modules*

.PHONY: modules modules_install clean

else
    obj-m := lcd.o
endif

