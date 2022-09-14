#include <unistd.h>
#include <stdio.h>


int main()
{
    // 创建新进程
    pid_t pid = fork();

    if (pid > 0)
    {
        // 父进程
         printf("我是父进程, pid=%d\n", getpid());
    }
    else if (pid == 0)
    {
        // 子进程
        sleep(1);	// 强迫子进程睡眠1s, 这个期间, 父进程退出, 当前进程变成了孤儿进程
        // 子进程
        printf("我是子进程, pid=%d, 父进程ID: %d\n", getpid(), getppid());
    }
    
    return 0;
}