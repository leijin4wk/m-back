//
// Created by Administrator on 2019/7/25.
//

#ifndef M_BACK_EVENT_PROCESS_H
#define M_BACK_EVENT_PROCESS_H
#include "event.h"

void ev_accept_callback(int e_pool_fd,struct m_event *watcher);

void ev_read_callback(int e_pool_fd,struct m_event* watcher);

void ev_write_callback(int e_pool_fd,struct m_event* watcher);

struct http_client* new_http_client();

void free_http_client(struct http_client* client);
#endif //M_BACK_EVENT_PROCESS_H
