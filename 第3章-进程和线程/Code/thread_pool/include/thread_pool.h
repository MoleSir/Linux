#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <pthread.h>


/* work function type */
typedef void (* task_func_t)(void*);

/* task struct */
typedef struct TASK
{
    task_func_t function;
    void* arg;
} task_t;


/* thread pool struct */
typedef struct THREAD_POOL
{
    task_t* task_queue;
    int q_capacity;
    int q_size;
    int q_front;
    int q_rear;

    pthread_t manager_id;
    pthread_t* workers_id;
    int min_num;
    int max_num;
    int busy_num;
    int alive_num;
    int exit_num;
    
    pthread_mutex_t mutex_pool;
    pthread_mutex_t mutex_busy;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;

    int shutdowm;

} thread_pool_t;


/* thread pool functions */
/* create and initial thread pool */
thread_pool_t* thread_pool_create(int min, int max, int q_size);

/* destroy thread pool */
int thread_pool_destory(thread_pool_t* pool);

/* add task for pool */
void thread_pool_add(thread_pool_t* pool, task_func_t func, void* arg);

/* get working thread number */
int thread_pool_buzy_num(thread_pool_t* pool);

/* get live thread number */
int thread_pool_alive_num(thread_pool_t* pool);


#endif