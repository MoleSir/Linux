#include "cpptcp.h"

#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <iostream>


namespace Tcp
{
    /**
     * @brief 构造函数 */
    TcpServer::TcpServer()
    {
        // 创建监听的socket
        this->_lfd = socket(AF_INET, SOCK_STREAM, 0);
    }


    /**
     * @brief 析构函数 */
    TcpServer::~TcpServer()
    {
        // 如果文件描述符存在，释放
        if (this->_lfd > 0)
        {
            close(this->_lfd);
            this->_lfd = 0;
        }
    }


    /**
     * @brief 设置为监听
     * @param port 端口号 
     * @return 成功0，失败-1 */
    int TcpServer::SetListen(unsigned short port) const
    {
        // 创建本地地址
        struct sockaddr_in locaddr;
        // 使用IPV4地址格式
        locaddr.sin_family = AF_INET;
        // 设置端口，先转换字节序
        locaddr.sin_port = htons(port);
        // 默认本机IP
        locaddr.sin_addr.s_addr = INADDR_ANY;

        // 调用bind
        int ret = bind(this->_lfd, (struct sockaddr*)&locaddr, sizeof(locaddr));
        if (ret == -1)
        {
            perror("bind");
            return -1;
        }

        // 信息提示
        std::cout << "套接字绑定成功, ip " 
                  << inet_ntoa(locaddr.sin_addr)
                  << ", port: " << port << std::endl;

        // 调用listen函数监听
        ret = listen(this->_lfd, 128);
        if (ret == -1)
        {
            perror("listen");
            return -1;
        }

        // 信息提示
        std::cout << "设置监听成功..." << std::endl;
        return ret;
    } 


    /**
     * @brief 阻塞等待客户端连接 
     * @param socketInfo 传出参数，保持客户端地址、用于通信的TcpSocket */
    void TcpServer::AcceptConnect(SocketInfo* socketInfo)
    {
        socklen_t addrlen = sizeof(struct sockaddr_in);

        // 调用accept堵塞等待客户端连接
        int cfd = accept(this->_lfd, (struct sockaddr*)(&socketInfo->_addr), &addrlen);
        // 检测执行结果
        if (cfd == -1)
        {
            perror("accept");
        }
        else
        {
            printf("成功和客户端建立连接...\n");
        }

        // 创建用于通信的TcpSocket，失败cfd == -1
        socketInfo->_socket = TcpSocket(cfd);
    }
}