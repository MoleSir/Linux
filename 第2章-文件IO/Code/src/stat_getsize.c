#include <sys/stat.h>
#include <stdio.h>

int main()
{
    // 1. 定义结构体, 存储文件信息
    struct stat myst;

    // 2. 获取文件属性 english.txt
    int ret = stat("./data/src_file.txt", &myst);
    if(ret == -1)
    {
        perror("stat");
        return -1;
    }

    printf("文件大小: %d\n", (int)myst.st_size);

    return 0;
}