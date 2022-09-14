#include <thread_pool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void task_function(void* arg)
{
    int num = *(int*)arg;
    printf("thread %ld is working, number %ld\n", pthread_self(), num);
    sleep(1);
}


int main(void) 
{
    thread_pool_t* pool = thread_pool_create(1, 3, 5);
    int num[100] = {0};
    
    for (int i = 0; i < 100; ++i)
    {
        num[i] += i;
        thread_pool_add(pool, task_function, num + i);
    }

    sleep(30);

    thread_pool_destory(pool);

    return 0;
}