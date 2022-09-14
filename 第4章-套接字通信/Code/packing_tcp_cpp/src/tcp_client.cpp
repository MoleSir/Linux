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
    TcpClient::TcpClient()
    {
        // 创建一个文件描述符
        // 使用IPV4格式的tcp通信
        this->_cfd = socket(AF_INET, SOCK_STREAM, 0);
    }


    /**
     * @brief 析构函数 */
    TcpClient::~TcpClient()
    {  
        // 如果文件描述符存在，释放
        if (this->_cfd > 0)
        {
            close(this->_cfd);
            this->_cfd = 0;
        }
    }


    /**
     * @brief 与服务器connect 
     * @param ip 服务器ip
     * @param port 服务器端口  
     * @return TcpSocket connect成功返回用来通信的TcpSokcet类*/
    TcpSocket TcpClient::ConnectToServer(std::string ip, unsigned short port) const
    {
        // 创建服务器地址
        struct sockaddr_in seraddr;
        // 使用IPV4地址格式
        seraddr.sin_family = AF_INET;
        // 设置端口，先转换字节序
        seraddr.sin_port = htons(port);
        // 将ip转为网络中流动的数值，后设置
        inet_pton(AF_INET, ip.data(), &seraddr.sin_addr.s_addr);
        
        // 调用connect函数
        int ret = connect(this->_cfd, (struct sockaddr*)&seraddr, sizeof(seraddr));
        // 判断是否成功
        if (ret == -1)
        {
            perror("connect");
            // conncet失败，返回一个带有-1文件描述符的socket
            return TcpSocket(-1);
        }

        // connect成功打印信息
        std::cout << "成功和服务器建立连接" << std::endl;

        // 把有效的可以通信的文件描述符交给 TcpSocket，客户端类的任务完成
        return TcpSocket(this->_cfd);
    } 
}


