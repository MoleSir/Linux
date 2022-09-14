// process_loop.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
    for (int i = 0; i < 3; i++)
    {
        pid_t pid = fork();
        printf("current process id: %d\n", getpid());
    }

    return 0;
}