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
    int time = DEFAULT_EPOOL;
    pthread_mutex_lock(&timer_mutex);
    if (p_queue_size(time_pq)>0) {
        time_update();
        timer_node = (struct timer_node_t *)p_queue_peek(time_pq);
        if(timer_node==NULL){
            log_err("pqueue_peek get node fail!");
        }
        time = (int) (timer_node->pri - current_time_millis);
        time = (time > 0? time: DEFAULT_EPOOL);
    }
    pthread_mutex_unlock(&timer_mutex);
    return time;
}
void handle_expire_timers(int (*call_back)(struct timer_node_t *)){
    int (*function)(struct timer_node_t*);
    pthread_mutex_lock(&timer_mutex);
    while (p_queue_size(time_pq)>0) {
        struct timer_node_t * timer_node = (struct timer_node_t *)(p_queue_peek(time_pq));
        if(timer_node==NULL){
            log_err("timer_node malloc fail!");
            exit(1);
        }
        time_update();
        function=call_back;
        if(function(timer_node)<0){
            continue;
        }else{
            pthread_mutex_unlock(&timer_mutex);
            return;
        }
    }
    pthread_mutex_unlock(&timer_mutex);
}

void add_timer(void* value,void (*call_back)(void*, struct timer_node_t *)){
    int rc;
    void (*function)(void*,struct timer_node_t*);
    struct timer_node_t *timer_node = (struct timer_node_t *)malloc(sizeof(struct timer_node_t));
    if(timer_node==NULL){
        log_err("timer_node malloc fail!");
        exit(1);
    }
    pthread_mutex_lock(&timer_mutex);
    time_update();
    timer_node->pri = current_time_millis + TIMEOUT_DEFAULT;
    timer_node->value=value;
    rc = p_queue_insert(time_pq, timer_node);
    if (rc!=0){
        log_err("pqueue_insert fail!");
        exit(1);
    }
    function=call_back;
    function(value,timer_node);
    pthread_mutex_unlock(&timer_mutex);
}

void update_time_pri(void* value,struct timer_node_t * (*call_back)(void*)){
    struct timer_node_t * (*function)(void*);
    pthread_mutex_lock(&timer_mutex);
    time_update();
    function=call_back;
    struct timer_node_t * node=function(value);
    pqueue_change_priority(time_pq,current_time_millis + TIMEOUT_DEFAULT,node);
    pthread_mutex_unlock(&timer_mutex);
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
