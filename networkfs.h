#pragma once

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Grigorenko Pavel <311639@niuitmo.ru>");
MODULE_VERSION("0.01");

int networkfs_init(void);
void networkfs_exit(void);

module_init(networkfs_init);
module_exit(networkfs_exit);
