#include <thread_pool.h>
#include <pthread.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


//#define AUTO_COLLECTION


/* add number */
static const int NUMBER = 2;


/**
 * @brief worker thread exit
 * 
 * @param pool the pointer of thread pool
 * @return none 
 */
static void thread_exit(thread_pool_t* pool)
{
    pthread_t tid = pthread_self();
    for (int i = 0; i < pool->max_num; ++i)
    {
        if (pool->workers_id[i] == tid)
        {
            pool->workers_id[i] = 0;
            printf("thread exit call, %ld exiting...\n", tid);
            break;
        }
    }
    pthread_exit(NULL);
}


/**
 * @brief worker thread function
 */
static void* worker_thread_function(void* arg)
{
    // get argument
    thread_pool_t* pool = (thread_pool_t*)arg;

    while (1)
    {
        pthread_mutex_lock(&pool->mutex_pool);

        // check queue empty?
        while (pool->q_size == 0 && !pool->shutdowm)
        {
            // block to wait a task been added
            pthread_cond_wait(&pool->not_empty, &pool->mutex_pool);
            
            // check destroy
            if (pool->exit_num > 0)
            {
                pool->exit_num--;
                if (pool->alive_num > pool->min_num)
                {
                    pool->alive_num--;
                    pthread_mutex_unlock(&pool->mutex_pool);
                    thread_exit(pool);
                }
            }
        }

        // close thread pool?
        if (pool->shutdowm)
        {
            pthread_mutex_unlock(&pool->mutex_pool);
            thread_exit(pool);
        }

        // get a task from task queue
        task_t task;
        task.function = pool->task_queue[pool->q_front].function;
        task.arg = pool->task_queue[pool->q_front].arg;

        // move head
        pool->q_front = (pool->q_front + 1) % pool->q_capacity;
        pool->q_size--;

        // unlock 
        pthread_cond_signal(&pool->not_full);
        pthread_mutex_unlock(&pool->mutex_pool);

        printf("thread %ld start working...\n", pthread_self());
        pthread_mutex_lock(&pool->mutex_busy);
        pool->busy_num++;
        pthread_mutex_unlock(&pool->mutex_busy);

        // execute function
        task.function(task.arg);
#ifdef AUTO_COLLECTION
        free(task.arg);
        task.arg = NULL;
#endif

        printf("thread %ld end working...\n", pthread_self());
        pthread_mutex_lock(&pool->mutex_busy);
        pool->busy_num--;
        pthread_mutex_unlock(&pool->mutex_busy);
    }   
    return NULL;
}


/**
 * @brief 
 * 
 */
static void* manager_thread_function(void* arg)
{
    thread_pool_t* pool = (thread_pool_t*) arg;
    while (!pool->shutdowm)
    {
        // each 3s
        sleep(3);

        // get task number and worker thread number
        pthread_mutex_lock(&pool->mutex_pool);
        int q_size = pool->q_size;
        int alive_num = pool->alive_num;
        pthread_mutex_unlock(&pool->mutex_pool);

        // task busy 
        pthread_mutex_lock(&pool->mutex_busy);
        int busy_num = pool->busy_num;
        pthread_mutex_unlock(&pool->mutex_busy);

        // add new thread ?
        // task number > alive thread number &&
        // alive thread number < max thread number
        if (q_size > alive_num && alive_num < pool->max_num)
        {
            pthread_mutex_lock(&pool->mutex_pool);
            int counter = 0;

            for (int i = 0; i < pool->max_num &&
                            counter < NUMBER &&
                            pool->alive_num < pool->max_num; ++i)
            {
                if (pool->workers_id[i] == 0)
                {
                    pthread_create(pool->workers_id + i, NULL, worker_thread_function, pool);
                    counter++;
                    pool->alive_num++;
                }
            }
            pthread_mutex_unlock(&pool->mutex_pool);
        }

        // destroy thread ?
        // busy number * 2 > alive number &&
        // alive number > pool->min_num
        if (busy_num * 2 < alive_num && alive_num > pool->min_num)
        {
            pthread_mutex_lock(&pool->mutex_pool);
            pool->exit_num = NUMBER;
            pthread_mutex_unlock(&pool->mutex_pool);

            for (int i = 0; i < NUMBER; ++i)
            {
                pthread_cond_signal(&pool->not_empty);
            }
        }
    }
    return NULL;
}


/**
 * @brief create thread pool
 * 
 * @param min lest number of alive worker thread
 * @param max biggest number of alive worker thread
 * @param q_size task queue's capcity
 * @return pointer of a thread pool, while NULL stand for create fail
 */
