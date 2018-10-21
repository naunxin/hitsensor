DRIVER_NAME = hi_tsensor

PWD       = $(shell pwd)
export ARCH=arm

#CROSS_COMPILE	?=mips-linux-gnu-
CROSS_COMPILE	?=arm-hisiv500-linux-

#KERNELDIR ?= /lib/modules/$(shell uname -r)/build
KERNELDIR ?=/home/rong/hi3520d_sdk/3536C/Hi3536C_SDK_V1.0.3.0/osdrv/opensource/kernel/linux-3.18.y
EXE			= $(DRIVER_NAME).ko

obj-m := $(DRIVER_NAME).o
hi_tsensor-objs := Hi_Tsensor.o

.PHONY: all
all:
	@echo "  Making $@ ..."
ifneq ($(CROSS_COMPILE),)
	$(MAKE) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNELDIR) M=$(PWD) modules
else
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif
ifneq ($(OUT),)
	cp -f $(EXE) $(OUT)/
endif
ifneq ($(INC),)
	cp -f *.h $(INC)/
endif

.PHONY: clean
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions modules.order Module.symvers *.ko.unsigned

depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > .depend

ifeq (.depend,$(wildcard .depend))
	include .depend
endif
