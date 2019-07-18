//
// Created by Administrator on 2019/7/11.
//
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <zconf.h>
#include <netdb.h>
#include "event.h"
#include "socket_tool.h"
#include "http.h"
#include "dbg.h"


int total_clients=0;

struct epoll_event *events;

int e_pool_fd;

static int socket_accept_fd;

static void ev_accept_callback(int e_pool_fd,struct m_event *watcher,int events);

void ev_loop_init(){
    e_pool_fd = epoll_create1(0);
    check_exit(e_pool_fd < 0, "epoll_create fail!");
    events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * MAXEVENTS);
    check_exit(events == NULL, "epoll_event_create faill");
}
void ev_accept_start(int server_fd){
    struct epoll_event event;
    socket_accept_fd=server_fd;
    int flag=set_nonblock(server_fd);
    check(flag== 0,"make server_fd non_blocking")
    struct m_event *accept_event=malloc(sizeof(struct m_event));
    check_exit(accept_event==NULL,"malloc m_event fail!");
    accept_event->event_fd=socket_accept_fd;
    event.data.ptr = (void *)accept_event;
    event.events = EPOLLIN | EPOLLET;
    int rc = epoll_ctl(e_pool_fd, EPOLL_CTL_ADD, server_fd, &event);
    check(rc == 0, "accept_fd add epoll_event!");
}

static void ev_accept_callback(int e_pool_fd,struct m_event *watcher,int events)
{
    struct sockaddr_in in_addr;
    socklen_t in_len;
    memset(&in_addr, 0, sizeof(struct sockaddr_in));
    int in_fd;
    while(1) {
        in_fd = accept(watcher->event_fd,&in_addr, &in_len);
        if (in_fd < 0) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                /* we have processed all incoming connections */
                break;
            } else {
                log_err("accept error！");
                break;
            }
        }
        char *ip=inet_ntoa(in_addr.sin_addr);
        log_info("connection from %s",ip);
        char*client_ip=malloc(sizeof(ip)+1);
        strcpy(client_ip,ip);
        int flag = set_nonblock(in_fd);
        check(flag == 0, "make_socket_non_blocking");
        log_info("new connection fd %d", in_fd);
        struct epoll_event event;
        struct http_client * client=malloc(sizeof(struct http_client));
        client->event_fd=in_fd;
        client->client_ip=client_ip;
        event.data.ptr = (void *)client;
        event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
        int rc = epoll_ctl(e_pool_fd, EPOLL_CTL_ADD, in_fd, &event);
        check_exit(rc != 0, "zv_epoll_add fail");
        total_clients++;
        log_info("当前连接人数%d",total_clients);
    }
    // end of while of accept
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
                ev_accept_callback(e_pool_fd,r,events[i].events);
            }else{
                if ((events[i].events & EPOLLERR) ||
                    (events[i].events & EPOLLHUP) ||
                    (!(events[i].events & EPOLLIN))) {
                    log_err("epoll error fd: %d", r->event_fd);
                    close(r->event_fd);
                    free(r);
                    continue;
                }
                handler_request(events[i].data.ptr);
            }
        }
    }
}
