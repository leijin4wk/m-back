//
// Created by Administrator on 2019/7/11.
//

#ifndef M_BACK_EVENT_H
#define M_BACK_EVENT_H
#include <sys/epoll.h>
#include <openssl/ssl.h>
#include "buffer.h"
#include "http.h"
#include "timer.h"
//最大同时处理事件数
#define MAXEVENTS 1024

struct m_event {
    int e_pool_fd;
    int event_fd;
};
struct http_client{
    int e_pool_fd;
    int event_fd;
    char *client_ip;
    SSL *ssl;
    //ssl是否链接的标志
    int ssl_connect_flag;
    struct http_request *request;
    struct Buffer* request_data;
    struct http_response *response;
    void *timer;
    void (*handler)(struct http_request* request,struct http_response* response);
};
void ev_loop_init();
void ev_accept_start(int server_fd);
void ev_loop_start();
#endif //M_BACK_EVENT_H
