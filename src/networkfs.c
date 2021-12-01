#include "networkfs.h"
#include "utils.h"
#include "api.h"
#include <linux/memory.h>

struct file_system_type nwfs_fs_type = { .name = "networkfs", .mount = nwfs_mount, .kill_sb = nwfs_kill_sb };
struct inode_operations nwfs_inode_ops = { .lookup = nwfs_lookup,
					   .create = nwfs_create,
					   .unlink = nwfs_unlink,
					   .mkdir = nwfs_mkdir,
					   .rmdir = nwfs_rmdir,
					   .link = nwfs_link };
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
	mode |= S_IRWXUGO;
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
	struct nwfs_entry_info entry_info;
	struct inode *inode;
	u64 err;
#ifdef NWFSDEBUG
	printk(KERN_DEBUG "nwfs_lookup: parent_inode @%p, child_dentry @%p, flag = %u\n", parent_inode, child_dentry,
	       flag);
#endif
	err = nwfs_api_lookup(token_buffer, parent_inode->i_ino, child_dentry->d_name.name, &entry_info);
	if (err == NWFS_OK) {
		inode = nwfs_get_inode(parent_inode->i_sb, NULL, entry_info.entry_type == DT_DIR ? S_IFDIR : S_IFREG,
				       entry_info.ino);
		d_add(child_dentry, inode);
	}
	return NULL;
}

int nwfs_iterate(struct file *filp, struct dir_context *ctx)
{
	struct nwfs_entries *contents;
	struct nwfs_entry *entry;
	struct dentry *dentry;
	struct inode *inode;
	u64 err;
#ifdef NWFSDEBUG
	size_t e_no;
#endif
	contents = NULL;
	dentry = filp->f_path.dentry;
	inode = dentry->d_inode;

#ifdef NWFSDEBUG
	printk(KERN_DEBUG "nwfs_iterate: filp @%p, ctx @%p, ctx->pos = %llu, filp->f_pos = %llu\n", filp, ctx, ctx->pos,
	       filp->f_pos);
#endif
	ctx->pos = filp->f_pos;
	while (true) {
		if (ctx->pos == 0) {
			dir_emit(ctx, ".", strlen("."), inode->i_ino, DT_DIR);
		} else if (ctx->pos == 1) {
			dir_emit(ctx, "..", strlen(".."), dentry->d_parent->d_inode->i_ino, DT_DIR);
		} else {
			if (contents == NULL) {
#ifdef NWFSDEBUG
				printk(KERN_DEBUG "nwfs_iterate: getting contents for inode = %lu\n", inode->i_ino);
#endif
				contents = kmalloc(sizeof(struct nwfs_entries), GFP_KERNEL);
#ifdef NWFSDEBUG
				printk(KERN_DEBUG "nwfs_iterate: alloced @0x%p\n", contents);
#endif
				err = nwfs_api_list(token_buffer, inode->i_ino, contents);
#ifdef NWFSDEBUG
				printk(KERN_DEBUG "nwfs_iterate: got return code = %llu\n", err);
				if (err == NWFS_OK) {
					printk(KERN_DEBUG "newfs_iterate: got %ld entries\n", contents->entries_count);
					for (e_no = 0; e_no < contents->entries_count; e_no++) {
						printk(KERN_DEBUG
						       "nwfs_iterate: entry #%ld: type = 0x%x, inode = %lu, name = %s\n",
						       e_no, contents->entries[e_no].entry_type,
						       contents->entries[e_no].ino, contents->entries[e_no].name);
					}
				}
#endif
				if (err != NWFS_OK) {
					break;
				}
			}
			if (ctx->pos - 2 >= contents->entries_count) {
				break;
			} else {
				entry = &(contents->entries[ctx->pos - 2]);
				dir_emit(ctx, entry->name, strlen(entry->name), entry->ino, entry->entry_type);
			}
		}
		ctx->pos++;
	}
	if (contents != NULL) {
		kfree(contents);
	}
	return contents->entries_count + 2;
}

int nwfs_create(struct inode *parent_inode, struct dentry *child_dentry, umode_t mode, bool b)
{
	ino_t ino;
	u64 err;
#ifdef NWFSDEBUG
	printk(KERN_DEBUG "nwfs_create: parent_inode @%p, child_dentry @%p, mode = 0x%016x, b = %d\n", parent_inode,
	       child_dentry, mode, b);
#endif
	err = nwfs_api_create(token_buffer, parent_inode->i_ino, child_dentry->d_name.name, file, &ino);
	if (err == NWFS_OK) {
		d_add(child_dentry, nwfs_get_inode(parent_inode->i_sb, NULL, mode | S_IFREG, ino));
	}
	return 0;
}

int nwfs_unlink(struct inode *parent_inode, struct dentry *child_dentry)
{
	u64 err;
#ifdef NWFSDEBUG
	printk(KERN_DEBUG "nwfs_unlink: parent_inode @%p, child_dentry @%p\n", parent_inode, child_dentry);
#endif
	err = nwfs_api_unlink(token_buffer, parent_inode->i_ino, child_dentry->d_name.name);
	return 0;
}
int nwfs_mkdir(struct inode *parent_inode, struct dentry *child_dentry, umode_t mode)
{
	ino_t ino;
	u64 err;
#ifdef NWFSDEBUG
	printk(KERN_DEBUG "nwfs_mkdir: parent_inode @%p, child_dentry @%p, mode = 0x%016x\n", parent_inode,
	       child_dentry, mode);
#endif
	err = nwfs_api_create(token_buffer, parent_inode->i_ino, child_dentry->d_name.name, directory, &ino);
	if (err == NWFS_OK) {
		d_add(child_dentry, nwfs_get_inode(parent_inode->i_sb, NULL, mode | S_IFDIR, ino));
	}
	return 0;
}
int nwfs_rmdir(struct inode *parent_inode, struct dentry *child_dentry)
{
	u64 err;
#ifdef NWFSDEBUG
	printk(KERN_DEBUG "nwfs_rmdir: parent_inode @%p, child_dentry @%p\n", parent_inode, child_dentry);
#endif
	err = nwfs_api_rmdir(token_buffer, parent_inode->i_ino, child_dentry->d_name.name);
	return 0;
}

int nwfs_link(struct dentry *old_dentry, struct inode *parent_dir, struct dentry *new_dentry)
{
	u64 err;
#ifdef NWFSDEBUG
	printk(KERN_DEBUG "nwfs_link: old_dentry @%p, parent_dir @%p, new_dentry @%p\n", old_dentry, parent_dir,
	       new_dentry);
#endif
	err = nwfs_api_link(token_buffer, old_dentry->d_inode->i_ino, parent_dir->i_ino, new_dentry->d_name.name);
	return 0;
}
