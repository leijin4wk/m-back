//
// Created by Administrator on 2019/7/11.
//

#ifndef M_BACK_SOCKET_H
#define M_BACK_SOCKET_H
#include <sys/socket.h>
#include <netinet/in.h>
int init_server_socket(void);
int set_nonblock(int fd);
#endif //M_BACK_SOCKET_H
