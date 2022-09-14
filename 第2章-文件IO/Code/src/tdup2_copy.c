// 使用dup2 复制文件描述符
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main()
{
    // 打开一个文件
    int fd = open("./data/hello2.txt", O_RDWR|O_CREAT, 0664);
    if (fd == -1)
    {
        perror("open");
        return -1;
    }
    printf("fd: %d\n", fd);

    // 一个没有指向的文件描述符
    int newfd = 1023;

    // 复制文件描述符
    dup2(fd, newfd);

    // 关闭旧的文件描述符
    close(fd);

    // 还能使用
    const char* buf = "I am back!";
    write(newfd, buf, strlen(buf));

    close(newfd);

    return 0;
}