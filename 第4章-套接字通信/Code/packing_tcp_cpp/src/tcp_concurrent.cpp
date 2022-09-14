#include "cpptcp.h"

#include <iostream>
#include <unistd.h>


namespace Tcp
{
/**
* @brief 线程工作函数
* @param arg 通信信息的地址
* @return void*  */
void* working(void* arg)
{
    // 转换地址
    SocketInfo* socketInfo = (SocketInfo*)arg;
    TcpSocket* socket = &(socketInfo->_socket);
    pthread_t tid = socketInfo->_tid;
    int fd = socketInfo->_socket.GetCfd();

    // 接受数据的空间
    char buf[128] = {0};
    while(1)
    {
        // 读取数据
        std::cout << "等待发送信息...\n";
        //int len = read(fd, buf, sizeof(buf));
        std::string msg = socket->RecvMessage();
        std::cout << tid << "号客户端say: " << msg << "\n\n"; 
    }
    
    return NULL;
}

}