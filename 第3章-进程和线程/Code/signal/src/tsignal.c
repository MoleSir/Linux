#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>


// 回调函数：返回值void 参数int
void callbackFunction(int arg)
{
    printf("当前捕捉到的信号是: %d\n", arg);
}

int main()
{   
    // 注册要捕捉哪一个信号, 执行什么样的处理动作
    signal(SIGALRM, callbackFunction);

    // 设置定时器
    struct itimerval newact;
    // 3s 之后发出第一个信号，之后每1s一个
    newact.it_value.tv_sec = 3;
    newact.it_value.tv_usec = 0;
    newact.it_interval.tv_sec = 1;
    newact.it_interval.tv_usec = 0;
    // 开始倒计时
    setitimer(ITIMER_REAL, &newact, NULL);

    // 循环
    while(1)
    {
        sleep(100000);
    }

    return 0;
}