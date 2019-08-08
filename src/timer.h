//
// Created by oyo on 2019-08-02.
//

#ifndef M_BACK_TIMER_H
#define M_BACK_TIMER_H
#define TIMER_QUEUE_SIZE 10
#define TIMEOUT_DEFAULT 500     /* ms */
#include <stddef.h>
#include "pqueue.h"
extern size_t current_time_millis;
extern pqueue_t *time_pq;
struct timer_node_t
{
    pqueue_pri_t pri;
    size_t pos;
    // 前时间减去最后操作时间小于超时时间，那么只删除节点，不删除客户端
    //deleted==1并且当前时间减去最后操作时间大于超时时，删除节点，并且删除客户端
    int deleted;
    void *value;
};
void timer_init();
void time_update();
int find_timer(int (*call_back)(struct timer_node_t *));
void handle_expire_timers(int (*call_back)(struct timer_node_t*));
void add_timer(void* value,void (*call_back)(void*, struct timer_node_t *));
void delete_timer(void* value,void (*call_back)(void* value));

#endif //M_BACK_TIMER_H
