#ifndef _CPP_TCP_H_
#define _CPP_TCP_H_

#include <string>
#include <arpa/inet.h>

namespace Tcp
{
// TCP中用于通信的类，需要让客户端或服务器端把通信用的文件描述符传递进来
// TcpSocket 对连接的建立一无所知，其只负责向这个文件描述符读，写
class TcpSocket
{
public:
    TcpSocket() = default;

     /**
     * @brief 构造函数 
     * @param socket 直接给cfd幅值*/
    TcpSocket(int socket);   

    /**
     * @brief 析构函数 */
    ~TcpSocket();


    /**
     * @brief 发送信息
     * @param msg 发送的信息  
     * @return int 发送成功返回0，否则返回-1*/
    int SendMessage(const std::string& msg) const;


    /**
     * @brief 发送信息  
     * @return std::string 返回读取到的信息 */    
    std::string RecvMessage() const;


    /**
     * @brief 获取文件描述符 */
    int GetCfd() const;


private:
    /**
     * @brief 发送信息
     * @param buf 传出参数，读取的信息
     * @param size buf的容量  
     * @return int 成功返回0，否则返回-1*/
     int readn(char* buf, int size) const;


    /**
     * @brief 发送信息
     * @param msg 发送的信息
     * @param size 数据的大小  
     * @return int 成功返回0，否则返回-1*/
     int writen(const char* msg, int size) const;


private:
    // 通信的文件描述符
    int _cfd;
};



// 用于传递线程间信息的结构体
struct SocketInfo
{     
    TcpSocket _socket;           // 用于通信的TcpSocket结构体，其实只是一个文件描述符
    pthread_t _tid;              // 线程id
    sockaddr_in _addr;    // 客户端网络地址
};



// TCP客户端类封装
class TcpClient
{
public:
    /**
     * @brief 构造函数 */
    TcpClient();

    /**
     * @brief 析构函数 */
    ~TcpClient();


    /**
     * @brief 与服务器connect 
     * @param ip 服务器ip
     * @param port 服务器端口  
     * @return TcpSocket connect成功返回用来通信的TcpSokcet类*/
    TcpSocket ConnectToServer(std::string ip, unsigned short port) const; 

private:
    // 连接服务器用的文件描述符
    int _cfd;
};



// TCP服务器类封装
class TcpServer
{
public:
    /**
     * @brief 构造函数 */
    TcpServer();

    /**
     * @brief 析构函数 */
    ~TcpServer();


    /**
     * @brief 设置为监听
     * @param port 端口号 
     * @return 成功0，失败-1 */
    int SetListen(unsigned short port) const; 


    /**
     * @brief 阻塞等待客户端连接 
     * @param socketInfo 传出参数，保持客户端地址、用于通信的TcpSocket */
    void AcceptConnect(SocketInfo* socketInfo);


private:
    // 监听用的文件描述符
    int _lfd;
};


/**
* @brief 线程工作函数
* @param arg 通信信息的地址
* @return void*  */
void* working(void* arg);

}


#endif