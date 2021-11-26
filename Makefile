module_name = networkfs

# Thanks, https://github.com/RedPill-TTG/redpill-lkm
SRCS += src/networkfs.c src/utils.c
EXTRA_CFLAGS += -I$(src)/include
OBJS += $(SRCS:.c=.o)

obj-m += $(module_name).o
$(module_name)-objs := $(OBJS)


.PHONY: clean token.txt install uninstall mount umount
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean


install: all
	sudo insmod $(module_name).ko
	sudo dmesg -c
remove:
	sudo rmmod $(module_name)
	sudo dmesg -c


mount_path = mnt
token_file = token.txt

mount:
	if [ ! -f "$(token_file)" ]; \
	then curl https://nerc.itmo.ru/teaching/os/networkfs/v1/token/issue | \
	 tr -d '\000' > "$(token_file)"; \
	fi
	mkdir -p "$(mount_path)"
	sudo mount --type networkfs "$(shell cat $(token_file))" "$(mount_path)"
	sudo dmesg -c
umount:
	sudo umount "$(mount_path)"
	-rmdir "$(mount_path)"
	sudo dmesg -c
