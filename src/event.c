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
#include "socket_tool.h"
#include "dbg.h"
#include "event.h"
#include "event_process.h"
#include "thpool.h"
threadpool read_thread_pool;
threadpool write_thread_pool;
struct epoll_event *events;

int e_pool_fd;

static int socket_accept_fd;

static struct m_event* new_m_event();
static int handle_expire_timers_call_back(struct timer_node_t *node);
static int handle_expire_timers_call_back(struct timer_node_t *node) {
    struct http_client *http_client = (struct http_client *) node->value;
    long a=current_time_millis - node->pri;
    if (a > TIMEOUT_DEFAULT) {
        log_info("删除SSL连接:%d ,ip:%s , 当前连接人数%d", http_client->event_fd, http_client->client_ip, --total_clients);
        p_queue_pop(time_pq);
        node->value=NULL;
        free(node);
        free_http_client(http_client);
        return -1;
    }
    return 1;

}
static struct m_event* new_m_event(){
    struct m_event *event=malloc(sizeof(struct m_event));
    if (event==NULL){
       log_err("create m_event fail!");
        return NULL;
    }
    event->event_fd=-1;
    event->e_pool_fd=-1;
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
    accept_event->e_pool_fd=e_pool_fd;
    event.data.ptr = (void *)accept_event;
    //采用默认触发方式（水平触发）EPOLLLT  https://blog.csdn.net/zxm342698145/article/details/80524331 这篇文章给了很大的帮助
    event.events = EPOLLIN ;
    int rc = epoll_ctl(accept_event->e_pool_fd, EPOLL_CTL_ADD, server_fd, &event);
    if (rc != 0){
        log_err("accept_fd epoll_add fail!");
    }
}
void ev_loop_start(){
    log_info("server loop started.");
    timer_init();
    int i,n;
    int time;
    int flag=1;
    while (flag) {
        //获取最小超时时间
        time=find_timer();
        n = epoll_wait(e_pool_fd, events, MAXEVENTS, time);
        //处理超时事件
        handle_expire_timers(handle_expire_timers_call_back);
        for (i = 0; i < n; i++) {
            struct m_event *r = (struct m_event *)events[i].data.ptr;
            if(r->event_fd==socket_accept_fd){
                ev_accept_callback(r);
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
                  int res= thpool_add_work(read_thread_pool, ev_read_callback,(void*)r);
                  if (res<0){
                      log_err("fd:%d 添加读线程失败",r->event_fd);
                  }
                }else if(events[i].events&EPOLLOUT) //有数据待发送，写socket
                {
                    int res=  thpool_add_work(write_thread_pool, ev_write_callback,(void*)r);
                    if (res<0){
                        log_err("fd:%d 添加写线程失败",r->event_fd);
                    }
                }else{
                    log_err("未知的事件:%d",events[i].events);
                }
            }
        }
    }
}
