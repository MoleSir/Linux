#include <sys/stat.h>
#include <stdio.h>

int main()
{
    // 定义结构体
    struct stat myst;
    
    // 获取信号
    int ret = stat("./data/src_file.txt", &myst);
    if(ret == -1)
    {
        perror("stat");
        return -1;
    }

    // 判断文件类型
    if(S_ISREG(myst.st_mode))
    {
        printf("这个文件是一个普通文件...\n");
    }

    if(S_ISDIR(myst.st_mode))
    {
        printf("这个文件是一个目录...\n");
    }
    if(S_ISLNK(myst.st_mode))
    {
        printf("这个文件是一个软连接文件...\n");
    }

    return 0;
}