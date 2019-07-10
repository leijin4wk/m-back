#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/sysinfo.h>
#include <ev.h>
#include "socket.h"
#include "dbg.h"
#include "epoll.h"
#include "thread_pool.h"
#include "http.h"

#define THREAD_NUM 8
extern struct epoll_event *events;


int main(){

    struct sockaddr_in client_addr;
    // initialize clientaddr and inlen to solve "accept Invalid argument" bug
    socklen_t in_len = 1;

    int listen_fd=create_and_bind(443);

    check_exit(listen_fd<0,"socket 创建和绑定失败！");

    int flag = make_socket_non_blocking(listen_fd);

    check_exit(flag <0, "设置socket为非阻塞失败！");

    m_thread_pool_t *tp = thread_pool_init(THREAD_NUM);

    check_exit(tp == NULL, "thread_pool_init error");

    int epoll_fd = m_epoll_create(0);

    check_exit(epoll_fd <0, "创建epoll失败！");

    struct epoll_event event;
    event.data.fd = listen_fd;
    //读入,边缘触发方式
    event.events = EPOLLIN | EPOLLET;

    m_epoll_add(epoll_fd, listen_fd, &event);

    while (1) {
        int n = m_epoll_wait(epoll_fd, events, MAXEVENTS, -1);
        for (int i = 0; i < n; i++){
            if ((events[i].events & EPOLLERR) ||(events[i].events & EPOLLHUP) ||(!(events[i].events & EPOLLIN))){
                fprintf (stderr, "epoll error\n");
                close (events[i].data.fd);
                continue;
            } else if (listen_fd == events[i].data.fd){
                while(1) {
                    int socket_in_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &in_len);
                    if (socket_in_fd < 0) {
                        if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                            break;
                        } else {
                            log_err("accept fail！");
                            break;
                        }
                    }
                    int rc = make_socket_non_blocking(socket_in_fd);
                    check(rc != 0, "accept make_socket_non_blocking fail!");
                    log_info("new connection fd %d", socket_in_fd);
                    event.data.fd = socket_in_fd;
                    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
                    m_epoll_add(epoll_fd, socket_in_fd, &event);
                }
            }else{
                log_info("new data from fd %d", events[i].data.fd);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-conversion"
                int rc = thread_pool_add(tp, handler_request, &events[i].data.fd);
#pragma GCC diagnostic pop
                check(rc != 0, "thread_pool_add");
            }
        }
    }
    return 0;
}