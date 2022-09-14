#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENT 16

// 主线程创建通信文件描述符后，需要向子进程传递参数，把参数打包为结构体，通过 void* 传入
typedef struct 
{
    int fd;                         // 通信文件描述符
    pthread_t tid;                  // 线程 ID
    struct sockaddr_in addr;        // 地址信息 
} sockinfo_t;

sockinfo_t infos[MAX_CLIENT];


// 线程工作函数
void* working(void* arg)
{
    // 读取参数
    sockinfo_t* info = (sockinfo_t*)arg;
    // 接受数据的空间
    char buf[128] = {0};
    while(1)
    {
        // 读取数据
        int len = read(info->fd, buf, sizeof(buf));
        if (len > 0)
        {
            printf("%ld号客户端say: %s\n", info->tid, buf);
        }
        else if (len == 0)
        {
            printf("客户端断开连接...\n");
        }
        else
        {
            perror("read");
        }
            
    }
    close(info->fd);
    return NULL;
}


int main()
{
    // 创建监听套接字
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
    {
        perror("socket");
        return 0;
    }

    // 绑定
    struct sockaddr_in locaddr;
    locaddr.sin_family = AF_INET;
    locaddr.sin_port = htons(10000);
    locaddr.sin_addr.s_addr = INADDR_ANY; 

    int ret = bind(lfd, (struct sockaddr*)&locaddr, sizeof(locaddr));
    if (ret == -1)
    {
        perror("bind");
        return -1;
    }

    // 监听
    ret = listen(lfd, 128);

    // 等待接收连接
    // 初始化结构体
    int len = sizeof(struct sockaddr_in);
    for (int i = 0; i < MAX_CLIENT; ++i)
    {
        bzero(&infos[i], sizeof(infos[i]));
        infos[i].fd = -1;
        infos[i].tid = -1;
    }
    // 循环 accept
    while(1)
    {
        // 创建子线程
        sockinfo_t* pinfo = NULL;
        // 获取一个空闲的结构体
        for (int i = 0; i < MAX_CLIENT; ++i)
        {
            if (infos[i].fd == -1)
            {
                pinfo = infos + i;
                break;;
            }
            if (i == MAX_CLIENT -1)
            {
                sleep(1);
                i--;
            }
        }
        // 阻塞accept
        int cfd = accept(lfd, (struct sockaddr*)&(pinfo->addr), &len);
        if (cfd == -1)
        {
            perror("accept");
            return 0;
        }
        // 正常accept
        printf("parent thread, cfd: %d\n", cfd);
        pinfo->fd = cfd;
        pthread_create(&(pinfo->tid), NULL, working, pinfo);
        pthread_detach(pinfo->tid);
    }

    close(lfd);

    return 0;
}

