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

struct dentry *networkfs_mount(struct file_system_type *fs_type, int flags, const char *token, void *data);
void networkfs_kill_sb(struct super_block *sb);


int networkfs_fill_super(struct super_block *sb, void *data, int silent);
struct inode *networkfs_get_inode(struct super_block *sb, const struct inode *dir, umode_t mode, int i_ino);


module_init(networkfs_init);
module_exit(networkfs_exit);
