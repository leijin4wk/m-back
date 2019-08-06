//
// Created by oyo on 2019-08-02.
//

#ifndef M_BACK_TIMER_H
#define M_BACK_TIMER_H
#include <stddef.h>
#include "pqueue.h"
#include "event_process.h"

#define TIMER_QUEUE_SIZE 10
#define TIMEOUT_DEFAULT 500     /* ms */

extern size_t current_msec;

struct timer_node_t
{
    pqueue_pri_t pri;
    struct http_client *client;
    // 前时间减去最后操作时间小于超时时间，那么只删除节点，不删除客户端
    //deleted==1并且当前时间减去最后操作时间大于超时时，删除节点，并且删除客户端
    int deleted;
    size_t pos;
};
void timer_init();
int find_timer();
void add_timer(struct http_client *client);
void delete_timer(struct http_client *client);
void handle_expire_timers();
#endif //M_BACK_TIMER_H
