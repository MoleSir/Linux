// 文件的拷贝
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main()
{
    // 1. 打开存在的文件english.txt, 读这个文件
    int src_fd = open("./data/src_file.txt", O_RDONLY);
    if(src_fd == -1)
    {
        perror("open-readfile");
        return -1;
    }

    // 2. 打开不存在的文件, 将其创建出来, 将从english.txt读出的内容写入这个文件中
    int dst_fd = open("./data/dst_file.txt", O_WRONLY|O_CREAT, 0664);
    if(dst_fd == -1)
    {
        perror("open-writefile");
        return -1;
    }

    // 3. 循环读文件, 循环写文件
    char buf[1024] = {0};
    int len = -1;
    int times = 1;
    while((len = read(src_fd, buf, sizeof(buf))) > 0)
    {
        // 写
        printf("%d times write!\n", times++); // 只写了一次就够了，因为buf比较大 
        // 如果 把 buf 改小就要写多次了
        write(dst_fd, buf, len);
    }

    // 关闭
    close(src_fd);
    close(dst_fd);

    return 0;
}