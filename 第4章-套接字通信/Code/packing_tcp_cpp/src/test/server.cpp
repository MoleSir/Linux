#include "cpptcp.h"

#include <iostream>
#include <pthread.h>
#include <cstring>

using namespace Tcp;

// 客户端最大数量
#define MAX_CLIENT 16
// 储存指针
Tcp::SocketInfo* pInfo[MAX_CLIENT];
// 当前客户端数量
int nClient = 0;


int main()
{
    // 数组清零
    memset(pInfo, 0, MAX_CLIENT * sizeof(Tcp::SocketInfo));

    // 创建一个服务器
    Tcp::TcpServer server;

    // 设置聆听，其中包括绑定
    int ret = server.SetListen(10000);
    if (ret == -1) return 0;




    // 循环
    while(1)
    {
        // 创建一个子进程信息结构体
        Tcp::SocketInfo* socketInfo = new Tcp::SocketInfo;
        pInfo[nClient++] = socketInfo;

        // 阻塞等待接收accept
        server.AcceptConnect(socketInfo);
        if (socketInfo->_socket.GetCfd() == -1)
        {
            std::cout << "Accept失败" << std::endl;
            return 0;
        }

        // 正常accept，创建子线程
        std::cout << "a new client!" << std::endl;

        // 创建子线程
        pthread_create(&(socketInfo->_tid), NULL, Tcp::working, socketInfo);
        pthread_detach(socketInfo->_tid);
    }

    // 释放地址空间
    for (int i = 0; i < nClient; ++i)
        delete pInfo[i];


    return 0;
}