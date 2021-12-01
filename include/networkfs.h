#pragma once

#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/kernel.h>

typedef struct file_operations operations;
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Grigorenko Pavel <311639@niuitmo.ru>");
MODULE_VERSION("0.01");

int nwfs_init(void);
void nwfs_exit(void);

struct dentry *nwfs_mount(struct file_system_type *fs_type, int flags, const char *token, void *data);
void nwfs_kill_sb(struct super_block *sb);

int nwfs_fill_super(struct super_block *sb, void *data, int silent);
struct inode *nwfs_get_inode(struct super_block *sb, const struct inode *dir, umode_t mode, int i_ino);

struct dentry *nwfs_lookup(struct inode *parent_inode, struct dentry *child_dentry, unsigned int flag);
int nwfs_iterate(struct file *filp, struct dir_context *ctx);

int nwfs_create(struct inode *parent_inode, struct dentry *child_dentry, umode_t mode, bool b);
int nwfs_unlink(struct inode *parent_inode, struct dentry *child_dentry);

int nwfs_mkdir(struct inode *parent_inode, struct dentry *child_dentry, umode_t mode);
int nwfs_rmdir(struct inode *parent_inode, struct dentry *child_dentry);

ssize_t nwfs_read(struct file *filp, char *buffer, size_t len, loff_t *offset);
ssize_t nwfs_write(struct file *filp, const char *buffer, size_t len, loff_t *offset);

int nwfs_link(struct dentry *old_dentry, struct inode *parent_dir, struct dentry *new_dentry);

module_init(nwfs_init);
module_exit(nwfs_exit);
