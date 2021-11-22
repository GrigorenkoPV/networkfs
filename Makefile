obj-m += networkfs.o
module_name = networkfs
mount_path = mnt

.PHONY: all clean module_install module_remove dmesg install remove mount umount

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
clean:
	-rmdir mnt
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

module_install: all
	sudo insmod $(module_name).ko
module_remove:
	sudo rmmod $(module_name)
dmesg:
	sudo dmesg -c

install: module_install dmesg
remove: module_remove dmesg

mount:
	-mkdir "$(mount_path)" 2> /dev/null
	sudo mount --type networkfs "$(shell cat token.txt)" "$(mount_path)"

umount:
	sudo umount "$(mount_path)"
	-rmdir "$(mount_path)"
