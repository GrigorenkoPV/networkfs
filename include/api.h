#pragma once
#include "api_types.h"

#define NETWORKFS_METHOD(method_name) struct networkfs_api_result_##method_name networkfs_api_##method_name

NETWORKFS_METHOD(list)(char const *token, ino_t inode);
NETWORKFS_METHOD(create)(char const *token, ino_t parent, char const *name, enum networkfs_inode_kind type);
NETWORKFS_METHOD(read)(char const *token, ino_t inode);
NETWORKFS_METHOD(write)(char const *token, ino_t inode, char const *content);
NETWORKFS_METHOD(link)(char const *token, ino_t source, ino_t parent, char const *name);
NETWORKFS_METHOD(unlink)(char const *token, ino_t parent, char const *name);
NETWORKFS_METHOD(rmdir)(char const *token, ino_t parent, char const *name);
NETWORKFS_METHOD(lookup)(char const *token, ino_t parent, char const *name);
