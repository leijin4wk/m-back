//
// Created by oyo on 2019-08-02.
//
#include "timer.h"
#include "dbg.h"
//用来存储系统当前毫秒数
size_t current_time_millis;
//全局的时间优先级队列
pqueue_t *time_pq;
//用来操作优先级队列的互斥锁
static pthread_mutex_t timer_mutex;

static void time_update();

static int cmp_pri(pqueue_pri_t next, pqueue_pri_t curr);

static pqueue_pri_t get_pri(void *a);

static void set_pri(void *a, pqueue_pri_t pri);

static size_t get_pos(void *a);

static void set_pos(void *a, size_t pos);

void timer_init() {
    time_pq = pqueue_init(TIMER_QUEUE_SIZE, cmp_pri, get_pri, set_pri, get_pos, set_pos);
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
    while (pqueue_size(time_pq)>0) {
        time_update();
        timer_node = (struct timer_node_t *)pqueue_peek(time_pq);
        if(timer_node==NULL){
            log_err("pqueue_peek get node fail!");
        }
        if (timer_node->deleted) {
            timer_node=pqueue_pop(time_pq);
            free(timer_node);
            continue;
        }

        time = (int) (timer_node->pri - current_time_millis);
        time = (time > 0? time: 0);
        break;
    }
    pthread_mutex_unlock(&timer_mutex);
    return time;
}

void add_timer(struct http_client *client){
    int rc;
    struct timer_node_t *timer_node = (struct timer_node_t *)malloc(sizeof(struct timer_node_t));
    if(timer_node==NULL){
        log_err("timer_node malloc fail!");
        exit(1);
    }
    pthread_mutex_lock(&timer_mutex);
    time_update();
    client->last_update_time=current_time_millis;
    timer_node->client = client;
    timer_node->deleted=0;
    timer_node->pri = current_time_millis + TIMEOUT_DEFAULT;
    rc = pqueue_insert(time_pq, timer_node);
    if (rc!=0){
        log_err("pqueue_insert fail!");
        exit(1);
    }
    client->timer=timer_node;
    pthread_mutex_unlock(&timer_mutex);
}
void handle_expire_timers(){
    struct timer_node_t *timer_node;
    struct http_client * client;
    pthread_mutex_lock(&timer_mutex);
    while (pqueue_size(time_pq)>0) {
        time_update();
        timer_node = (struct timer_node_t *)pqueue_peek(time_pq);
        if(timer_node==NULL){
            log_err("timer_node malloc fail!");
            exit(1);
        }
        client=timer_node->client;
        if (timer_node->deleted) {
            timer_node=pqueue_pop(time_pq);
            free(timer_node);
            //如果客户端最后更新时间超过超时，删除客户端
            if(current_time_millis-client->last_update_time>TIMEOUT_DEFAULT){
                free_http_client(client);
            }
        }else{
            //如果客户端最后更新时间超过5倍超时，删除客户端
            if(current_time_millis-client->last_update_time>TIMEOUT_DEFAULT*5){
                timer_node=pqueue_pop(time_pq);
                free(timer_node);
                free_http_client(client);
            }
        }
        if (timer_node->pri > current_time_millis) {
            return;
        }
    }
    pthread_mutex_unlock(&timer_mutex);
}

void delete_timer(struct http_client *client){
    client->timer->deleted=1;
}



static void time_update(){
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
