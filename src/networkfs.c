#include "networkfs.h"
#include "utils.h"
#include "api.h"

struct file_system_type networkfs_fs_type = { .name = "networkfs", .mount = nwfs_mount, .kill_sb = nwfs_kill_sb };

int nwfs_init(void)
{
	int err;
	err = register_filesystem(&networkfs_fs_type);
	if (err) {
		printk(KERN_ERR "Error registering networkfs: register_filesystem returned code %d\n", err);

	} else {
#ifdef NWFSDEBUG
		printk(KERN_DEBUG "Successfully registered networkfs\n");
#endif
	}
	return err;
}

void nwfs_exit(void)
{
	int err;
	err = unregister_filesystem(&networkfs_fs_type);
	if (err) {
		printk(KERN_ERR "Error unregistering networkfs: unregister_filesystem returned code %d\n", err);

	} else {
#ifdef NWFSDEBUG
		printk(KERN_DEBUG "Successfully unregistered networkfs\n");
#endif
	}
}

struct dentry *nwfs_mount(struct file_system_type *fs_type, int flags, const char *token, void *data)
{
	struct dentry *ret;
#ifdef NWFSDEBUG
	printk(KERN_DEBUG "nwfs_mount: flags = %x, token = %s, data @%p = %s\n", flags, token, data,
	       (char const *)data);
#endif
	ret = mount_nodev(fs_type, flags, data, nwfs_fill_super);
	if (ret == NULL) {
		printk(KERN_ERR "nwfs_mount: Can't mount file system\n");
	} else {
#ifdef NWFSDEBUG
		printk(KERN_DEBUG "nwfs_mount: success!\n");
#endif
	}
	return ret;
}

void nwfs_kill_sb(struct super_block *sb)
{
#ifdef NWFSDEBUG
	printk(KERN_DEBUG "nwfs_kill_sb: Successfully unmounted\n");
#endif
}

int nwfs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *inode;
#ifdef NWFSDEBUG
	printk(KERN_DEBUG "nwfs_fill_super: data @%p = %s, silent = %d\n", data, (char const *)data, silent);
#endif
	inode = nwfs_get_inode(sb, NULL, S_IFDIR, 1000);
	sb->s_root = d_make_root(inode);
	if (sb->s_root == NULL) {
		return -ENOMEM;
	}

#ifdef NWFSDEBUG
	printk(KERN_DEBUG "nwfs_fill_super: Successfully filled super block @%p \n", sb);
#endif
	return 0;
}

struct inode *nwfs_get_inode(struct super_block *sb, const struct inode *dir, umode_t mode, int i_ino)
{
	// fixme
	struct inode *inode;
#ifdef NWFSDEBUG
	printk(KERN_DEBUG "nwfs_get_inode: sb @%p, dir @%p, mode = %x, i_ino = %d\n", sb, dir, mode, i_ino);
#endif
	inode = new_inode(sb);
	inode->i_ino = i_ino;
	if (inode != NULL) {
		inode_init_owner(inode, dir, mode);
	}
	return inode;
}
