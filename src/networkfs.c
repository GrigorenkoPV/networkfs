#include "networkfs.h"
#include "utils.h"
#include "api_types.h"
#include "api.h"

struct file_system_type networkfs_fs_type = { .name = "networkfs",
					      .mount = networkfs_mount,
					      .kill_sb = networkfs_kill_sb };

int networkfs_init(void)
{
	int err;
	err = register_filesystem(&networkfs_fs_type);
	if (err) {
		printk(KERN_ERR "Error registering networkfs: register_filesystem returned code %d\n", err);

	} else {
		printk(KERN_DEBUG "Successfully registered networkfs\n");
	}
	return err;
}

void networkfs_exit(void)
{
	int err;
	err = unregister_filesystem(&networkfs_fs_type);
	if (err) {
		printk(KERN_ERR "Error unregistering networkfs: unregister_filesystem returned code %d\n", err);

	} else {
		printk(KERN_DEBUG "Successfully unregistered networkfs\n");
	}
}

struct dentry *networkfs_mount(struct file_system_type *fs_type, int flags, const char *token, void *data)
{
	// fixme
	struct dentry *ret;
	printk(KERN_DEBUG "NETWORK_FS_MOUNT: flags: %x, token: %s, data: %s\n", flags, token, (char const *)data);
	ret = mount_nodev(fs_type, flags, data, networkfs_fill_super);
	if (ret == NULL) {
		printk(KERN_ERR "Can't mount file system\n");
	} else {
		printk(KERN_DEBUG "Mounted successfuly\n");
	}
	return ret;
}

void networkfs_kill_sb(struct super_block *sb)
{
	// fixme
	printk(KERN_INFO "networkfs super block is destroyed. Unmount successfully.\n");
}

int networkfs_fill_super(struct super_block *sb, void *data, int silent)
{
	// fixme
	struct inode *inode;
	inode = networkfs_get_inode(sb, NULL, S_IFDIR, 1000);
	sb->s_root = d_make_root(inode);
	if (sb->s_root == NULL) {
		return -ENOMEM;
	}
	printk(KERN_INFO "return 0\n");
	return 0;
}

struct inode *networkfs_get_inode(struct super_block *sb, const struct inode *dir, umode_t mode, int i_ino)
{
	// fixme
	struct inode *inode;
	inode = new_inode(sb);
	inode->i_ino = i_ino;
	if (inode != NULL) {
		inode_init_owner(inode, dir, mode);
	}
	return inode;
}
