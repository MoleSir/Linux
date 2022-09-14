#include <stdio.h>
#include <unistd.h>

int main()
{
    pid_t pid;
    for(int i = 0; i < 3; i++)
    {
        pid = fork();
        if (pid == 0)
            break;
    }
    printf("current process id: %d\n", getpid());
    return 0;
}