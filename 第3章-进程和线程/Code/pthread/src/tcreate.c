// pthread_create.c 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

// 处理函数 void*(*)(void *)
void* working(void * arg)
{
    printf("I am child thread whose thread ID is: %ld\n", pthread_self());
    for (int i = 0; i< 9; ++i)
    {
        printf("i: %d\n", i);
	}
    return NULL;
}

int main()
{
	// 创建子线程
    pthread_t tid;
    pthread_create(&tid, NULL, working, NULL);
    printf("子线程创建成功, 线程ID: %ld\n", tid);
    
    // 子线程不会执行下边的代码, 主线程执行
    printf("我是主线程, 线程ID: %ld\n", pthread_self());
    for(int i=0; i<3; ++i)
    {
        printf("i = %d\n", i);
    }
    
    // 休息, 休息一会儿...
    sleep(1);
    
    return 0;
}