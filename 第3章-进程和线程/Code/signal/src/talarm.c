#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
    // 计时1s: 1s 后发送信号中止进程
    alarm(1);
    int i = 0;
    while(1)
    {
        printf("%d\n", i++);
    }
    return 0;
}   