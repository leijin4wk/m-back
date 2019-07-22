//
// Created by Administrator on 2019/7/11.
//
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <zconf.h>
#include <netdb.h>
#include <http_parser.h>
#include "event.h"
#include "socket_tool.h"
#include "ssl_tool.h"
#include "dbg.h"

static struct http_client* new_http_client();
static void free_http_client(struct http_client* client);

int total_clients=0;

struct epoll_event *events;

int e_pool_fd;

static int socket_accept_fd;

static struct m_event* new_m_event();

static void ev_accept_callback(int e_pool_fd,struct m_event *watcher);

static void ev_read_callback(int e_pool_fd,struct m_event* watcher);

static void ev_write_callback(int e_pool_fd,struct m_event* watcher);

static struct m_event* new_m_event(){
    struct m_event *event=malloc(sizeof(struct m_event));
    if (event==NULL){
       log_err("create m_event fail!");
        return NULL;
    }
    event->event_fd=-1;
    event->client_ip=NULL;
    event->ssl=NULL;
    return event;
}
static struct http_client* new_http_client(){
    struct http_client *client = malloc(sizeof(struct http_client));
    if(client==NULL){
        log_err("new http client fail!");
        return NULL;
    }

    client->response=NULL;
    client->request=NULL;
    client->request_data=NULL;
    client->response_data=new_buffer(MAX_LINE, MAX_RESPONSE_SIZE);
    return client;
}

static void free_http_client(struct http_client* client){
    //todo
}

void ev_loop_init(){
    e_pool_fd = epoll_create1(0);
    if (e_pool_fd<0){
        log_err("epoll_create fail!");
        exit(-1);
    }
    events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * MAXEVENTS);
    if (events==NULL){

    }
    if (events == NULL){
        log_err("epoll_event_create faill");
        exit(-1);
    }
}
void ev_accept_start(int server_fd){
    struct epoll_event event;
    socket_accept_fd=server_fd;
    int flag=set_nonblock(server_fd);
    if(flag<0){
        log_err("make_socket_non_blocking fail!");
        exit(1);
    }
    struct m_event *accept_event=new_m_event();
    if(accept_event==NULL){
        log_err("malloc m_event fail!");
        exit(1);
    }
    accept_event->event_fd=socket_accept_fd;
    event.data.ptr = (void *)accept_event;
    event.events = EPOLLIN | EPOLLET;
    int rc = epoll_ctl(e_pool_fd, EPOLL_CTL_ADD, server_fd, &event);
    if (rc != 0){
        log_err("accept_fd epoll_add fail!");
    }
}

static void ev_accept_callback(int e_pool_fd,struct m_event *watcher)
{
    struct sockaddr_in in_addr;
    socklen_t in_len;
    memset(&in_addr, 0, sizeof(struct sockaddr_in));
    int in_fd;
    while(1) {
        in_fd = accept(watcher->event_fd, &in_addr, &in_len);
        if (in_fd < 0) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                /* we have processed all incoming connections */
                break;
            } else {
                log_err("accept error！");
                break;
            }
        }
        int flag = set_nonblock(in_fd);
        if (flag < 0) {
            log_err("make_socket_non_blocking fail!");
            continue;
        }
        struct epoll_event event;
        struct http_client *client = new_http_client();
        char *ip = inet_ntoa(in_addr.sin_addr);
        alloc_cpy(client->client_ip,ip,strlen(ip))
        client->event_fd = in_fd;
        SSL *ssl=create_ssl(client->event_fd);
        if (ssl == NULL) {
            log_err("create_ssl fail!");
            free_http_client(client);
            continue;
        }
        client->ssl=ssl;
        event.data.ptr = (void *) client;
        event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
        int rc = epoll_ctl(e_pool_fd, EPOLL_CTL_ADD, in_fd, &event);
        if (rc != 0) {
            log_err("epoll_add fail!");
            free_http_client(client);
            continue;
        }
        total_clients++;
        log_info("添加SSL连接:%d ,ip:%s , 当前连接人数%d", in_fd, ip, total_clients);
    }
}

static void ev_read_callback(int e_pool_fd,struct m_event* watcher){
    struct http_client* client= (struct http_client *)watcher;
    struct Buffer* read_buff=new_buffer(MAX_LINE, MAX_REQUEST_SIZE);
    int res=ssl_read(client->ssl,read_buff);
    if(res<0){
        free_http_client(client);
        return;
    }
    client->request_data=read_buff;
    struct http_parser* http_parser=parser_http_request_buffer(client->request_data);
    client->request=(struct http_request *)http_parser->data;
    log_info("http parser complete!");
}

static void ev_write_callback(int e_pool_fd,struct m_event* watcher){
    int res=0;
    struct http_client* client= (struct http_client *)watcher;
    struct Buffer* read_buff=create_http_response_buffer(client->response);
    res=ssl_write(client->ssl,read_buff);
    if(res<0){
        free_http_client(client);
        return;
    }
    log_info("http send complete!");
}

void ev_loop_start(){
    log_info("server loop started.");
    int i,n;
    int time=10;
    int flag=1;
    while (flag) {
        n = epoll_wait(e_pool_fd, events, MAXEVENTS, time);
        for (i = 0; i < n; i++) {
            struct m_event *r = (struct m_event *)events[i].data.ptr;
            if(r->event_fd==socket_accept_fd){
                ev_accept_callback(e_pool_fd,r);
            }else{
                if ((events[i].events & EPOLLERR) ||
                    (events[i].events & EPOLLHUP) ||
                    (!(events[i].events & EPOLLIN))) {
                    log_err("epoll error fd: %d", r->event_fd);
                    close(r->event_fd);
                    free(r);
                    continue;
                }
                else if(events[i].events&EPOLLIN )//有数据可读，写socket
                {
                    ev_read_callback(e_pool_fd,r);

                }else if(events[i].events&EPOLLOUT) //有数据待发送，写socket
                {
                    ev_write_callback(e_pool_fd,r);
                }else{
                    log_err("未知的事件:%d",events[i].events);
                }
            }
        }
    }
}

int process(struct http_client *client){
    //TODO new response
}
