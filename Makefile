# Thanks, https://github.com/RedPill-TTG/redpill-lkm and https://github.com/smlinux/rtl8723de

KERNEL_VERSION = $(shell uname -r)
KDIR = /lib/modules/$(KERNEL_VERSION)/build

# the name of the module we're gonna build
module_name = networkfs
# telling linux's makefiles that we want to build our module
obj-m += $(module_name).o


# the list of our source files
# it is only used to create the list of object files that will be linked into our module file
SRCS += src/networkfs.c src/utils.c src/api.c
# generating the said list by substituting .c with .o
OBJS += $(SRCS:.c=.o)
# ... and telling linux's makefiles that in order to generate our module,
# they will have to compile the provided files first
$(module_name)-objs := $(OBJS)

# The way it works is a bit funky to say the least.
# If you're fine with having essentially one C file for your module (let's say modulename.c),
# then you just need to put
# 	obj-m += modulename.o
# in your makefile. But if you want to have multiple files: file1.c, file2.c, file3.c ...
# then you still have to do
# 	obj-m += modulename.o
# but this time you also add a magic line
# 	modulename-objs := file1.o file2.o file3.o
# Also note that none of the separate files can be named the same as the module.
# (But src/modulename is fine, and that's what I do here).


# the directory where you want to store your headers
INCLUDE_DIRS += include

# Linux's makefiles seem to nicely resolve $(src) to dirname of this makefile
EXTRA_CFLAGS += $(addprefix -I$(src)/,$(INCLUDE_DIRS))

# For debug logging
ifdef DEBUG
EXTRA_CFLAGS += -DNWFSDEBUG
endif

all:
	make -C "$(KDIR)" M=$(PWD) modules
clean:
	make -C "$(KDIR)" M=$(PWD) clean

.PHONY: all clean install remove mount umount

# loads/unloads the module and prints dmesg's messages
install: all
	sudo insmod $(module_name).ko
	sudo dmesg -c
remove:
	sudo rmmod $(module_name)
	sudo dmesg -c


# mounts/unmounts a test directory, feel free to change variables below
mount_path = /mnt/networkfs
token_file = token.txt

# if we have no token, gets it using binary api and strips 0x00 bytes of the return code
# don't forget to $(make install) first
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
