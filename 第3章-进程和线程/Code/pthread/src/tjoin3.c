// 子线程使用主线程的栈区空间
// pthread_join.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

// 定义一个结构体
typedef struct 
{
    int id;
    char name[16];
    int age;
} person_t;


// 处理函数
void* working(void* arg)
{
    // 打印线程id
    printf("I am child thread, my thread ID is: %ld\n", pthread_self());
    
    // 改造指针类型
    person_t* p = (person_t*)arg;

    // 赋值一个人
    p->id = 1;
    p->age = 18;
    strcpy(p->name, "tom");

    // 退出并将数据地址传递出去
    pthread_exit(p);

    // 实际不会执行return
    return NULL;
}

int main()
{
    // 打印线程id
    printf("I am main thread, my thread ID is: %ld\n", pthread_self());

    // 创建一个人
    person_t p;

    // 创建子线程
    pthread_t tid;
    // 使用创建线程函数 pthread_create 的最后一个参数，将p的地址传入
    pthread_create(&tid, NULL, working, &p);
    printf("create a child thread, thread ID: %ld\n", tid);

    // 阻塞等待子线程退出
    void* ptr = NULL;
    pthread_join(tid, &ptr);

    // 打印信息
    person_t* pp = (person_t*)ptr;
    printf("get data form child thread: name: %s, age: %d, id: %d\n", pp->name, pp->age, pp->id);
    printf("get data form child thread: name: %s, age: %d, id: %d\n", p.name, p.age, p.id);

    return 0;
}