//
// Created by Administrator on 2019/7/11.
//
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include<unistd.h>
#include <netdb.h>
#include "http_parser.h"
#include "event.h"
#include "socket_tool.h"
#include "dbg.h"
#include "event_process.h"


struct epoll_event *events;

int e_pool_fd;

static int socket_accept_fd;

static struct m_event* new_m_event();

static struct m_event* new_m_event(){
    struct m_event *event=malloc(sizeof(struct m_event));
    if (event==NULL){
       log_err("create m_event fail!");
        return NULL;
    }
    event->event_fd=-1;
    return event;
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
                    (events[i].events & EPOLLHUP)) {
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
