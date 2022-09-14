// 测试读写锁
// 要求：8 个线程操作同一个全局变量，3 个线程不定时写同一全局资源，5 个线程不定时读同一全局资源。
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>


// 全局变量
int number = 0;

// 读写锁
pthread_rwlock_t rwlock;

// 写数据线程函数
void* writeNum(void* arg)
{
    while(1)
    {
        // 上写锁
        pthread_rwlock_wrlock(&rwlock);
        // 加1
        int temp = number;
        temp ++;
        number = temp;
        printf("++operation finished, number : %d, tid = %ld\n", number, pthread_self());
        // 解锁
        pthread_rwlock_unlock(&rwlock);
        // 等待
        usleep(rand() % 100);
    }

    return NULL;
}

// 读数据线程函数
void* readNum(void* arg)
{
    while(1)
    {
        // 上读锁
        pthread_rwlock_rdlock(&rwlock);
        printf("--operation finished, number : %d, tid = %ld\n", number, pthread_self());
        // 解锁
        pthread_rwlock_unlock(&rwlock);
        // 等待
        usleep(rand() % 100);
    }

    return NULL;
}


int main()
{
    // 初始化锁
    pthread_rwlock_init(&rwlock, NULL);

    // 创建线程
    pthread_t wrtid[3];
    pthread_t rdtid[5];
    for (int i = 0; i < 3; ++i)
    {
        pthread_create((wrtid + i), NULL, writeNum, NULL);
    }
    for (int i = 0; i < 5; ++i)
    {
        pthread_create((rdtid + i), NULL, readNum, NULL);
    }

    // join
    for (int i = 0; i < 3; ++i)
    {
        pthread_join(wrtid[i], NULL);
    }
    for (int i = 0; i < 5; ++i)
    {
        pthread_join(rdtid[i], NULL);
    }

    // 销毁锁
    pthread_rwlock_destroy(&rwlock);

    return 0;
}