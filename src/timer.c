//
// Created by oyo on 2019-08-02.
//
#include <sys/time.h>
#include "timer.h"
#include "dbg.h"

//用来存储系统当前毫秒数
size_t current_time_millis;
//全局的时间优先级队列
pqueue_t *time_pq;
//用来操作优先级队列的互斥锁
static pthread_mutex_t timer_mutex;

static int cmp_pri(pqueue_pri_t next, pqueue_pri_t curr);

static pqueue_pri_t get_pri(void *a);

static void set_pri(void *a, pqueue_pri_t pri);

static size_t get_pos(void *a);

static void set_pos(void *a, size_t pos);

void timer_init() {
    time_pq = p_queue_init(TIMER_QUEUE_SIZE, cmp_pri, get_pri, set_pri, get_pos, set_pos);
    if (!time_pq){
        log_err("time_pq init fail!");
         exit(1);
    }
    time_update();
    /*互斥初始化*/
    pthread_mutex_init (&timer_mutex, NULL);
}
int find_timer(){
    struct timer_node_t *timer_node;
    int time = -1;
    pthread_mutex_lock(&timer_mutex);
    while (p_queue_size(time_pq)>1) {
        time_update();
        timer_node = (struct timer_node_t *)p_queue_peek(time_pq);
        if(timer_node==NULL){
            log_err("pqueue_peek get node fail!");
        }
        if (timer_node->deleted) {
            timer_node=p_queue_pop(time_pq);
            free(timer_node);
            continue;
        }

        time = (int) (timer_node->pri - current_time_millis);
        time = (time > 0? time: -1);
        break;
    }
    pthread_mutex_unlock(&timer_mutex);
    log_info("time: %d",time);
    return time;
}

void add_timer(void* value,void (*call_back)(void*, struct timer_node_t *)){
    log_info("add_timer start!");
    int rc;
    void (*function)(void*,struct timer_node_t*);
    struct timer_node_t *timer_node = (struct timer_node_t *)malloc(sizeof(struct timer_node_t));
    if(timer_node==NULL){
        log_err("timer_node malloc fail!");
        exit(1);
    }
    pthread_mutex_lock(&timer_mutex);
    time_update();
    rc = p_queue_insert(time_pq, timer_node);
    if (rc!=0){
        log_err("pqueue_insert fail!");
        exit(1);
    }
    function=call_back;
    function(value,timer_node);
    pthread_mutex_unlock(&timer_mutex);
    log_info("add_timer end!");
}
void handle_expire_timers(void (*call_back)(struct timer_node_t *)){
    log_info("handle_expire_timers start!");
    void (*function)(struct timer_node_t*)=call_back;
    pthread_mutex_lock(&timer_mutex);
    while (p_queue_size(time_pq)>1) {
        log_info("handle_expire_timers loop!");
        time_update();
        struct timer_node_t * timer_node = (struct timer_node_t *)(p_queue_peek(time_pq));
        if(timer_node==NULL){
            log_err("timer_node malloc fail!");
            exit(1);
        }
        if (timer_node->pri > current_time_millis) {
            pthread_mutex_unlock(&timer_mutex);
            return;
        }
        log_info("%ld    %ld",(long)timer_node->pri,current_time_millis);
        function(timer_node);
    }
    pthread_mutex_unlock(&timer_mutex);
    log_info("handle_expire_timers end!");
}

void delete_timer(void* value,void (*call_back)(void*)){
    void (*function)(void*);
    function=call_back;
    function(value);
}



void time_update(){
    // there is only one thread calling zv_time_update, no need to lock?
    struct timeval tv;
    int rc;

    rc = gettimeofday(&tv, NULL);
    if(rc<0){
      log_err("get time fail!");
    }
    current_time_millis = tv.tv_sec * 1000 + tv.tv_usec / 1000;
}


static int cmp_pri(pqueue_pri_t next, pqueue_pri_t curr)
{
    return (next > curr);
}


static pqueue_pri_t get_pri(void *a)
{
    return ((struct timer_node_t *) a)->pri;
}


static void set_pri(void *a, pqueue_pri_t pri)
{
    ((struct timer_node_t *) a)->pri = pri;
}


static size_t get_pos(void *a)
{
    return ((struct timer_node_t *) a)->pos;
}


static void set_pos(void *a, size_t pos) {
    ((struct timer_node_t *) a)->pos = pos;
}
