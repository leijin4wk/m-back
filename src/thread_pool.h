
#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include "dbg.h"

typedef struct m_task_s {
    void (*func)(void *);
    void *arg;
    struct m_task_s *next;
} m_task_t;

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    pthread_t *threads;
    m_task_t *head;
    int thread_count;
    int queue_size;
    int shutdown;
    int started;
} m_thread_pool_t;

typedef enum {
    m_tp_invalid   = -1,
    m_tp_lock_fail = -2,
    m_tp_already_shutdown  = -3,
    m_tp_cond_broadcast    = -4,
    m_tp_thread_fail       = -5,
    
} m_thread_pool_error_t;

m_thread_pool_t * thread_pool_init(int thread_num);

int thread_pool_add(m_thread_pool_t *pool, void (*func)(void *), void *arg);

int thread_pool_destroy(m_thread_pool_t *pool, int gracegul);

#endif
