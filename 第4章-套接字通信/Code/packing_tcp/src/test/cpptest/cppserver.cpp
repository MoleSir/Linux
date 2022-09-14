#include "cpptcp.h"

int main()
{
    // 创建一个服务器
    Tcp::TcpServer server;

    // 设置聆听
    int ret = server.SetListen(10000);
    if (ret == -1) return 0;

    // 阻塞等待连接
    Tcp::TcpSocket socket = server.AcceptConnect();
    if (socket.GetCfd() == -1) return 0;

    while (1)
    {
        std::cout << "接收数据: .....\n";
        std::string msg = socket.RecvMessage();
        if (!msg.empty())
        {
            cout << msg << endl << endl << endl;
        }
        else
        {
            break;
        }
    }

    return 0;
}