#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main()
{
    int fd = open("data/abcd.txt", O_RDONLY);
    if (fd == -1)
    {
        printf("open failed!\n");
    }
    else 
    {
        printf("open abcd.txt\n");
    }

    
    close(fd);
    return 0;
}