thread_pool_t* thread_pool_create(int min, int max, int q_size)
{
    // malloc space
    thread_pool_t* pool = (thread_pool_t*)malloc(sizeof(thread_pool_t));
    do
    {
        // malloc failed
        if (pool == NULL)
        {
            printf("malloc thread pool fail...\n");
            break;
        }

        // malloc thread id array
        pool->workers_id = (pthread_t*)malloc(sizeof(pthread_t) * max);
        if (pool->workers_id == NULL)
        {
            printf("malloc thread id fail...\n");
            break;
        }

        // set value
        memset(pool->workers_id, 0, sizeof(pthread_t) * max);
        pool->min_num = min;
        pool->max_num = max;
        pool->busy_num = 0;
        pool->alive_num =  min;
        pool->exit_num = 0;

        // init mutex and cond
        if ( 0 != pthread_mutex_init(&pool->mutex_pool, NULL) ||
             0 != pthread_mutex_init(&pool->mutex_busy, NULL) ||
             0 != pthread_cond_init(&pool->not_empty, NULL) ||
             0 != pthread_cond_init(&pool->not_full, NULL) )
        {
            printf("mutex or condition init fail...\n");
            break;
        }

        // malloc task queue
        pool->task_queue = (task_t*)malloc(sizeof(task_t) * q_size);
        pool->q_size = q_size;
        pool->q_front = 0;
        pool->q_rear = 0;
        
        pool->shutdowm = 0;

        // create threads
        pthread_create(&pool->manager_id, NULL, manager_thread_function, pool);
        for (int i = 0; i < min; ++i)
            pthread_create(&pool->workers_id[i], NULL, worker_thread_function, pool);
    
        return pool;
    } while (0);

    // if some failed, free srouce
    if (pool && pool->workers_id) free(pool->workers_id);
    if (pool && pool->task_queue) free(pool->task_queue);
    if (pool) free(pool);

    return NULL;
}


/**
 * @brief destory a thread pool
 * 
 * @param pool the pointer of a thread pool needed to destory
 * @return 0 stands for destory success while -1 fail  
 */
int thread_pool_destory(thread_pool_t* pool)
{
    // check argment
    if (pool == NULL)
        return -1;
    
    // close pool
    pool->shutdowm = 1;

    // join threads
    pthread_join(pool->manager_id, NULL);
    for (int i = 0; i < pool->alive_num; ++i)
        pthread_cond_signal(&pool->not_empty);

    // free memory
    if (pool->task_queue) free(pool->task_queue);
    if (pool->workers_id) free(pool->workers_id);

    // destroy muext and cond
    pthread_mutex_destroy(&pool->mutex_pool);    
    pthread_mutex_destroy(&pool->mutex_busy);
    pthread_cond_destroy(&pool->not_empty);
    pthread_cond_destroy(&pool->not_full);

    free(pool);
    pool = NULL;

    return 0;
}


/**
 * @brief add new task to thread pool
 * 
 * @param pool the pointer of thread pool
 * @param func task function 
 * @param arg task argment
 * @return  none
 */
void thread_pool_add(thread_pool_t* pool, task_func_t func, void* arg)
{
    // check argment
    if (pool == 0) 
    {
        printf("add fail, empty thread pool!\n");
        return;
    }

    // if task queue is full
    pthread_mutex_lock(&pool->mutex_pool);

    while (pool->q_size == pool->q_capacity && pool->shutdowm == 0)
    {
        // block 
        pthread_cond_wait(&pool->not_full, &pool->mutex_pool);
    }

    // if thread pool close
    if (pool->shutdowm)
    {
        pthread_mutex_unlock(&pool->mutex_pool);
        printf("add file, the thread pool already closed!\n");
        return;
    }

    // add task
    pool->task_queue[pool->q_rear].function = func;
    pool->task_queue[pool->q_rear].arg = arg;
    pool->q_rear = (pool->q_rear + 1) % pool->q_capacity;
    pool->q_size++;

    // new task, call thread
    pthread_cond_signal(&pool->not_empty);

    pthread_mutex_unlock(&pool->mutex_pool);
}


/**
 * @brief get buzy worker thread number
 * 
 * @param pool pointer of thread pool
 * @return buzy thread number, -1 stand for null pool
 */
int thread_pool_buzy_num(thread_pool_t* pool)
{
    // check argment
    if (pool == NULL) 
    {
        printf("add fail, empty thread pool!\n");
        return -1;
    }

    pthread_mutex_lock(&pool->mutex_busy);
    int busy_num = pool->busy_num;
    pthread_mutex_unlock(&pool->mutex_busy);
    return busy_num;
}


/**
 * @brief get alive worker thread number
 * 
 * @param pool pointer of thread pool 
 * @return * alive thread number, -1 stand for null pool
 */
int thread_pool_alive_num(thread_pool_t* pool)
{
    // check argment
    if (pool == NULL) 
    {
        printf("add fail, empty thread pool!\n");
        return -1;
    }

    pthread_mutex_lock(&pool->mutex_pool);
    int alive_num = pool->alive_num;
    pthread_mutex_unlock(&pool->mutex_pool);
    return alive_num;
}