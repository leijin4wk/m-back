//
// Created by Administrator on 2019/7/25.
//

#ifndef M_BACK_EVENT_PROCESS_H
#define M_BACK_EVENT_PROCESS_H
#include "event.h"

#define SHORTLINE   512
void ev_accept_callback(struct m_event *watcher);

void ev_read_callback(struct m_event* watcher);

void ev_write_callback(struct m_event* watcher);

struct http_client* new_http_client();

void free_http_client(struct http_client* client);

#endif //M_BACK_EVENT_PROCESS_H
