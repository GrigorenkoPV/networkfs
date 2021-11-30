#include "networkfs.h"
#include "utils.h"
#include "api.h"

struct file_system_type nwfs_fs_type = { .name = "networkfs", .mount = nwfs_mount, .kill_sb = nwfs_kill_sb };
struct inode_operations nwfs_inode_ops = { .lookup = nwfs_lookup };
struct file_operations nwfs_dir_ops = { .iterate = nwfs_iterate };

// fixme
static char token_buffer[1024];

int nwfs_init(void)
{
	int err;
	err = register_filesystem(&nwfs_fs_type);
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
	err = unregister_filesystem(&nwfs_fs_type);
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
	printk(KERN_DEBUG "nwfs_mount: flags = 0x%x, token = %s, data @%p = %s\n", flags, token, data,
	       (char const *)data);
#endif
	ret = mount_nodev(fs_type, flags, data, nwfs_fill_super);
	if (ret == NULL) {
		printk(KERN_ERR "nwfs_mount: Can't mount file system\n");
	} else {
#ifdef NWFSDEBUG
		printk(KERN_DEBUG "nwfs_mount: success!\n");
#endif
		strcpy(token_buffer, token);
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
	struct inode *inode;
#ifdef NWFSDEBUG
	printk(KERN_DEBUG "nwfs_get_inode: sb @%p, dir @%p, mode = 0x%016x, i_ino = %d\n", sb, dir, mode, i_ino);
#endif
	mode |= S_IRWXU | S_IRWXG | S_IRWXO;
#ifdef NWFSDEBUG
	printk(KERN_DEBUG "nwfs_get_inode: updated mode to = 0x%016x\n", mode);
#endif
	inode = new_inode(sb);
	if (inode != NULL) {
		inode->i_ino = i_ino;
		inode->i_op = &nwfs_inode_ops;
		inode->i_fop = &nwfs_dir_ops;
		inode_init_owner(inode, dir, mode);
	}
	return inode;
}

struct dentry *nwfs_lookup(struct inode *parent_inode, struct dentry *child_dentry, unsigned int flag)
{
#ifdef NWFSDEBUG
	printk(KERN_DEBUG "nwfs_lookup: parent_inode @%p, child_dentry @%p, flag = %u\n", parent_inode, child_dentry,
	       flag);
#endif
	return NULL; // todo
}

int nwfs_iterate(struct file *filp, struct dir_context *ctx)
{
	struct nwfs_entries contents;
	struct dentry *dentry;
	struct inode *inode;
	int stored;
	unsigned char ftype;
	ino_t ino;
	ino_t dino;
	u64 err;
	size_t e_no;

#ifdef NWFSDEBUG
	printk(KERN_DEBUG "nwfs_iterate: filp @%p, ctx @%p\n", filp, ctx);
#endif
	dentry = filp->f_path.dentry;
	inode = dentry->d_inode;
	ctx->pos = filp->f_pos;
	stored = 0;
	ino = inode->i_ino;
#ifdef NWFSDEBUG
	printk(KERN_DEBUG "nwfs_iterate: getting contents for inode = %lu, token = %s\n", ino, token_buffer);
#endif
	err = nwfs_api_list(token_buffer, ino, &contents);
#ifdef NWFSDEBUG
	printk(KERN_DEBUG "nwfs_iterate: got return code = %llu\n", err);
	if (err == 0) {
		printk(KERN_DEBUG "newfs_iterate: got %ld entries\n", contents.entries_count);
		for (e_no = 0; e_no < contents.entries_count; e_no++) {
			printk(KERN_DEBUG "nwfs_iterate: entry #%ld: type = 0x%x, inode = %lu, name = %s\n", e_no,
			       contents.entries[e_no].entry_type, contents.entries[e_no].ino,
			       contents.entries[e_no].name);
		}
	}
#endif
	// todo
	//	while (true) {
	//		if (ino == 1000) {
	//			if (ctx->pos == 0) {
	//				strcpy(fsname, ".");
	//				ftype = DT_DIR;
	//				dino = ino;
	//			} else if (ctx->pos == 1) {
	//				strcpy(fsname, "..");
	//				ftype = DT_DIR;
	//				dino = dentry->d_parent->d_inode->i_ino;
	//			} else if (ctx->pos == 2) {
	//				strcpy(fsname, "test.txt");
	//				ftype = DT_REG;
	//				dino = 101;
	//			} else {
	//				return stored;
	//			}
	//		}
	//		dir_emit(ctx, fsname, strlen(fsname), dino, ftype);
	//		stored++;
	//		ctx->pos++;
	//	}
	return stored;
}
