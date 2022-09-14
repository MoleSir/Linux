#include <stdio.h>
#include <unistd.h>

int main()
{
    // 创建五个子进程
    pid_t pid;
    for (int i = 0; i < 5; ++i)
    {
        pid = fork();
        if (pid == 0)
            break;
    }

    // 父进程
    if(pid > 0)
    {
        // 需要保证父进程一直在运行
        // 一直运行不退出, 并且也做回收, 就会出现僵尸进程
        while(1)
        {
            printf("我是父进程, pid=%d\n", getpid());
            sleep(1);
        }
    }
    else if(pid == 0)
    {
        // 子进程, 执行这句代码之后, 子进程退出了
        printf("我是子进程, pid=%d, 父进程ID: %d\n", getpid(), getppid());
    }
    
    return 0;
}