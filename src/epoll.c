
/**
 * 对epoll api 的封装的方法的具体实现
 */

#include "epoll.h"
#include "dbg.h"

struct epoll_event *events;

int m_epoll_create(int flags) {
    int fd = epoll_create1(flags);
    check(fd > 0, "zv_epoll_create: epoll_create1");

    events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * MAXEVENTS);
    check(events != NULL, "zv_epoll_create: malloc");
    return fd;
}

void m_epoll_add(int epfd, int fd, struct epoll_event *event) {
    int rc = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, event);
    check(rc == 0, "zv_epoll_add: epoll_ctl");
    return;
}

void m_epoll_mod(int epfd, int fd, struct epoll_event *event) {
    int rc = epoll_ctl(epfd, EPOLL_CTL_MOD, fd, event);
    check(rc == 0, "zv_epoll_mod: epoll_ctl");
    return;
}

void m_epoll_del(int epfd, int fd, struct epoll_event *event) {
    int rc = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, event);
    check(rc == 0, "zv_epoll_del: epoll_ctl");
    return;
}

int m_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout) {
    int n = epoll_wait(epfd, events, maxevents, timeout);
    check(n >= 0, "zv_epoll_wait: epoll_wait");
    return n;
}
