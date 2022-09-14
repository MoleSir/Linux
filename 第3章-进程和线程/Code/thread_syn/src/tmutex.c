// 使用互斥量解决线程同步
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>


// 创建一把互斥锁
// 全局变量, 多个线程共享
pthread_mutex_t mutex;

// 一个全局变量 
int number = 0;

// 线程函数
void* working(void * arg)
{
    printf("I am a child thread!, ID: %ld\n", pthread_self());

    // 将number累加50次
    for (int i = 0; i < 10; ++i)
    {
        // 加锁
        pthread_mutex_lock(&mutex);
        int temp = number;
        temp ++;
        printf("child thread %ld add %d times!, now number is %d\n", pthread_self(), i + 1, temp);
        number = temp;
        // 解锁
        pthread_mutex_unlock(&mutex);
    }
    
    // 累加完毕，退出线程
    pthread_exit(NULL);
    return NULL;
}

// 主线程
int main()
{
    printf("I am main thread, ID: %ld\n", pthread_self());

     // 初始化互斥锁
    pthread_mutex_init(&mutex, NULL);
    
    // 创建两个子线程执行累加函数
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, working, NULL);
    pthread_create(&tid2, NULL, working, NULL);

    // 主线程等待两个子线程结束
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    // 检查number
    printf("After '20' times add, now number is %d\n", number);

    // 销毁互斥锁
    // 线程销毁之后, 再去释放互斥锁
    pthread_mutex_destroy(&mutex);
    
    return 0;
}