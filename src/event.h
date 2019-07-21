//
// Created by Administrator on 2019/7/11.
//

#ifndef M_BACK_EVENT_H
#define M_BACK_EVENT_H
#include <sys/epoll.h>
#include "ssl_tool.h"
//最大同时处理事件数
#define MAXEVENTS 1024

struct m_event {
    int event_fd;
    char *client_ip;
    SSL* ssl;
};
void ev_loop_init();
void ev_accept_start(int server_fd);
void ev_loop_start();
#endif //M_BACK_EVENT_H
