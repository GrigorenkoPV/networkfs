#pragma once

#include <linux/inet.h>
#include <linux/in.h>
#include <net/sock.h>

#define ESOCKNOCREATE  -1
#define ESOCKNOCONNECT -2
#define ESOCKNOMSGSEND -3
#define ESOCKNOMSGRECV -4
#define ENOOKRESPONSE  -5
#define ENOSPACE       -10

int send_msg_to_server(struct socket *sock, char *send_buf);

int recv_msg_from_server(struct socket *sock, char *recv_buf, int recv_buf_size);

int connect_to_server_atoi(const char *c);

int connect_to_server(const char *command, int params_count, const char *params[], const char *token, char *output_buf);
