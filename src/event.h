//
// Created by Administrator on 2019/7/11.
//

#ifndef M_BACK_EVENT_H
#define M_BACK_EVENT_H
#include <ev.h>
void ev_loop_init();
void ev_accept_start(int server_fd);
void ev_read_cb(struct ev_loop *loop,struct ev_io *watcher,int revents);
void ev_loop_start();
#endif //M_BACK_EVENT_H
