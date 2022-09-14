// 写实例程序, 给文件描述符追加 O_APPEND
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


int main()
{
    // 不以追加方式打开
    int fd = open("./data/hello2.txt", O_RDWR);
    if (fd == -1)
    {
        perror("open");
        return -1;
    }

    // 获取当前文件状态标志
    int flags = fcntl(fd, F_GETFL);
    // 加上追加标志
    flags = flags | O_APPEND;
    // 修改状态标志
    fcntl(fd, F_SETFL, flags);

    // 写看看
    const char* buf = "yyyyyyyyyyy";
    write(fd, buf, strlen(buf));
    close(fd);


    return 0;
}