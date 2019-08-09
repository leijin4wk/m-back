//
// Created by oyo on 2019-08-02.
//

#ifndef M_BACK_TIMER_H
#define M_BACK_TIMER_H
#define TIMER_QUEUE_SIZE 10
#define TIMEOUT_DEFAULT 6000     //http 默认超时时间
#define DEFAULT_EPOOL 500 //epool wait 默认等待时间
#include <stddef.h>
#include "pqueue.h"
extern size_t current_time_millis;
extern pqueue_t *time_pq;
struct timer_node_t
{
    pqueue_pri_t pri;
    size_t pos;
    void *value;
};
void timer_init();
void time_update();
int find_timer();
void handle_expire_timers(int (*call_back)(struct timer_node_t*));
void add_timer(void* value,void (*call_back)(void*, struct timer_node_t *));
void update_time_pri(void* value,struct timer_node_t * (*call_back)(void*));


#endif //M_BACK_TIMER_H
