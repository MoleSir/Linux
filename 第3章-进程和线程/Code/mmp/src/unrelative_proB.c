#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>


int main(void)
{
    // open file
    int fd = open("./data/mem.txt", O_RDWR);
    if (fd == -1)
    {
        perror("open");
        exit(0);
    }

    // create mmap
    void* ptr = mmap(NULL, 4000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(0);
    }

    const char* message = "============I am your father=============";
    memcpy(ptr, message, strlen(message));

    // free mmap
    munmap(ptr, 4000);

    return 0;
}