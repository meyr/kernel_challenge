#
# Makefile for external module
#


PWD := $(shell pwd)
KERVER := $(shell uname -r)
WORKDIR = /lib/modules/$(KERVER)/build

#test-objs := test.o 
obj-m := test.o

modules:
	$(MAKE) -C $(WORKDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(WORKDIR) M=$(PWD) clean
modules_install:
	$(MAKE) -C $(WORKDIR) M=$(PWD) modules_install

.PHONY:	modules modules_install clean
