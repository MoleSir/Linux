// 一个错误的示例说明为什么需要线程同步数据
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

// 一个全局变量 
int number = 0;

// 线程函数
void* working(void * arg)
{
    printf("I am a child thread!, ID: %ld\n", pthread_self());

    // 将number累加50次
    for (int i = 0; i < 10; ++i)
    {
        int temp = number;
        temp ++;
        printf("child thread %ld add %d times!, now number is %d\n", pthread_self(), i + 1, temp);
        number = temp;
    }
    
    // 累加完毕，退出线程
    pthread_exit(NULL);
    return NULL;
}

// 主线程
int main()
{
    printf("I am main thread, ID: %ld\n", pthread_self());
    
    // 创建两个子线程执行累加函数
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, working, NULL);
    pthread_create(&tid2, NULL, working, NULL);

    // 主线程等待两个子线程结束
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    // 检查number
    printf("After '20' times add, now number is %d\n", number);
    
    return 0;
}