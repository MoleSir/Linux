#ifndef _CTCP_H_
#define _CTCP_H_

//===================================================// 
//                       服务器                      //
//===================================================// 

/**
 * @brief 绑定本地addr
 * @param lfd 监听用的文件描述符
 * @param port 端口号 */
int bindSocket(int lfd, unsigned short port);


/**
 * @brief 设置监听
 * @param lfd 监听用的文件描述符 */
int setListen(int lfd);


/**
 * @brief 阻塞并等待客户端的连接
 * @param lfd 监听用的文件描述符
 * @param addr 客户端地址信息指针 */
int acceptConn(int lfd, struct sockaddr_in *addr);



//===================================================// 
//                       客户端                      //
//===================================================//

/**
 * @brief 绑定本地addr
 * @param lfd 监听用的文件描述符
 * @param port 端口号 */
int connectToHost(int fd, const char* ip, unsigned short port);




//===================================================// 
//                       公用                        //
//===================================================// 

/**
 * @brief 创建socket 
 * @return int 返回创建的文件描述符 */
int createSocket();


/**
 * @brief 发送数据
 * @param fd 通信用的文件描述符
 * @param msg 需要发送的数据 */
int sendMsg(int fd, const char* msg);


/**
 * @brief 接收数据
 * @param fd 通信用的文件描述符
 * @param msg 需要发送的数据 
 * @param size msg的大小 */
int recvMsg(int fd, char* msg, int size);


/**
 * @brief 关闭socket
 * @param fd 通信用的文件描述符 */
int closeSocket(int fd);


/**
 * @brief 读取数据（内部函数）
 * @param fd 通信用的文件描述符
 * @param msg 需要读取的数据 
 * @param size 数据大小*/
int readn(int fd, char* buf, int size);


/**
 * @brief 发送数据（内部函数）
 * @param fd 通信用的文件描述符
 * @param msg 需要发送的数据 
 * @param size msg的容量*/
int writen(int fd, const char* msg, int size);


#endif