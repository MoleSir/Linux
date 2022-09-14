#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>


// 子进程执行函数
void* working(void *arg)
{   
    printf("I am child thread, ID: %ld\n", pthread_self());
    for(int i = 0; i < 9; i++)
    {
        printf("child i = %d\n", i);
    }
    pthread_exit(NULL);
    return 0;
}

int main()
{
    printf("I am main thread, ID: %ld\n", pthread_self());
    // 创建子线程
    pthread_t tid;
    pthread_create(&tid, NULL, working, NULL);
    printf("create a child thread, ID : %ld\n", tid);

    // 分离
    pthread_detach(tid);

    // 主线程exit
    pthread_exit(NULL);

    return 0;
}