#include "networkfs.h"

int networkfs_init(void)
{
	printk(KERN_INFO "Hello, World!\n");
	return 0;
}

void networkfs_exit(void)
{
	printk(KERN_INFO "Goodbye!\n");
}
