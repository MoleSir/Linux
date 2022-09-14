/* mmp in father and child process */
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int main(void)
{
    // open a file
    int fd = open("./data/mem.txt", O_RDWR);
    if (fd == -1)
    {
        perror("open");
        return -1;
    }

    // create mmap
    void* ptr = mmap(NULL, 4000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        return -1;
    }

    // create child
    pid_t pid;
    if ( !(pid = fork()) )
    {
        // child process:
        usleep(1);
        printf("read from mem: %s\n", (char*)ptr);
    }
    else
    {
        // father process
        const char* buf = "I am your father!";
        memcpy(ptr, buf, strlen(buf) + 1);
        wait(NULL);
    }

    // free mmap
    munmap(ptr, 4000);

    return 0;
}