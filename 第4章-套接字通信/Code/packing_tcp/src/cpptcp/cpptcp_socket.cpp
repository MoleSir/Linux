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
    TcpSocket::TcpSocket(int socket): _cfd(socket)
    {
    }


    /**
     * @brief 析构函数 */
    TcpSocket::~TcpSocket()
    {
        // 如果文件描述符存在，释放
        if (this->_cfd > 0)
        {
            close(this->_cfd);
        }
    }



    /**
     * @brief 发送信息
     * @param msg 发送的信息  
     * @return int 发送成功返回0，否则返回-1*/
    int TcpSocket::SendMessage(const std::string& msg) const 
    {
        // 申请空间：数据长度 + 包头4字节
        char* data = new char[msg.size()];
        // 数据长度要先转为大段
        int bigLen = htonl(msg.size());

        // 复制数据
        // 复制数据长度
        memcpy(data, &bigLen, 4);
        // 复制数据
        memcpy(data + 4, msg.data(), msg.size());

        // 发送数据
        int ret = this->writen(data, msg.size() + 4);

        // 释放空间
        delete[] data;
        return ret;
    }


    /**
     * @brief 发送信息  
     * @return std::string 返回读取到的信息 */    
    std::string TcpSocket::RecvMessage() const 
    {
        // 接收数据包头
        int len = 0;
        this->readn((char*)&len, 4);
        // 转变字节序
        len = ntohl(len);
        // 打印信息
        std::cout << "数据大小：" << len << std::endl;

        // 根据长度读取分配内存
        char* buf = new char[len + 1];
        int ret = this->readn(buf, len);
        if (ret != len)
        {
            // 接收出问题
            return std::string();
        }

        buf[len] = '\0';
        std::string msg(buf);
        delete[] buf;

        return msg;
    }


    /**
     * @brief 发送信息
     * @param buf 传出参数，读取的信息
     * @param size 要读取的大小 
     * @return int 成功返回0，否则返回-1*/
    int TcpSocket::readn(char* buf, int size) const 
    {
         int nread = 0;
         int left = size;
         char* ptr = buf;

        // 循环读取直到left为0，读完了
        while(left > 0)
        {
            // read的第三个参数表示ptr最大能够读取的字节数
            // 所以开始我们就直接尝试直接读取size大小
            // 但是不一定可以一次读完，所以需要循环
            if ((nread = read(this->_cfd, ptr, left)) > 0)
            {
                // 读取成功
                // ptr后移nread字节，因为nread字节已经读完了
                ptr += nread;
                // 需要读取的字节数-nread
                left -= nread;
            }
            else if (nread == -1)
            {
                // 读取失败
                return -1;
            }
        }

        // 返回读取字节的大小
        return size;
    }


    /**
     * @brief 发送信息
     * @param msg 发送的信息
     * @param size 数据的大小  
     * @return int 成功返回发送的字节，否则返回-1*/
    int TcpSocket::writen(const char* msg, int size) const 
    {
        // 与 readn很类似
        int left = size;
        int nwrite = 0;
        const char* p = msg;

        while(left > 0)
        {
            if ((nwrite = write(this->_cfd, msg, left)) > 0)
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
}