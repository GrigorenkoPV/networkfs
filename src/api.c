#include "api.h"
#include "utils.h"

#define INODE_S_LEN (sizeof(ino_t) * 3)

u64 nwfs_api_list(char const *token, ino_t inode, struct nwfs_entries *result)
{
	url_key_value_pair params[1];
	char inode_s[INODE_S_LEN];

#ifdef NWFSDEBUG
	if (token == NULL || result == NULL) {
		return NWFS_ERR_BAD_ARGUMENT;
	}
#endif

	sprintf(inode_s, "%lu", inode);

	params[0][0] = "inode";
	params[0][1] = inode_s;

	return nwfs_connect_to_server("list", 1, params, token, (char *)(void *)result);
}

u64 nwfs_api_create(char const *token, ino_t parent, char const *name, enum networkfs_inode_kind type, ino_t *result)
{
	url_key_value_pair params[3];
	char parent_s[INODE_S_LEN];

#ifdef NWFSDEBUG
	if (token == NULL || result == NULL) {
		return NWFS_ERR_BAD_ARGUMENT;
	}
#endif

	sprintf(parent_s, "%lu", parent);

	params[0][0] = "parent";
	params[0][1] = parent_s;

	params[1][0] = "name";
	params[1][1] = name;

	params[2][0] = "type";
	switch (type) {
	case directory:
		params[2][1] = "directory";
		break;
	case file:
		params[2][1] = "file";
		break;
	default:
		return NWFS_ERR_BAD_ARGUMENT;
	}

	return nwfs_connect_to_server("create", 3, params, token, (char *)(void *)result);
}

u64 nwfs_api_read(char const *token, ino_t inode, struct nwfs_content *result)
{
	url_key_value_pair params[1];
	char inode_s[INODE_S_LEN];

#ifdef NWFSDEBUG
	if (token == NULL || result == NULL) {
		return NWFS_ERR_BAD_ARGUMENT;
	}
#endif

	sprintf(inode_s, "%lu", inode);

	params[0][0] = "inode";
	params[0][1] = inode_s;

	return nwfs_connect_to_server("read", 1, params, token, (char *)(void *)result);
}

u64 nwfs_api_write(char const *token, ino_t inode, char const *content)
{
	url_key_value_pair params[2];
	char inode_s[INODE_S_LEN];

#ifdef NWFSDEBUG
	if (token == NULL) {
		return NWFS_ERR_BAD_ARGUMENT;
	}
#endif

	sprintf(inode_s, "%lu", inode);

	params[0][0] = "inode";
	params[0][1] = inode_s;

	params[1][0] = "content";
	params[1][1] = content;

	return nwfs_connect_to_server("write", 1, params, token, NULL);
}

u64 nwfs_api_link(char const *token, ino_t source, ino_t parent, char const *name)
{
	url_key_value_pair params[3];
	char source_s[INODE_S_LEN];
	char parent_s[INODE_S_LEN];

#ifdef NWFSDEBUG
	if (token == NULL) {
		return NWFS_ERR_BAD_ARGUMENT;
	}
#endif

	sprintf(source_s, "%lu", source);
	sprintf(parent_s, "%lu", parent);

	params[0][0] = "source";
	params[0][1] = source_s;

	params[1][0] = "parent";
	params[1][1] = parent_s;

	params[2][0] = "name";
	params[2][1] = name;

	return nwfs_connect_to_server("link", 3, params, token, NULL);
}

u64 nwfs_api_unlink(char const *token, ino_t parent, char const *name)
{
	url_key_value_pair params[2];
	char parent_s[INODE_S_LEN];

#ifdef NWFSDEBUG
	if (token == NULL) {
		return NWFS_ERR_BAD_ARGUMENT;
	}
#endif

	sprintf(parent_s, "%lu", parent);

	params[0][0] = "parent";
	params[0][1] = parent_s;

	params[1][0] = "name";
	params[1][1] = name;

	return nwfs_connect_to_server("unlink", 2, params, token, NULL);
}

u64 nwfs_api_rmdir(char const *token, ino_t parent, char const *name)
{
	url_key_value_pair params[2];
	char parent_s[INODE_S_LEN];

#ifdef NWFSDEBUG
	if (token == NULL) {
		return NWFS_ERR_BAD_ARGUMENT;
	}
#endif

	sprintf(parent_s, "%lu", parent);

	params[0][0] = "parent";
	params[0][1] = parent_s;

	params[1][0] = "name";
	params[1][1] = name;

	return nwfs_connect_to_server("rmdir", 2, params, token, NULL);
}

u64 nwfs_api_lookup(char const *token, ino_t parent, char const *name, struct nwfs_entry_info *result)
{
	url_key_value_pair params[2];
	char parent_s[INODE_S_LEN];

#ifdef NWFSDEBUG
	if (token == NULL || result == NULL) {
		return NWFS_ERR_BAD_ARGUMENT;
	}
#endif

	sprintf(parent_s, "%lu", parent);

	params[0][0] = "parent";
	params[0][1] = parent_s;

	params[1][0] = "name";
	params[1][1] = name;

	return nwfs_connect_to_server("lookup", 2, params, token, (char *)(void *)result);
}
