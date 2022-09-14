#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

// 信号处理函数
void callback(int num)
{
    while(1)
    {
        pid_t pid = waitpid(-1, NULL, WNOHANG);
        if (pid <= 0)
        {
            printf("子进程正在运行，或者子进程被回收完毕了\n");
            break;
        }
        printf("child die, pid = %d\n", pid);
    }
}

// 子进程工作函数
int childWork(int cfd);


int main()
{
    // 1、创建监听套接字
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
    {
        perror("socket");
        return -1;
    }

    // 2、绑定
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10000);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    int ret = bind(lfd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("bind");
        return -1;
    }

    // 设置监听
    ret = listen(lfd, 128);
    if (ret == -1)
    {
        perror("listen");
        return -1;
    }

    // 注册信号捕获
    struct sigaction act;
    act.sa_flags = 0;
    act.sa_handler = callback;
    sigemptyset(&act.sa_mask);
    sigaction(SIGCHLD, &act, NULL);

    // 接收多个客户连接，循环调用accept
    while(1)
    {
        // 4、阻塞等待并接受客户端的连接
        struct sockaddr_in cliaddr;
        int clilen = sizeof(cliaddr);
        int cfd = accept(lfd, (struct sockaddr*)&cliaddr, &clilen);
        if (cfd == -1)
        {
            if (errno == EINTR)
            {
                // accept 调用被信号中断，解除阻塞，返回-1
                // 重新调用
                continue;
            }
            perror("accept");
            return -1;
        }
        
        // 连接成功，打印客户端信息
        char ip[24] = {0};
        printf("客户端的IP地址: %s, 端口: %d\n", 
                inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip, sizeof(ip)),
                ntohs(cliaddr.sin_port));

        // 新的连接已经建立，创建子进程，让子进程和客户端通信
        pid_t pid = fork();
        if (pid == 0)
        {
            // 进入子进程->和客户端通信
            // 通信的文件描述符cfd被拷贝到子进程
            // 子进程不需要监听
            close(lfd);
            while(1)
            {
                int ret = childWork(cfd);
                if (ret <= 0)
                {
                    break;
                }
            }
            
            // 退出子进程
            close(cfd);
            return 0;
        }
        else if (pid > 0)
        {
            // 父进程不和客户端通信
            close(cfd);
        }
    }

    return 0;
}


// 5、客户端通信函数
int childWork(int cfd)
{
    // 接受数据
    char buf[64];
    memset(buf, 0, sizeof(buf));
    // 读取
    int len = read(cfd, buf, sizeof(buf));
    if (len > 0)
    {
        printf("%d号客户端say: %s\n", cfd, buf);
    }
    else if (len == 0)
    {
        printf("客户端断开连接...\n");
    }
    else
    {
        perror("read");
    }

    return len;
}