#include "utils.h"
#include "api_errors.h"
#include <linux/inet.h>
#include <linux/in.h>
#include <net/sock.h>

int nwfs_send_msg_to_server(struct socket *sock, char *send_buf)
{
	struct kvec send_vec;
	struct msghdr send_msg;
	memset(&send_msg, 0, sizeof(send_msg));
	memset(&send_vec, 0, sizeof(send_vec));
	send_vec.iov_base = send_buf;
	send_vec.iov_len = strlen(send_buf);
	return kernel_sendmsg(sock, &send_msg, &send_vec, 1, strlen(send_buf));
}

static char connect_to_server_output_buf[8192];
static const int connect_to_server_output_buf_size = 8192;

int nwfs_recv_msg_from_server(struct socket *sock, char *recv_buf, int recv_buf_size)
{
	struct kvec recv_vec;
	struct msghdr recv_msg;
	int ret, sum;
	sum = 0;
	memset(recv_buf, 0, recv_buf_size);
	while (true) {
		memset(&recv_msg, 0, sizeof(recv_msg));
		memset(&recv_vec, 0, sizeof(recv_vec));
		recv_vec.iov_base = connect_to_server_output_buf;
		recv_vec.iov_len = connect_to_server_output_buf_size;
		ret = kernel_recvmsg(sock, &recv_msg, &recv_vec, 1, connect_to_server_output_buf_size, 0);
		if (ret == 0) {
			break;
		} else if (ret < 0) {
			return NWFS_ERR_SOCK_RECV;
		}
		memcpy(recv_buf + sum, connect_to_server_output_buf, ret);
		sum += ret;
	}
	return sum;
}

int nwfs_connect_to_server_atoi(const char *c)
{
	int res;
	res = 0;
	while (*c != '\n' && *c != '\r') {
		int cur_c = *c - '0';
		if (cur_c < 0 || cur_c > 9) {
			return -1;
		}
		res = 10 * res + cur_c;
		c++;
	}
	return res;
}

static char connect_to_server_send_buf[8192];
static char connect_to_server_recv_buf[8192];

u64 nwfs_connect_to_server(const char *command, int params_count, url_key_value_pair params[], const char *token,
			   char *output_buf)
{
	const int BUFFER_SIZE = 8192;
	struct socket sock;
	struct socket *sock_ptr;
	struct sockaddr_in s_addr;
	char *send_buf, *recv_buf;
	int i;
	int error;
	int message_len;
	u64 return_code;

	if (command == NULL || token == NULL) {
		return NWFS_ERR_BAD_ARGUMENT;
	}

	send_buf = connect_to_server_send_buf;
	recv_buf = connect_to_server_recv_buf;
	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(80);
	s_addr.sin_addr.s_addr = in_aton("77.234.215.132");
	sock_ptr = &sock;
	error = sock_create_kern(&init_net, AF_INET, SOCK_STREAM, IPPROTO_TCP, &sock_ptr);
	if (error < 0) {
#ifdef NWFSDEBUG
		printk(KERN_ERR "Negative error number when creating socket: %d\n", error);
#endif
		return NWFS_ERR_SOCK_CREATE;
	}
	error = sock_ptr->ops->connect(sock_ptr, (struct sockaddr *)&s_addr, sizeof(s_addr), 0);
	if (error != 0) {
#ifdef NWFSDEBUG
		printk(KERN_ERR "Nonzero error number when connecting using socket: %d\n", error);
#endif
		return NWFS_ERR_SOCK_CONNECT;
	}
	strcpy(send_buf, "GET /teaching/os/networkfs/v1/");
	strcat(send_buf, token);
	strcat(send_buf, "/fs/");
	strcat(send_buf, command);
	strcat(send_buf, "?");
	i = 0;
	while (i < params_count) {
		if (i != 0) {
			strcat(send_buf, "&");
		}
		strcat(send_buf, params[i][0]);
		if (params[i][1] != NULL) {
			strcat(send_buf, "=");
			strcat(send_buf, params[i][1]);
		}
		i++;
	}
	strcat(send_buf, " HTTP/1.1\r\nHost: nerc.itmo.ru\r\nConnection: close\r\n\r\n");
#ifdef NWFSDEBUG
	printk(KERN_DEBUG "Sending message: %s\n", send_buf);
#endif
	error = nwfs_send_msg_to_server(sock_ptr, send_buf);
	if (error < 0) {
#ifdef NWFSDEBUG
		printk(KERN_ERR "Error sending message to the server: %d\n", error);
#endif
		return NWFS_ERR_SOCK_SEND;
	}
	error = nwfs_recv_msg_from_server(sock_ptr, recv_buf, BUFFER_SIZE);
	if (error < 0) {
#ifdef NWFSDEBUG
		printk(KERN_ERR "Error receiving message from the server: %d\n", error);
#endif
		return NWFS_ERR_SOCK_RECV;
	}
#ifdef NWFSDEBUG
	printk(KERN_DEBUG "Received message: %s\n", recv_buf);
#endif
	if (strncmp(recv_buf, "HTTP/1.1 200 ", strlen("HTTP/1.1 200 ")) != 0) {
		return NWFS_ERR_SOCK_NOK_HTTP;
	}
	recv_buf = strchr(recv_buf, '\n') + 1;
	recv_buf = strchr(recv_buf, '\n') + 1;
	recv_buf = strchr(recv_buf, '\n') + 1;
	recv_buf = strchr(recv_buf, '\n') + 1;
	recv_buf = strchr(recv_buf, ' ') + 1;
	message_len = nwfs_connect_to_server_atoi(recv_buf);
	recv_buf = strchr(recv_buf, '\n') + 1;
	recv_buf = strchr(recv_buf, '\n') + 1;
	recv_buf = strchr(recv_buf, '\n') + 1;
	return_code = *(u64 *)(recv_buf);
	recv_buf += 8;
	message_len -= 8;
	if ((message_len == 0) != (output_buf == NULL)) {
		return NWFS_ERR_BAD_ARGUMENT;
	}
	if (message_len != 0) {
		memcpy(output_buf, recv_buf, message_len);
	}
	kernel_sock_shutdown(sock_ptr, SHUT_RDWR);
	sock_release(sock_ptr);
	return return_code;
}
