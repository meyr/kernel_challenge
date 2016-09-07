#
# Makefile for external module
#


PWD := $(shell pwd)
KERVER := $(shell uname -r)
WORKDIR = /lib/modules/$(KERVER)/build
MODULE_NAME = myDebugfs
SRC_FILE = hello.c
OBJ_FILE = $(SRC_FILE:.c=.o)

obj-m := $(MODULE_NAME).o
$(MODULE_NAME)-y = $(OBJ_FILE)
ccflags-y := -DDEBUG 

modules:
	$(MAKE) -C $(WORKDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(WORKDIR) M=$(PWD) clean
modules_install:
	$(MAKE) -C $(WORKDIR) M=$(PWD) modules_install
test:
	@echo MODULE_NAME = $(MODULE_NAME)
	@echo SRC_FILE = $(SRC_FILE)	
	@echo WORKDIR = $(WORKDIR)
	@echo ccflags = $(ccflags-y)

.PHONY:	modules modules_install clean
