// c++封装的tcp 客户端测试
#include "cpptcp.h"
#include <iostream>

int main()
{
    // 创建客户端
    Tcp::TcpClient client;
    
    // 尝试connect服务器
    Tcp::TcpSocket socket = client.ConnectToServer("127.0.0.1", 10000);
    // 判断是否connect成功
    if (socket.GetCfd() == -1)
    {
        // conncet失败，结束程序
        std::cout << "conncet失败！" << std::endl;
        return 0;
    }

    // conncet成功，可以使用 socket进行通信
    char buf[256] = {0};
    while (1)
    {   
        // 输入发送的信息
        std::cout << "输入需要发送的信息：" << std::endl;
        std::cin >> buf;

        if (strcmp(buf, "exit") == 0)
        {
            break;
        }

        // 发出去
        int ret = socket.SendMessage(buf);
        if (ret == -1)
        {
            break;
        }
        std::cout << "发送成功" << std::endl;
    }
}
