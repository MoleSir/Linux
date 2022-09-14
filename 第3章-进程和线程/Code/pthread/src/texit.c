// pthread_create.c 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>


void* working(void* arg)
{
    sleep(1);
    printf("I am child thread, my threaad id is: %ld\n", pthread_self());
    for (int i = 0; i < 9; ++i)
    {
        // 计数到 i == 6退出
        if (i == 6)
        {
            pthread_exit(NULL);
        }
        printf("child == i: = %d\n", i);
    }
    return NULL;
}


int main()
{
    // 1. 创建一个子线程
    pthread_t tid;
    pthread_create(&tid, NULL, working, NULL);
    printf("子线程创建成功, 线程ID: %ld\n", tid);
    
    // 2. 子线程不会执行下边的代码, 主线程执行
    printf("我是主线程, 线程ID: %ld\n", pthread_self());
    for(int i=0; i<3; ++i)
    {
        printf("i = %d\n", i);
    }

    // 主线程调用退出函数退出, 地址空间不会被释放
    pthread_exit(NULL);

    return 0;
}