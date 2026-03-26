# Name of the module
obj-m := pcp_probe.o

# Path to the currently running kernel build directory
KDIR := /lib/modules/$(shell uname -r)/build

# Current directory
PWD := $(shell pwd)

# Default target: build module
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Clean build artifacts
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

# Install module
install:
	sudo insmod pcp_probe.ko || true

# Remove module
uninstall:
	sudo rmmod pcp_probe || true
