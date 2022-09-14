#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{   
    // 创建新进程
    pid_t pid = fork();
    // 进入子进程
    if (pid == 0)
    {
        // 磁盘上的 /bin/ps
#if 1
        execl("/bin/ps", "title", "aux", NULL);
#else
        execlp("ps", "title", "aux", NULL);
#endif
        // 如果成功当前子进程的代码区别 ps中的代码区代码替换
        // 下面的所有代码都不会执行
        // 如果函数调用失败了,才会继续执行下面的代码
        perror("execl");
        printf("++++++++++++++++++++++++\n");
        printf("++++++++++++++++++++++++\n");
        printf("++++++++++++++++++++++++\n");
        printf("++++++++++++++++++++++++\n");
        printf("++++++++++++++++++++++++\n");
        printf("++++++++++++++++++++++++\n");
    }
    else if (pid > 0)
    {
         printf("我是父进程.....\n");
    }
    
    return 0;
}