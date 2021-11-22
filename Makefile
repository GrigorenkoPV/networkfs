obj-m += networkfs.o
module_name = networkfs
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

install: all
	sudo insmod $(module_name).ko
remove:
	sudo rmmod $(module_name)
