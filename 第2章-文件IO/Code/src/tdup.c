#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


int main()
{
    // 1. 创建一个新的磁盘文件
    int fd = open("./data/hello1.txt", O_RDWR|O_CREAT, 0664);
    if(fd == -1)
    {
        perror("open");
        exit(0);
    }
    printf("fd: %d\n", fd);

    // 写数据
    const char* pt = "hello world!";
    // 写成功之后, 文件指针在文件尾部
    write(fd, pt, strlen(pt));

    // 复制这个文件描述符 fd
    int newfd = dup(fd);
    printf("newfd: %d\n", newfd);

    // 关闭旧的文件描述符
    close(fd);

    // 使用新的文件描述符继续写文件
    const char* ppt = "I am still here!";
    write(newfd, ppt, strlen(ppt));
    close(newfd);


    return 0;
}