
/*
 * Copyright (C) Zhu Jiashun
 * Copyright (C) Zaver
 */

#include "epoll.h"
#include "dbg.h"

struct epoll_event *events;

int zv_epoll_create(int flags) {
    int fd = epoll_create1(flags);
    check_exit(fd > 0, "zv_epoll_create: epoll_create1");

    events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * MAXEVENTS);
    check_exit(events == NULL, "zv_epoll_create faill");
    return fd;
}

void zv_epoll_add(int epfd, int fd, struct epoll_event *event) {
    int rc = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, event);
    check_exit(rc != 0, "zv_epoll_add fial");
    return;
}

void zv_epoll_mod(int epfd, int fd, struct epoll_event *event) {
    int rc = epoll_ctl(epfd, EPOLL_CTL_MOD, fd, event);
    check_exit(rc != 0, "zv_epoll_mod fail");
    return;
}

void zv_epoll_del(int epfd, int fd, struct epoll_event *event) {
    int rc = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, event);
    check_exit(rc != 0, "zv_epoll_del fial");
    return;
}

int zv_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout) {
    int n = epoll_wait(epfd, events, maxevents, timeout);
    check_exit(n < 0, "zv_epoll_wait fail!");
    return n;
}
