#ifndef LAB_UTILS_C
#define LAB_UTILS_C

#include <linux/inet.h>
#include <linux/in.h>
#include <net/sock.h>

#define ESOCKNOCREATE  -1
#define ESOCKNOCONNECT -2
#define ESOCKNOMSGSEND -3
#define ESOCKNOMSGRECV -4
#define ENOOKRESPONSE  -5
#define ENOSPACE       -10

int send_msg_to_server(struct socket *sock, char *send_buf)
{
	struct kvec send_vec;
	struct msghdr send_msg;
	memset(&send_msg, 0, sizeof(send_msg));
	memset(&send_vec, 0, sizeof(send_vec));
	send_vec.iov_base = send_buf;
	send_vec.iov_len = strlen(send_buf);
	return kernel_sendmsg(sock, &send_msg, &send_vec, 1, strlen(send_buf));
}

char connect_to_server_output_buf[8192];
const int connect_to_server_output_buf_size = 8192;

int recv_msg_from_server(struct socket *sock, char *recv_buf, int recv_buf_size)
{
	struct kvec recv_vec;
	struct msghdr recv_msg;
	int ret, sum;
	sum = 0;
	memset(recv_buf, 0, recv_buf_size);
	while (true)
	{
		memset(&recv_msg, 0, sizeof(recv_msg));
		memset(&recv_vec, 0, sizeof(recv_vec));
		recv_vec.iov_base = connect_to_server_output_buf;
		recv_vec.iov_len = connect_to_server_output_buf_size;
		ret = kernel_recvmsg(sock, &recv_msg, &recv_vec, 1, connect_to_server_output_buf_size, 0);
		if (ret == 0)
		{
			break;
		}
		memcpy(recv_buf + sum, connect_to_server_output_buf, ret);
		sum += ret;
	}
	return sum;
}

int connect_to_server_atoi(const char *c)
{
	int res;
	res = 0;
	while (*c != '\n' && *c != '\r')
	{
		int cur_c = *c - '0';
		if (cur_c < 0 || cur_c > 9)
		{
			return -1;
		}
		res = 10 * res + cur_c;
		c++;
	}
	return res;
}

char connect_to_server_send_buf[8192];
char connect_to_server_recv_buf[8192];

int connect_to_server(const char *command, int params_count, const char *params[], const char *token, char *output_buf)
{
	const int BUFFER_SIZE = 8192;
	struct socket sock;
	struct socket *sock_ptr;
	struct sockaddr_in s_addr;
	char *send_buf, *recv_buf;
	int i;
	int error;
	int message_len;
	send_buf = connect_to_server_send_buf;
	recv_buf = connect_to_server_recv_buf;
	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(80);
	s_addr.sin_addr.s_addr = in_aton("77.234.215.132");
	sock_ptr = &sock;
	error = sock_create_kern(&init_net, AF_INET, SOCK_STREAM, IPPROTO_TCP, &sock_ptr);
	if (error < 0)
	{
		return ESOCKNOCREATE;
	}
	error = sock_ptr->ops->connect(sock_ptr, (struct sockaddr *)&s_addr, sizeof(s_addr), 0);
	if (error != 0)
	{
		return ESOCKNOCONNECT;
	}
	strcpy(send_buf, "GET /teaching/os/networkfs/v1/");
	strcat(send_buf, token);
	strcat(send_buf, "/fs/");
	strcat(send_buf, command);
	strcat(send_buf, "?");
	i = 0;
	while (i < params_count)
	{
		if (i != 0)
		{
			strcat(send_buf, "&");
		}
		strcat(send_buf, params[i]);
		i++;
	}
	strcat(send_buf, " HTTP/1.1\nHost: nerc.itmo.ru\n\n");
	error = send_msg_to_server(sock_ptr, send_buf);
	if (error < 0)
	{
		return ESOCKNOMSGSEND;
	}
	error = recv_msg_from_server(sock_ptr, recv_buf, BUFFER_SIZE);
	if (error < 0)
	{
		return ESOCKNOMSGRECV;
	}
	if (strncmp(recv_buf, "HTTP/1.1 200 ", strlen("HTTP/1.1 200 ")) != 0)
	{
		return ENOOKRESPONSE;
	}
	recv_buf = strchr(recv_buf, '\n') + 1;
	recv_buf = strchr(recv_buf, '\n') + 1;
	recv_buf = strchr(recv_buf, '\n') + 1;
	recv_buf = strchr(recv_buf, '\n') + 1;
	recv_buf = strchr(recv_buf, ' ') + 1;
	message_len = connect_to_server_atoi(recv_buf);
	recv_buf = strchr(recv_buf, '\n') + 1;
	recv_buf = strchr(recv_buf, '\n') + 1;
	recv_buf = strchr(recv_buf, '\n') + 1;
	error = *(int *)(recv_buf);
	recv_buf += 8;
	memcpy(output_buf, recv_buf, message_len);
	kernel_sock_shutdown(sock_ptr, SHUT_RDWR);
	sock_release(sock_ptr);		
	return error;
}

#endif
