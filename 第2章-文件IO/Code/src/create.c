#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


int main()
{   
    int fd = open("./data/new_file.txt", O_CREAT|O_RDWR, 0664);
    if(fd == -1)
    {
        printf("open file failed\n");
    }
    else
    {
        printf("create new file!, fd: %d\n", fd);
    }

    close(fd);
    return 0;
}