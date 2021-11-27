#include "api_types.h"
#include "api.h"
#include "utils.h"

static char const *itoa(char *buff, ino_t inode)
{
	// todo
}

NETWORKFS_METHOD(list)(char const *token, ino_t inode)
{
	//todo
	//	struct networkfs_api_result_list result;
	//	char inode_s[sizeof(ino_t) * 3];
	//	int err;
	//	char const *params[1];
	//	params[0] = itoa(inode_s, inode);
	//	err = connect_to_server("list", 1, params, token, (void *)&result);
	//	if (err != 0) {
	//		*(int *)(void *)(&result) = err;
	//	}
	//	return result;
}
NETWORKFS_METHOD(create)(char const *token, ino_t parent, char const *name, enum networkfs_inode_kind type);
NETWORKFS_METHOD(read)(char const *token, ino_t inode);
NETWORKFS_METHOD(write)(char const *token, ino_t inode, char const *content);
NETWORKFS_METHOD(link)(char const *token, ino_t source, ino_t parent, char const *name);
NETWORKFS_METHOD(unlink)(char const *token, ino_t parent, char const *name);
NETWORKFS_METHOD(rmdir)(char const *token, ino_t parent, char const *name);
NETWORKFS_METHOD(lookup)(char const *token, ino_t parent, char const *name);
