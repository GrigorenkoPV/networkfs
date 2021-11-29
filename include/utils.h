#pragma once

#include <linux/inet.h>
#include <linux/in.h>
#include <net/sock.h>

int nwfs_send_msg_to_server(struct socket *sock, char *send_buf);

int nwfs_recv_msg_from_server(struct socket *sock, char *recv_buf, int recv_buf_size);

int nwfs_connect_to_server_atoi(const char *c);

typedef char const *url_key_value_pair[2];
u64 nwfs_connect_to_server(const char *command, int params_count, url_key_value_pair params[], const char *token,
			   char *output_buf);
