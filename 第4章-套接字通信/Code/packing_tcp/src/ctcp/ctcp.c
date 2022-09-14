#include "ctcp.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


//===================================================// 
//                       服务器                      //
//===================================================// 

/**
 * @brief 绑定本地addr
 * @param lfd 监听用的文件描述符
 * @param port 端口号 
 * @return int 绑定成功返回0，否则返回-1 */
int bindSocket(int lfd, unsigned short port)
{
    // 获取一个本地的地址
    struct sockaddr_in locaddr;
    // 使用 IPV4 地址格式
    locaddr.sin_family = AF_INET;
    // 传入端口号，先转为网络大端序
    locaddr.sin_port = htons(port);
    // 使用 INADDR_ANY(0.0.0.0) 表示本机IP
    locaddr.sin_addr.s_addr = INADDR_ANY;

    // 调用bind函数
    int ret = bind(lfd, &(struct sockaddr*)&locaddr, sizeof(locaddr));
    // 根据返回值判断是否绑定成功
    if (ret == -1)
    {
        perror("bind");
        return -1;
    }

    printf("套接字绑定成功, ip: %s, port: %d\n", inet_ntoa(saddr.sin_addr), port);
    return ret;
}


/**
 * @brief 设置监听
 * @param lfd 监听用的文件描述符
 * @return  int 监听成功返回0，否则返回-1 */
int setListen(int lfd)
{
    // 调用listen，默认使用最大连接要求数为128
    int ret = listen(lfd, 128);
    // 判断是否设置监听成功
    if (ret == -1)
    {
        perror("listen");
        return -1;
    }
    
    // 信息提示
    printf("设置监听成功\n");
    return ret;
}


/**
 * @brief 阻塞并等待客户端的连接
 * @param lfd 监听用的文件描述符
 * @param addr 传输参数，获取客户端地址信息指针 
 * @return int accept成功返回通信用的文件描述符，失败返回-1 */
int acceptConn(int lfd, struct sockaddr_in *addr)
{
    // 参数检测
    int cfd = -1;
    if(addr == NULL)
    {
        cfd = accept(lfd, NULL, NULL);
    }
    else
    {
        // 调用accept函数
        int addrlen = sizeof(struct sockaddr_in);
        cfd = accept(lfd, (struct sockaddr*)addr, &addrlen);
    }

    // 判断是否成功接收
    if (cfd == -1)
    {
        perror("accept");
        return -1;
    }

    // 信息提示
    printf("成功和客户端建立连接...\n");
    return cfd; 
}



//===================================================// 
//                       客户端                      //
//===================================================//

/**
 * @brief 客户端连接服务器
 * @param cfd 客户端通信描述符文件描述符
 * @param ip 服务器的ip地址 
 * @param port 服务器的端口 
 * @return int 成功返回0，失败返回-1*/
int connectToHost(int cfd, const char* ip, unsigned short port)
{
    // 构造一个服务器地址
    struct sockaddr_in seraddr;
    // 使用IPV4地址
    seraddr.sin_family = AF_INET;
    // 设置服务器端口号, 需要序列顺序转换
    seraddr.sin_port = htons(port);
    // 设置ip地址 把IPV4地址ip转为网络地址，填入s_addr
    inet_pton(AF_INET, ip, &seraddr.sin_addr.s_addr);

    // 创建连接, 把服务器地址填入
    int ret = connect(cfd, (struct sockaddr*)&seraddr, sizeof(seraddr));
    if (ret == -1)
    {
        perror("connect");
        return -1;
    }

    // 信息提示
    printf("成功与服务器建立连接\n");
    return ret;
}




//===================================================// 
//                       公用                        //
//===================================================// 

/**
 * @brief 创建socket 
 * @return int 返回创建的文件描述符 如果创建失败，返回-1 */
int createSocket()
{
    // 创建tcp文件描述符：IPV4地址，使用默认的流协议——tcp
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    // 检测是否创建成功
    if (lfd == -1)
    {
        perror("socket");
        return -1;
    }

    // 提示用户传创建成功
    printf("套接字创建成功, 对应文件描述符fd = %d\n", lfd);
    return lfd;
}


/**
 * @brief 发送数据
 * @param fd 通信用的文件描述符
 * @param msg 需要发送的数据 */
int sendMsg(int fd, const char* msg)
{
    if(msg == NULL || len <= 0)
    {
       return -1;
    }
    // 申请内存空间: 数据长度 + 包头4字节(存储数据长度)
    char* data = (char*)malloc(len+4);
    int bigLen = htonl(len);
    memcpy(data, &bigLen, 4);
    memcpy(data+4, msg, len);
    // 发送数据
    int ret = writen(cfd, data, len+4);
    return ret;
}


/**
 * @brief 接收数据
 * @param cfd 通信用的文件描述符
 * @param msg 需要发送的数据 */
int recvMsg(int cfd, char** msg)
{
    // 参数检测
    if(msg == NULL || cfd <= 0)
    {
        return -1;
    }
    // 接收数据
    // 1. 读数据头
    int len = 0;
    readn(cfd, (char*)&len, 4);
    len = ntohl(len);
    printf("数据块大小: %d\n", len);

    // 根据读出的长度分配内存
    char *buf = (char*)malloc(len+1);
    int ret = readn(cfd, buf, len);
    if(ret != len)
    {
        return -1;
    }
    buf[len] = '\0';
    *msg = buf;

    return ret;
}


/**
 * @brief 关闭socket
 * @param fd 通信用的文件描述符 */
int closeSocket(int fd)
{
    // 调用close关闭文件描述符
    int ret = close(fd);
    // 检测是否是否成功
    if(ret == -1)
    {
        perror("close");
    }
    return ret;
}


/**
 * @brief 读取数据（内部函数）
 * @param fd 通信用的文件描述符
 * @param buf 需要读取的数据 
 * @param size 数据大小*/
int readn(int fd, char* buf, int size)
{
    int nread = 0;
    int left = size;
    char* p = buf;

    while(left > 0)
    {
        if((nread = read(fd, p, left)) > 0)
        {
            p += nread;
            left -= nread;
        }
        else if(nread == -1)
        {
            return -1;
        }
    }
    return size;
}


/**
 * @brief 发送数据（内部函数）
 * @param fd 通信用的文件描述符
 * @param msg 需要发送的数据 
 * @param size msg的容量*/
int writen(int fd, const char* msg, int size)
{
    int left = size;
    int nwrite = 0;
    const char* p = msg;

    while(left > 0)
    {
        if((nwrite = write(fd, msg, left)) > 0)
        {
            p += nwrite;
            left -= nwrite;
        }
        else if(nwrite == -1)
        {
            return -1;
        }
    }
    return size;
}