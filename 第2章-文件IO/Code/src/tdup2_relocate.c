// 使用dup2 重定向
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main()
{
    // 打开一个文件
    int fd = open("./data/hello2.txt", O_RDWR);
    if (fd == -1)
    {
        perror("open");
        return -1;
    }
    printf("fd: %d\n", fd);

    // 再打开一个
    int fd1 = open("./data/hello1.txt", O_RDWR);
    if (fd1 == -1)
    {
        perror("open");
        return -1;
    }
    printf("fd1: %d\n", fd1);

    // 重定向
    dup2(fd, fd1);

    // 关闭文件描述符
    close(fd);

    //可以使用 fd1 去写了
    const char* buf = "I am back again!";
    write(fd1, buf, strlen(buf));

    close(fd1);

    return 0;
}