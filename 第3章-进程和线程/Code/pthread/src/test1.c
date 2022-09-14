#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>


void* calc1(void* arg)
{
    printf("I am child1 thread, ID: %ld\n", pthread_self());
    // do 1 sec cal
    sleep(1);
    pthread_exit(NULL);
    return NULL;
}

void* calc2(void* arg)
{
    printf("I am child2 thread, ID: %ld\n", pthread_self());
    // do 2 sec cal
    sleep(2);
    pthread_exit(NULL);
    return NULL;
}


int main()
{
    printf("I am main thread, ID: %ld\n", pthread_self());
    // 创建子线程
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, calc1, NULL);
    pthread_create(&tid2, NULL, calc2, NULL);

    // 等待子线程执行完毕
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    // 完成计算
    printf("all cals are done!\n");
    return 0;
}

// 结果测试，整个程序只要2s，如果不用多线程，需要3s