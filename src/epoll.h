
/**
 * 对epoll api 的封装
 */
#ifndef M_BACK_EPOLL_H
#define M_BACK_EPOLL_H

#include <sys/epoll.h>

#define MAXEVENTS 1024

int m_epoll_create(int flags);

void m_epoll_add(int epfd, int fs, struct epoll_event *event);

void m_epoll_mod(int epfd, int fs, struct epoll_event *event);

void m_epoll_del(int epfd, int fs, struct epoll_event *event);

int m_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);

#endif
