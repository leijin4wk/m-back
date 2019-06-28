#include <stdio.h>
#include <unistd.h>
#include "socket.h"
#include "dbg.h"
#include "epoll.h"

extern struct epoll_event *events;

int main() {

#ifdef USE_A
    printf("USE_A\n");
#else
    printf("NOT USE_A\n");
#endif

#ifdef USE_B
    printf("USE_B\n");
#else
    printf("NOT USE_B\n");
#endif

    int listen_fd=create_and_bind(8888);
    check_exit(listen_fd<0,"socket 创建和绑定失败！");

    int flag = make_socket_non_blocking(listen_fd);

    check_exit(flag <0, "设置socket为非阻塞失败！");

    int epoll_fd = m_epoll_create(0);

    check_exit(epoll_fd <0, "创建epoll失败！");

    struct epoll_event event;
    event.data.fd = listen_fd;
    //读入,边缘触发方式
    event.events = EPOLLIN | EPOLLET;

    m_epoll_add(epoll_fd, listen_fd, &event);

//    while (1) {
//        int n = m_epoll_wait(epoll_fd, events, MAXEVENTS, -1);
//        for (int i = 0; i < n; i++){
//
//            if ((events[i].events & EPOLLERR) ||(events[i].events & EPOLLHUP) ||(!(events[i].events & EPOLLIN))){
//                fprintf (stderr, "epoll error\n");
//                close (events[i].data.fd);
//                continue;
//            } else if (listen_fd == events[i].data.fd){
//                printf("我来了！\n");
//            }else{
//                printf("哈哈哈！\n");
//            }
//
//        }
//    }

    printf("aaa\n");
    return 0;
}