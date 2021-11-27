#pragma once
#include <linux/types.h>
#include <linux/fs.h>

#define NETWORKFS_RESULT_STRUCT(method_name, return_type)                                                              \
	struct networkfs_api_result_##method_name {                                                                    \
		u64 status;                                                                                            \
		return_type response;                                                                                  \
	}
#define NETWORKFS_RESULT_STRUCT_NO_RESPONSE(method_name)                                                               \
	struct networkfs_api_result_##method_name {                                                                    \
		u64 status;                                                                                            \
	}

struct networkfs_entries {
	size_t entries_count;
	struct entry {
		unsigned char entry_type; // DT_DIR (4) or DT_REG (8)
		ino_t ino;
		char name[256];
	} entries[16];
};

struct networkfs_content {
	u64 content_length;
	char content[];
};
struct networkfs_entry_info {
	unsigned char entry_type; // DT_DIR (4) or DT_REG (8)
	ino_t ino;
};

enum networkfs_inode_kind { directory, file };

NETWORKFS_RESULT_STRUCT(list, struct networkfs_entries);
NETWORKFS_RESULT_STRUCT(create, ino_t);
NETWORKFS_RESULT_STRUCT(read, struct networkfs_content);
NETWORKFS_RESULT_STRUCT_NO_RESPONSE(write);
NETWORKFS_RESULT_STRUCT_NO_RESPONSE(link);
NETWORKFS_RESULT_STRUCT_NO_RESPONSE(unlink);
NETWORKFS_RESULT_STRUCT_NO_RESPONSE(rmdir);
NETWORKFS_RESULT_STRUCT(lookup, struct networkfs_entry_info);
