# Thanks, https://github.com/RedPill-TTG/redpill-lkm and https://github.com/smlinux/rtl8723de

KERNEL_VERSION = $(shell uname -r)
KDIR = /lib/modules/$(KERNEL_VERSION)/build

# the name of the module we're gonna build
MODULE_NAME = networkfs
# telling linux's makefiles that we want to build our module
obj-m += $(MODULE_NAME).o


# the list of our source files
# it is only used to create the list of object files that will be linked into our module file
SRCS += src/networkfs.c src/utils.c src/api.c
# generating the said list by substituting .c with .o
OBJS += $(SRCS:.c=.o)
# ... and telling linux's makefiles that in order to generate our module,
# they will have to compile the provided files first
$(MODULE_NAME)-objs := $(OBJS)

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

.PHONY: all clean install remove mount umount stuff

# loads/unloads the module and prints dmesg's messages
install: all
	sudo insmod $(MODULE_NAME).ko
	@sudo dmesg -c
remove:
	sudo rmmod $(MODULE_NAME)
	@sudo dmesg -c


# mounts/unmounts a test directory, feel free to change variables below
MOUNT_PATH = /mnt/networkfs
TOKEN_FILE = token.txt

# if we have no token, gets it using binary api and strips 0x00 bytes of the return code
# don't forget to $(make install) first
mount:
	if [ ! -f "$(TOKEN_FILE)" ]; \
	then curl https://nerc.itmo.ru/teaching/os/networkfs/v1/token/issue | \
	 tr -d '\000' > "$(TOKEN_FILE)"; \
	fi
	mkdir -p "$(MOUNT_PATH)"
	sudo mount --type networkfs "$(shell cat $(TOKEN_FILE))" "$(MOUNT_PATH)"
	@sudo dmesg -c
umount:
	sudo umount "$(MOUNT_PATH)"
	@sudo dmesg -c

TIMESTAMP = $(shell date +%s)
TEST_FILE_NAME = $(MOUNT_PATH)/$(TIMESTAMP).txt
TEST_DIR_NAME = $(MOUNT_PATH)/$(TIMESTAMP).dir

stuff:
	ls -lahi "$$(dirname "$(MOUNT_PATH)")"
	@sudo dmesg -c
	-ls -lahi "$(MOUNT_PATH)"; sudo dmesg -c

	-touch "$(TEST_FILE_NAME)"; sudo dmesg -c
	-ls -lahi "$(MOUNT_PATH)"; sudo dmesg -c > /dev/null
	-rm "$(TEST_FILE_NAME)"; sudo dmesg -c
	-ls -lahi "$(MOUNT_PATH)"; sudo dmesg -c > /dev/null

	-mkdir "$(TEST_DIR_NAME)"; sudo dmesg -c
	-ls -lahi "$(MOUNT_PATH)"; sudo dmesg -c > /dev/null
	-rmdir "$(TEST_DIR_NAME)"; sudo dmesg -c
	-ls -lahi "$(MOUNT_PATH)"; sudo dmesg -c > /dev/null

