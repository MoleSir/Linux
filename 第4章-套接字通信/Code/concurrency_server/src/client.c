// client.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


int main()
{
    // 创建socket
    int cfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cfd == -1)
    {
        perror("socket");
        return -1;
    }

    // 连接服务器
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10000);
    // "127.0.0.1" 为本机回环IP
    inet_pton(AF_INET, "127.0.0.1", &(addr.sin_addr.s_addr));

    int ret = connect(cfd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1)
    {
        perror("connect");
        exit(0);
    }

    // 和服务器通信
    int number = 0;
    while(1)
    {
        // 发送数据
        char buf[128];

        // 用户输入数据
        printf("请输入需要发送的数据：");
        scanf("%s", buf);
        
        // 检查退出
        if (strcmp(buf, "exit") == 0)
            break;

        // 发送数据
        write(cfd, buf, strlen(buf) + 1);
        printf("客户端发送数据完毕！\n");
        
        sleep(1);   // 每隔1s发送一条数据
    }

    close(cfd);

    return 0;
}