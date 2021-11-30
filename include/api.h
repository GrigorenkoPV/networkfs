#pragma once
#include "api_errors.h"
#include <linux/types.h>
#include <linux/fs.h>

#define NWFS_MAX_FILE_LENGTH 512
#define NWFS_MAX_FILE_NAME_LENGTH 256
#define NWFS_MAX_FILES_IN_DIR 16

struct nwfs_entries {
	size_t entries_count;
	struct entry {
		unsigned char entry_type; // DT_DIR (4) or DT_REG (8)
		ino_t ino;
		char name[NWFS_MAX_FILE_NAME_LENGTH];
	} entries[NWFS_MAX_FILES_IN_DIR];
};

struct nwfs_content {
	u64 content_length;
	char content[NWFS_MAX_FILE_LENGTH];
};
struct nwfs_entry_info {
	unsigned char entry_type; // DT_DIR (4) or DT_REG (8)
	ino_t ino;
};

enum networkfs_inode_kind { directory, file };

u64 nwfs_api_list(char const *token, ino_t inode, struct nwfs_entries *result);
u64 nwfs_api_create(char const *token, ino_t parent, char const *name, enum networkfs_inode_kind type, ino_t *result);
u64 nwfs_api_read(char const *token, ino_t inode, struct nwfs_content *result);
u64 nwfs_api_write(char const *token, ino_t inode, char const *content);
u64 nwfs_api_link(char const *token, ino_t source, ino_t parent, char const *name);
u64 nwfs_api_unlink(char const *token, ino_t parent, char const *name);
u64 nwfs_api_rmdir(char const *token, ino_t parent, char const *name);
u64 nwfs_api_lookup(char const *token, ino_t parent, char const *name, struct nwfs_entry_info *result);
