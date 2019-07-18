//
// Created by Administrator on 2019/7/11.
//

#ifndef M_BACK_SOCKET_TOOL_H
#define M_BACK_SOCKET_TOOL_H
#include <netinet/in.h>
int init_server_socket(void);
int set_nonblock(int fd);
#endif //M_BACK_SOCKET_TOOL_H
